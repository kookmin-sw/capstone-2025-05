from fastapi import APIRouter, HTTPException, UploadFile, File, Form, Body
from fastapi.responses import FileResponse
from manager.firebase_manager import firestore_db, storage_bucket
from typing import Optional
from datetime import datetime
import uuid
import os
import tempfile
from pydantic import BaseModel
import json
import logging

router = APIRouter()

@router.get("/results-all", tags=["Analysis"])
async def get_all_results(uid: str):
    user_ref = firestore_db.collection("analysis_results").document(uid)
    results = list(user_ref.collections())

    if not results:
        raise HTTPException(status_code=404, detail="UID not found")
    
    output = []
    for result in results:
        result_id = result.id
        for task_doc in result.stream():
            output.append({"result_id": result_id, "task_id": task_doc.id})
    return {"uid": uid, "results": output}

@router.get("/result-one", tags=["Analysis"])
async def get_one_result(uid: str, result_id: str):
    result_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    docs = result_ref.stream()
    data = [{**doc.to_dict(), "task_id": doc.id} for doc in docs]
    if not data:
        raise HTTPException(status_code=404, detail="Result not found")
    return data[0]

MAX_FIRESTORE_DOC_SIZE = 1_000_000

def safe_json_loads(json_str):
    try:
        return json.loads(
            json_str,
            parse_constant=lambda x: 0.0  # "NaN", "Infinity" 등을 0.0으로 처리
        )
    except Exception as e:
        raise HTTPException(status_code=400, detail=f"JSON 파싱 오류: {str(e)}")

def clean_firestore_dict(obj, depth=0, max_depth=15):
    if depth > max_depth:
        return str(obj)
    if isinstance(obj, dict):
        return {
            str(k): clean_firestore_dict(v, depth + 1, max_depth)
            for k, v in obj.items()
            if v is not None
        }
    elif isinstance(obj, list):
        return [clean_firestore_dict(v, depth + 1, max_depth) for v in obj if v is not None]
    elif isinstance(obj, float):
        if obj != obj or obj == float('inf') or obj == float('-inf'):
            return 0.0
        return obj
    elif isinstance(obj, datetime):
        return obj.isoformat()
    elif isinstance(obj, (str, int, bool, type(None))):
        return obj
    else:
        return str(obj)

@router.post("/results-save", tags=["Analysis"])
async def save_result(
    result_id: str = Form(...),
    uid: str = Form(...),
    song_id: str = Form(...),
    analysis_type: Optional[str] = Form(None),
    status: Optional[str] = Form(None),
    progress: Optional[int] = Form(0),
    analysis_json: str = Form(...),
    compare_json: str = Form(...),
    score: float = Form(...),
    created_at: datetime = Form(...),
    completed_at: datetime = Form(...),
    duration_sec: Optional[float] = Form(None),
    error_message: Optional[str] = Form(None)
):
    try:
        # JSON 파싱 및 정제
        analysis_data = clean_firestore_dict(safe_json_loads(analysis_json))

        # compare_json은 구조가 복잡하므로 문자열로 저장
        compare_data_dict = safe_json_loads(compare_json)
        compare_data_cleaned = clean_firestore_dict(compare_data_dict)

        # 불필요한 feedback 필드 제거 (만약 존재하면)
        compare_data_cleaned.get("comparison", {}).pop("feedback", None)

        # 문자열로 저장
        compare_data = {
            "compare_json_string": json.dumps(compare_data_cleaned, ensure_ascii=False)
        }

        logging.warning("COMPARE_JSON 저장 문자열: %s", compare_data["compare_json_string"])

        # Firestore 문서 생성
        task_id = str(uuid.uuid4())
        doc_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id).document(task_id)

        doc_ref.set({
            "result_id": result_id,
            "uid": uid,
            "song_id": song_id,
            "analysis_type": analysis_type,
            "status": status,
            "progress": progress,
            "analysis_json": analysis_data,
            "compare_json": compare_data,
            "score": score,
            "created_at": created_at.isoformat(),
            "completed_at": completed_at.isoformat(),
            "duration_sec": duration_sec,
            "error_message": error_message
        })

        # 점수 저장
        try:
            pitch_score = int(compare_data_dict["comparison"]["scores"]["pitch_match_percentage"])
            rhythm_score = int(compare_data_dict["comparison"]["scores"]["rhythm_match_percentage"])
            technique_score = int(compare_data_dict["comparison"]["scores"]["technique_match_percentage"])

            count_id = str(uuid.uuid4())
            score_ref = firestore_db.collection(f"{uid}_score").document(song_id).collection(count_id).document("score")
            score_ref.set({
                "date": datetime.utcnow().isoformat(),
                "pitch": pitch_score,
                "technique": technique_score,
                "onset": rhythm_score,
                "accuracy": score
            })

        except Exception as e:
            raise HTTPException(status_code=400, detail=f"점수 저장 실패: {str(e)}")

        return {
            "message": "분석 결과 및 점수 저장 성공",
            "result_id": result_id,
            "task_id": task_id
        }

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"저장 실패: {str(e)}")
    
