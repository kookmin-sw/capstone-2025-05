from fastapi import APIRouter, BackgroundTasks, Depends, File, HTTPException, Path, UploadFile, status, Query, Form
from fastapi.responses import JSONResponse
from typing import Optional, List, Dict, Any
import io
import os
from tempfile import NamedTemporaryFile

# bson 모듈을 조건부로 임포트 (테스트 환경에서는 모의 객체 사용)
try:
    from bson import ObjectId
    from bson.errors import InvalidId
except (ImportError, ModuleNotFoundError):
    # 테스트를 위한 가짜 ObjectId 클래스
    class ObjectId:
        def __init__(self, oid=None):
            self.oid = oid
        
        def __str__(self):
            return str(self.oid)
    
    class InvalidId(Exception):
        pass

from app.schemas import AnalysisRequest, AnalysisType, TaskResponse, ProgressResponse, AnalysisResultResponse
from workers.tasks import analyze_audio, compare_audio, analyze_reference_audio
from app.db import (
    get_analysis_result, get_comparison_result, get_result,
    get_user_analysis_results, get_user_comparison_results,
    get_song_analysis_results, get_song_comparison_results,
    get_reference_features, get_reference_features_list, delete_reference_features
)

router = APIRouter(prefix="/v1")


@router.post("/analyze", response_model=TaskResponse)
async def analyze(
    background_tasks: BackgroundTasks,
    file: UploadFile = File(...),
    analysis_type: AnalysisType = AnalysisType.SIMPLE,
    user_id: Optional[str] = None,
    song_id: Optional[str] = None,
    generate_feedback: bool = False
):
    """
    Submit an audio file for analysis.
    Returns a task ID that can be used to track progress and retrieve results.
    
    Parameters:
    - file: Audio file to analyze (WAV or MP3)
    - analysis_type: Type of analysis to perform
    - user_id: Optional user identifier
    - song_id: Optional song identifier
    - generate_feedback: Whether to generate textual feedback using GROK API
    """
    if not file.filename.lower().endswith(('.wav', '.mp3')):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Only WAV and MP3 files are supported"
        )
    
    contents = await file.read()
    
    # Create analysis request
    request = AnalysisRequest(
        analysis_type=analysis_type,
        user_id=user_id,
        song_id=song_id,
        generate_feedback=generate_feedback  # 피드백 생성 옵션 추가
    )
    
    # Submit to Celery task queue
    task = analyze_audio.delay(
        audio_bytes=contents,
        request_data=request.model_dump()
    )
    
    return {"task_id": task.id}


@router.post("/compare", response_model=TaskResponse)
async def compare(
    background_tasks: BackgroundTasks,
    user_file: UploadFile = File(...),
    user_id: Optional[str] = Form(None),
    song_id: Optional[str] = Form(None),
    generate_feedback: bool = Form(False)
):
    """
    사용자 연주를 레퍼런스 오디오 또는 MIDI와 비교합니다.
    
    Parameters:
    - user_file: 사용자의 오디오 파일 (WAV 또는 MP3)
    - user_id: 사용자 ID (선택 사항)
    - song_id: 곡 ID (선택 사항)
    - generate_feedback: 피드백 생성 여부
    
    Returns:
    - task_id: 분석 작업의 ID
    """
    if not user_file.filename.lower().endswith(('.wav', '.mp3')):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Only WAV and MP3 files are supported"
        )
    
    user_contents = await user_file.read()
    
    # Submit to Celery task queue
    task = compare_audio.delay(
        user_audio_bytes=user_contents,
        user_id=user_id,
        song_id=song_id,
        generate_feedback=generate_feedback
    )
    
    return {"task_id": task.id}


@router.get("/tasks/{task_id}", response_model=ProgressResponse)
async def get_task_status(
    task_id: str = Path(..., description="The ID of the task to check")
):
    """
    Get the status and progress of a task.
    If the task is complete, this will include the analysis results.
    """
    task = analyze_audio.AsyncResult(task_id)
    response = ProgressResponse(
        task_id=task_id,
        status=task.status,
        progress=0
    )
    
    if task.state == 'PENDING':
        return response
    
    if task.state == 'SUCCESS':
        response.progress = 100
        response.result = task.result
        return response
    
    if task.state == 'FAILURE':
        response.error = str(task.result)
        return response
    
    # For tasks in progress, get progress information
    if task.info and isinstance(task.info, dict) and 'progress' in task.info:
        response.progress = task.info['progress']
    
    return response


