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
import math

router = APIRouter()

@router.get("/results-all", tags=["Analysis"])
async def get_all_results(uid: str):
    user_ref = firestore_db.collection("analysis_results").document(uid)
    results = list(user_ref.collections())

    if not results:
        raise HTTPException(status_code=404, detail="존재하지 않는 UID입니다.")
    
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
        raise HTTPException(status_code=404, detail="Result가 존재하지 않습니다.")
    return data[0]

#todo: 분석 결과 업로드
    
@router.post("/results-feedback", tags=["Analysis"])
async def upload_feedback(uid: str, result_id: str, file: UploadFile = File(...)):
    task_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    task_docs = list(task_ref.stream())
    if not task_docs:
        raise HTTPException(status_code=404, detail="Result가 존재하지 않습니다.")
    
    task_id = task_docs[0].id
    path = f"analysis_results/feedback/{uid}/{result_id}/{file.filename}"
    blob = storage_bucket.blob(path)
    blob.upload_from_file(file.file, content_type="text/plain")

    task_ref.document(task_id).update({"feedback_path": path})
    return {"피드백 파일 업로드 성공, feedback_path": path}

@router.post("/record-save", tags=["Analysis"])
async def upload_record(uid: str, result_id: str, song_id: str, file: UploadFile = File(...)):
    task_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    task_docs = list(task_ref.stream())
    if not task_docs:
        raise HTTPException(status_code=404, detail="Result가 존재하지 않습니다.")

    prefix = f"{uid}/record/{song_id}/"
    blobs = storage_bucket.list_blobs(prefix=prefix)
    indices = [int(blob.name.split("/")[-2]) for blob in blobs if blob.name.split("/")[-2].isdigit()]
    new_index = max(indices + [0]) + 1

    path = f"{uid}/record/{song_id}/{new_index}/{file.filename}"
    blob = storage_bucket.blob(path)
    blob.upload_from_file(file.file, content_type=file.content_type)

    task_id = task_docs[0].id
    task_ref.document(task_id).update({"record_path": path})
    return {"record_path": path}

@router.delete("/results-delete", tags=["Analysis"])
async def delete_result(uid: str, result_id: str):
    result_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    task_docs = list(result_ref.stream())
    if not task_docs:
        raise HTTPException(status_code=404, detail="Result가 존재하지 않습니다.")
    
    task_id = task_docs[0].id
    data = task_docs[0].to_dict()
    feedback_path = data.get("feedback_path")

    if feedback_path:
        blob = storage_bucket.blob(feedback_path)
        blob.delete()

    result_ref.document(task_id).delete()
    return {"message": "Result와 feedback 삭제가 완료되었습니다."}

@router.get("/feedback", tags=["Analysis"])
async def download_feedback(uid: str, result_id: str):
    result_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id)
    docs = list(result_ref.stream())
    if not docs:
        raise HTTPException(status_code=404, detail="Result가 존재하지 않습니다.")

    feedback_path = docs[0].to_dict().get("feedback_path")
    if not feedback_path:
        raise HTTPException(status_code=404, detail="feedback을 찾을 수 없습니다.")

    blob = storage_bucket.blob(feedback_path)
    _, ext = os.path.splitext(feedback_path)
    temp_file = tempfile.NamedTemporaryFile(delete=False, suffix=ext)
    blob.download_to_filename(temp_file.name)
    return FileResponse(temp_file.name, filename=os.path.basename(feedback_path))