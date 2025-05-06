from fastapi import APIRouter, BackgroundTasks, Depends, File, HTTPException, Path, UploadFile, status, Query
from fastapi.responses import JSONResponse
from typing import Optional, List
import io
import os
from tempfile import NamedTemporaryFile
from bson import ObjectId

from app.schemas import AnalysisRequest, AnalysisType, TaskResponse, ProgressResponse, AnalysisResultResponse, AnalysisListResponse
from workers.tasks import analyze_audio, compare_audio
from app.db import get_analysis_result, get_user_analysis_results, get_song_analysis_results

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
    task = analyze_audio.delay(contents, request.dict())
    
    return {"task_id": task.id}


@router.post("/compare", response_model=TaskResponse)
async def compare(
    background_tasks: BackgroundTasks,
    user_file: UploadFile = File(...),
    reference_file: Optional[UploadFile] = File(None),
    midi_file: Optional[UploadFile] = File(None),
    user_id: Optional[str] = None,
    song_id: Optional[str] = None,
    generate_feedback: bool = False
):
    """
    Compare a user's performance with a reference audio file and/or a MIDI file.
    Returns a task ID that can be used to track progress and retrieve results.
    
    Parameters:
    - user_file: User's audio file to analyze (WAV or MP3)
    - reference_file: Reference audio file for comparison (WAV or MP3)
    - midi_file: MIDI file for score reference
    - user_id: Optional user identifier
    - song_id: Optional song identifier
    - generate_feedback: Whether to generate textual feedback using GROK API
    """
    if not user_file.filename.lower().endswith(('.wav', '.mp3')):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Only WAV and MP3 files are supported"
        )
    
    user_contents = await user_file.read()
    
    reference_contents = None
    if reference_file:
        if not reference_file.filename.lower().endswith(('.wav', '.mp3')):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Only WAV and MP3 files are supported for reference audio"
            )
        reference_contents = await reference_file.read()
    
    midi_contents = None
    if midi_file:
        if not midi_file.filename.lower().endswith('.mid'):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Only MIDI files are supported for score references"
            )
        midi_contents = await midi_file.read()
    
    # Submit to Celery task queue
    task = compare_audio.delay(
        user_contents, 
        reference_contents, 
        midi_contents,
        user_id,
        song_id,
        generate_feedback  # 피드백 생성 옵션 추가
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
async def get_result(
    task_id: str = Path(..., description="The ID of the task to get results for")
):
    """
    MongoDB에서 분석 결과를 가져옵니다.
    
    Parameters:
    - task_id: 결과를 가져올 작업 ID
    
    Returns:
    - 분석 결과 데이터
    """
    result = get_analysis_result(task_id)
    if not result:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"No result found for task ID: {task_id}"
        )
    
    # MongoDB ObjectId를 str로 변환
    result["_id"] = str(result["_id"])
    
    return result


@router.get("/user/{user_id}/results", response_model=AnalysisListResponse)
async def get_user_results(
    user_id: str = Path(..., description="사용자 ID"),
    limit: int = Query(10, ge=1, le=100, description="결과 수 제한")
):
    """
    특정 사용자의 분석 결과를 가져옵니다.
    
    Parameters:
    - user_id: 사용자 ID
    - limit: 최대 결과 수 (기본값: 10)
    
    Returns:
    - 사용자의 분석 결과 목록
    """
    results = get_user_analysis_results(user_id, limit)
    
    # MongoDB ObjectId를 str로 변환
    for result in results:
        result["_id"] = str(result["_id"])
    
    return {"results": results, "count": len(results)}


@router.get("/song/{song_id}/results", response_model=AnalysisListResponse)
async def get_song_results(
    song_id: str = Path(..., description="곡 ID"),
    limit: int = Query(10, ge=1, le=100, description="결과 수 제한")
):
    """
    특정 곡의 분석 결과를 가져옵니다.
    
    Parameters:
    - song_id: 곡 ID
    - limit: 최대 결과 수 (기본값: 10)
    
    Returns:
    - 곡의 분석 결과 목록
    """
    results = get_song_analysis_results(song_id, limit)
    
    # MongoDB ObjectId를 str로 변환
    for result in results:
        result["_id"] = str(result["_id"])
    
    return {"results": results, "count": len(results)}