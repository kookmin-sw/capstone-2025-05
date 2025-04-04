from manager.firebase_manager import firestore_db, storage_bucket
import os
import aiofiles
from fastapi import HTTPException

class TestService:
    @staticmethod
    async def create_test_document(name: str, guitar_skill: str):
        """Firestore에 테스트 문서를 생성합니다"""
        try:
            doc_ref = firestore_db.collection("test").document()
            doc_ref.set({
                "이름": name,
                "기타 실력": guitar_skill
            })
            return {"success": True, "document_id": doc_ref.id}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"Firestore 문서 생성 실패: {str(e)}")

    @staticmethod
    async def upload_sample_file(file_path: str, destination_path: str):
        """Storage에 파일을 업로드합니다"""
        try:
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"파일을 찾을 수 없습니다: {file_path}")
                
            blob = storage_bucket.blob(destination_path)
            # 비동기적으로 파일 처리하기
            blob.upload_from_filename(file_path)
            return {"success": True, "path": destination_path}
        except FileNotFoundError as e:
            raise HTTPException(status_code=404, detail=str(e))
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"파일 업로드 실패: {str(e)}")