@router.post("/results-feedback", tags=["Analysis"])
async def upload_feedback(uid: str = Form(...), result_id: str = Form(...), file: UploadFile = File(...)):
    task_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    task_docs = list(task_ref.stream())
    if not task_docs:
        raise HTTPException(status_code=404, detail="Result not found")
    
    task_id = task_docs[0].id
    path = f"analysis_results/feedback/{uid}/{result_id}/{file.filename}"
    blob = storage_bucket.blob(path)
    blob.upload_from_file(file.file, content_type="text/plain")

    task_ref.document(task_id).update({"feedback_path": path})
    return {"message": "Feedback uploaded", "feedback_path": path}

@router.post("/record-save", tags=["Analysis"])
async def upload_record(uid: str = Form(...), result_id: str = Form(...), song_id: str = Form(...), file: UploadFile = File(...)):
    task_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    task_docs = list(task_ref.stream())
    if not task_docs:
        raise HTTPException(status_code=404, detail="Result not found")

    prefix = f"{uid}/record/{song_id}/"
    blobs = storage_bucket.list_blobs(prefix=prefix)
    indices = [int(blob.name.split("/")[-2]) for blob in blobs if blob.name.split("/")[-2].isdigit()]
    new_index = max(indices + [0]) + 1

    path = f"{uid}/record/{song_id}/{new_index}/{file.filename}"
    blob = storage_bucket.blob(path)
    blob.upload_from_file(file.file, content_type=file.content_type)

    task_id = task_docs[0].id
    task_ref.document(task_id).update({"record_path": path})
    return {"message": "Record uploaded", "record_path": path}

@router.delete("/results-delete", tags=["Analysis"])
async def delete_result(uid: str, result_id: str):
    result_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    task_docs = list(result_ref.stream())
    if not task_docs:
        raise HTTPException(status_code=404, detail="Result not found")
    
    task_id = task_docs[0].id
    data = task_docs[0].to_dict()
    feedback_path = data.get("feedback_path")

    if feedback_path:
        blob = storage_bucket.blob(feedback_path)
        blob.delete()

    result_ref.document(task_id).delete()
    return {"message": "Result and feedback deleted"}

@router.get("/feedback", tags=["Analysis"])
async def download_feedback(uid: str, result_id: str):
    result_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    docs = list(result_ref.stream())
    if not docs:
        raise HTTPException(status_code=404, detail="Result not found")

    feedback_path = docs[0].to_dict().get("feedback_path")
    if not feedback_path:
        raise HTTPException(status_code=404, detail="No feedback path found")

    blob = storage_bucket.blob(feedback_path)
    _, ext = os.path.splitext(feedback_path)
    temp_file = tempfile.NamedTemporaryFile(delete=False, suffix=ext)
    blob.download_to_filename(temp_file.name)
    return FileResponse(temp_file.name, filename=os.path.basename(feedback_path))