@router.get("/results/{task_id}", response_model=AnalysisResultResponse)
async def get_result_by_id(
    task_id: str = Path(..., description="The ID of the task to get results for")
):
    """
    MongoDB에서 분석 또는 비교 결과를 가져옵니다.
    
    Parameters:
    - task_id: 결과를 가져올 작업 ID
    
    Returns:
    - 분석 또는 비교 결과 데이터
    """
    result = get_result(task_id)
    if not result:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"No result found for task ID: {task_id}"
        )
    
    # MongoDB ObjectId를 str로 변환
    result["_id"] = str(result["_id"])
    
    return result


@router.get("/user/{user_id}/results", response_model=List[AnalysisResultResponse])
async def get_user_results(
    user_id: str = Path(..., description="사용자 ID"),
    limit: int = Query(10, ge=1, le=100, description="결과 수 제한"),
    result_type: str = Query("all", description="결과 유형 (all, analysis, comparison)")
):
    """
    특정 사용자의 분석 또는 비교 결과를 가져옵니다.
    
    Parameters:
    - user_id: 사용자 ID
    - limit: 최대 결과 수 (기본값: 10)
    - result_type: 결과 유형 (all, analysis, comparison)
    
    Returns:
    - 사용자의 분석 또는 비교 결과 목록
    """
    results = []
    
    if result_type.lower() in ["all", "analysis"]:
        analysis_results = get_user_analysis_results(user_id, limit if result_type.lower() == "analysis" else limit // 2)
        for result in analysis_results:
            result["_id"] = str(result["_id"])
            result["result_type"] = "analysis"
        results.extend(analysis_results)
    
    if result_type.lower() in ["all", "comparison"]:
        comparison_results = get_user_comparison_results(user_id, limit if result_type.lower() == "comparison" else limit // 2)
        for result in comparison_results:
            result["_id"] = str(result["_id"])
            result["result_type"] = "comparison"
        results.extend(comparison_results)
    
    # 날짜 기준으로 정렬
    results.sort(key=lambda x: x.get("created_at", ""), reverse=True)
    
    # 결과 수 제한
    results = results[:limit]
    
    return results


@router.get("/song/{song_id}/results", response_model=List[AnalysisResultResponse])
async def get_song_results(
    song_id: str = Path(..., description="곡 ID"),
    limit: int = Query(10, ge=1, le=100, description="결과 수 제한"),
    result_type: str = Query("all", description="결과 유형 (all, analysis, comparison)")
):
    """
    특정 곡의 분석 또는 비교 결과를 가져옵니다.
    
    Parameters:
    - song_id: 곡 ID
    - limit: 최대 결과 수 (기본값: 10)
    - result_type: 결과 유형 (all, analysis, comparison)
    
    Returns:
    - 곡의 분석 또는 비교 결과 목록
    """
    results = []
    
    if result_type.lower() in ["all", "analysis"]:
        analysis_results = get_song_analysis_results(song_id, limit if result_type.lower() == "analysis" else limit // 2)
        for result in analysis_results:
            result["_id"] = str(result["_id"])
            result["result_type"] = "analysis"
        results.extend(analysis_results)
    
    if result_type.lower() in ["all", "comparison"]:
        comparison_results = get_song_comparison_results(song_id, limit if result_type.lower() == "comparison" else limit // 2)
        for result in comparison_results:
            result["_id"] = str(result["_id"])
            result["result_type"] = "comparison"
        results.extend(comparison_results)
    
    # 날짜 기준으로 정렬
    results.sort(key=lambda x: x.get("created_at", ""), reverse=True)
    
    # 결과 수 제한
    results = results[:limit]
    
    return results


@router.post("/reference", response_model=TaskResponse)
async def add_reference(
    background_tasks: BackgroundTasks,
    reference_file: UploadFile = File(...),
    song_id: str = Form(...),
    midi_file: Optional[UploadFile] = File(None),
    description: Optional[str] = Form(None)
):
    """
    레퍼런스 오디오를 분석하여 특성을 추출하고 DB에 저장합니다.
    해당 레퍼런스 오디오는 이후 비교 분석에 사용할 수 있습니다.
    
    Parameters:
    - reference_file: 레퍼런스 오디오 파일 (WAV 또는 MP3)
    - song_id: 고유 곡 식별자
    - midi_file: MIDI 파일 (선택 사항)
    - description: 곡에 대한 설명 (선택 사항)
    
    Returns:
    - task_id: 분석 작업의 ID
    """
    if not reference_file.filename.lower().endswith(('.wav', '.mp3')):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="WAV와 MP3 파일만 지원됩니다"
        )
    
    reference_contents = await reference_file.read()
    
    midi_contents = None
    if midi_file:
        if not midi_file.filename.lower().endswith('.mid'):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="MIDI 파일만 지원됩니다 (.mid)"
            )
        midi_contents = await midi_file.read()
    
    # Celery 작업 큐에 제출
    task = analyze_reference_audio.delay(
        audio_bytes=reference_contents,
        song_id=song_id,
        midi_bytes=midi_contents,
        description=description
    )
    
    return {"task_id": task.id}


@router.get("/reference/{song_id}", response_model=Dict[str, Any])
async def get_reference(song_id: str):
    """
    DB에서 레퍼런스 오디오 특성 정보를 가져옵니다.
    
    Parameters:
    - song_id: 곡 식별자
    
    Returns:
    - 레퍼런스 오디오 특성 정보
    """
    reference = get_reference_features(song_id)
    if not reference:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"song_id '{song_id}'에 해당하는 레퍼런스 오디오를 찾을 수 없습니다"
        )
    
    # MongoDB의 _id 필드를 문자열로 변환
    if "_id" in reference:
        reference["_id"] = str(reference["_id"])
    
    return reference


@router.get("/references", response_model=List[Dict[str, Any]])
async def list_references(limit: int = Query(20, ge=1, le=100)):
    """
    저장된 모든 레퍼런스 오디오 목록을 가져옵니다.
    
    Parameters:
    - limit: 반환할 최대 항목 수 (기본값: 20, 최소: 1, 최대: 100)
    
    Returns:
    - 레퍼런스 오디오 특성 목록
    """
    references = get_reference_features_list(limit)
    
    # MongoDB의 _id 필드를 문자열로 변환
    for ref in references:
        if "_id" in ref:
            ref["_id"] = str(ref["_id"])
    
    return references


@router.delete("/reference/{song_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_reference(song_id: str):
    """
    DB에서 레퍼런스 오디오 특성 정보를 삭제합니다.
    
    Parameters:
    - song_id: 곡 식별자
    """
    # 먼저 레퍼런스가 존재하는지 확인
    reference = get_reference_features(song_id)
    if not reference:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"song_id '{song_id}'에 해당하는 레퍼런스 오디오를 찾을 수 없습니다"
        )
    
    # 레퍼런스 삭제
    success = delete_reference_features(song_id)
    if not success:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="레퍼런스 오디오 삭제 중 오류가 발생했습니다"
        )
    
    return None


