import urllib.parse
from io import BytesIO
from fastapi import HTTPException
from fastapi.responses import StreamingResponse
from manager.firebase_manager import storage_bucket

class RecentPrepareService:
    """최근 준비된 음악 관련 서비스 클래스"""

    @staticmethod
    def get_storage_url(file_path: str):
        """파일에 대한 Firebase Storage 다운로드 URL을 생성"""
        try:
            blob = storage_bucket.blob(file_path)
            return blob.generate_signed_url(expiration=3600)
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"Firebase Storage URL 생성 실패: {str(e)}")

    @staticmethod
    def get_recent_music_list():
        """Firebase Storage에서 최근 음악 파일 리스트를 가져옵니다."""
        try:
            blobs = storage_bucket.list_blobs(prefix="recent_prepare_music/")
            music_files = []

            for blob in blobs:
                if not blob.name.endswith("/"):  # 폴더가 아닌 파일일 때
                    file_name = blob.name.split("/")[-1]
                    music_files.append(file_name)

            return {"music_files": music_files}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"음악 리스트 가져오기 실패: {str(e)}")

    @staticmethod
    def download_music_file(file_name: str):
        """Firebase Storage에서 음악 파일을 다운로드합니다."""
        try:
            file_path = f"recent_prepare_music/{file_name}"
            blob = storage_bucket.blob(file_path)

            if not blob.exists():
                raise HTTPException(status_code=404, detail="해당 파일이 존재하지 않습니다.")

            file_bytes = blob.download_as_bytes()
            audio_stream = BytesIO(file_bytes)
            encoded_file_name = urllib.parse.quote(file_name)

            headers = {
                "Content-Disposition": f"attachment; filename*=UTF-8''{encoded_file_name}"
            }

            return StreamingResponse(audio_stream, headers=headers, media_type="application/octet-stream")

        except Exception as e:
            raise HTTPException(status_code=500, detail=f"음악 다운로드 실패: {str(e)}")