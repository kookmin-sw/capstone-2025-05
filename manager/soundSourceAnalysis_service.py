from typing import Dict, Any
from fastapi import HTTPException, UploadFile
from firebase_admin import firestore
from manager.firebase_manager import firestore_db, storage_bucket
from datetime import timedelta

class AnalysisService:
    @staticmethod
    async def upload_image(uid: str, song_name: str, index: int, file: UploadFile) -> Dict[str, str]:
        try:
            blob_path = f"{uid}/analysis/{song_name}/{index}/{file.filename}"
            blob = storage_bucket.blob(blob_path)
            blob.upload_from_file(file.file, content_type=file.content_type)
            return {"message": "이미지 업로드 완료", "path": blob_path}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"이미지 업로드 실패: {str(e)}")

    @staticmethod
    async def upload_score(uid: str, song_name: str, index: int, score: dict) -> Dict[str, str]:
        try:
            path = f"{uid}_score/{song_name}/{index}"
            firestore_db.document(f"{path}/detail_score").set(score)
            return {"message": "점수 업로드 완료", "path": path}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"점수 업로드 실패: {str(e)}")

    @staticmethod
    async def get_analysis_image(uid: str, song_name: str, index: int) -> Dict[str, str]:
        try:
            path_prefix = f"{uid}/analysis/{song_name}/{index}/"
            blobs = list(storage_bucket.list_blobs(prefix=path_prefix))
            if not blobs:
                raise HTTPException(status_code=404, detail="이미지를 찾을 수 없습니다.")
            image_blob = blobs[0]
            image_url = image_blob.generate_signed_url(version="v4", expiration=timedelta(minutes=10), method="GET")
            return {"image_url": image_url}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"이미지 조회 실패: {str(e)}")

    @staticmethod
    async def get_score(uid: str, song_name: str, index: int) -> Dict[str, Any]:
        try:
            doc_ref = firestore_db.document(f"{uid}_score/{song_name}/{index}/detail_score")
            doc = doc_ref.get()
            if not doc.exists:
                raise HTTPException(status_code=404, detail="점수를 찾을 수 없습니다.")
            return doc.to_dict()
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"점수 조회 실패: {str(e)}")