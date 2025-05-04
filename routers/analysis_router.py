from fastapi import APIRouter, HTTPException, UploadFile, File, Form
from fastapi.responses import FileResponse
from manager.firebase_manager import firestore_db, storage_bucket
from typing import Optional
from datetime import datetime
import uuid
import os
import tempfile

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

@router.post("/results-save", tags=["Analysis"])
async def save_analysis_result(
    uid: str,
    result_id: str,
    song_id: str,
    score: float,
    analysis_type: Optional[str] = Form(None),
    status: Optional[str] = Form(None),
    progress: Optional[int] = Form(0),
    created_at: Optional[str] = Form(None),
    completed_at: Optional[str] = Form(None),
    duration_sec: Optional[float] = Form(None),
    error_message: Optional[str] = Form(None),
    analysis_json: UploadFile = File(...),
    compare_json: UploadFile = File(...)
):
    try:
        import json
        with tempfile.NamedTemporaryFile(delete=False) as analysis_temp:
            analysis_temp.write(await analysis_json.read())
            analysis_path = analysis_temp.name

        with tempfile.NamedTemporaryFile(delete=False) as compare_temp:
            compare_temp.write(await compare_json.read())
            compare_path = compare_temp.name

        with open(compare_path, "r") as f:
            compare_data = json.load(f)

        try:
            pitch_score = int(compare_data["comparison"]["scores"]["pitch_match_percentage"])
            rhythm_score = int(compare_data["comparison"]["scores"]["rhythm_match_percentage"])
            technique_score = int(compare_data["comparison"]["scores"]["technique_match_percentage"])
        except (KeyError, ValueError, TypeError) as e:
            raise HTTPException(status_code=400, detail=f"점수 추출 오류: {str(e)}")

        analysis_blob_path = f"analysis_results/analysis/{uid}/{result_id}/analysis.json"
        compare_blob_path = f"analysis_results/compare/{uid}/{result_id}/compare.json"

        analysis_blob = storage_bucket.blob(analysis_blob_path)
        compare_blob = storage_bucket.blob(compare_blob_path)
        analysis_blob.upload_from_filename(analysis_path)
        compare_blob.upload_from_filename(compare_path)

        task_id = str(uuid.uuid4())
        doc_ref = firestore_db.collection("analysis_results").document(uid).collection(result_id).document(task_id)

        data_to_store = {
            "uid": uid,
            "result_id": result_id,
            "task_id": task_id,
            "song_id": song_id,
            "analysis_type": analysis_type,
            "status": status,
            "progress": progress,
            "analysis_json_path": analysis_blob_path,
            "compare_json_path": compare_blob_path,
            "score": score,
            "created_at": created_at or datetime.utcnow().isoformat(),
            "completed_at": completed_at,
            "duration_sec": duration_sec,
            "error_message": error_message,
        }
        doc_ref.set(data_to_store)

        song_score_ref = firestore_db.collection(f"{uid}_score").document(song_id)
        subcollections = song_score_ref.collections()

        existing_n = []
        for subcol in subcollections:
            try:
                existing_n.append(int(subcol.id))
            except ValueError:
                continue

        next_n = max(existing_n, default=0) + 1
        n_folder = str(next_n)

        final_score_ref = song_score_ref.collection(n_folder).document("score")
        final_score_ref.set({
            "date": datetime.utcnow().isoformat(),
            "pitch": pitch_score,
            "technique": technique_score,
            "onset": rhythm_score,
            "accuracy": score
        })

        return {"message": "분석 결과 저장 성공!!!!!!", "task_id": task_id}

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

    finally:
        if os.path.exists(analysis_path):
            os.remove(analysis_path)
        if os.path.exists(compare_path):
            os.remove(compare_path)
    
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