@router.post("/compare-with-reference", response_model=TaskResponse)
async def compare_with_reference(
    background_tasks: BackgroundTasks,
    user_file: UploadFile = File(...),
    song_id: str = Form(...),
    midi_file: Optional[UploadFile] = File(None),
    user_id: Optional[str] = Form(None),
    generate_feedback: bool = Form(False)
):
    """
    사용자의 연주를 DB에 저장된 레퍼런스 오디오와 비교합니다.
    song_id를 통해 저장된 레퍼런스 오디오 특성을 불러와 사용합니다.
    
    Parameters:
    - user_file: 사용자의 오디오 파일 (WAV 또는 MP3)
    - song_id: 비교할 레퍼런스 오디오의 곡 ID
    - midi_file: MIDI 파일 (선택 사항, 레퍼런스에 저장된 MIDI가 우선함)
    - user_id: 사용자 ID (선택 사항)
    - generate_feedback: 피드백 생성 여부
    
    Returns:
    - task_id: 분석 작업의 ID
    """
    if not user_file.filename.lower().endswith(('.wav', '.mp3')):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="WAV와 MP3 파일만 지원됩니다"
        )
    
    # 먼저 song_id에 해당하는 레퍼런스 데이터가 있는지 확인
    reference_data = get_reference_features(song_id)
    if not reference_data:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"song_id '{song_id}'에 해당하는 레퍼런스 오디오를 찾을 수 없습니다"
        )
    
    user_contents = await user_file.read()
    
    midi_contents = None
    if midi_file:
        if not midi_file.filename.lower().endswith('.mid'):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="MIDI 파일만 지원됩니다 (.mid)"
            )
        midi_contents = await midi_file.read()
    
    # Celery 작업 큐에 제출
    task = compare_audio.delay(
        user_audio_bytes=user_contents,
        user_id=user_id,
        song_id=song_id,
        generate_feedback=generate_feedback
    )
    
    return {"task_id": task.id}