import os
from fastapi import HTTPException
import requests
from firebase_admin import firestore, storage
from manager.firebase_manager import db

class RecentPrepareService:
    """최근 준비된 음악 관련 서비스 클래스"""
    
    @staticmethod
    def get_storage_url(file_path: str):
        """Firebase Storage에서 파일 URL을 생성합니다"""
        try:
            bucket = storage.bucket()
            blob = bucket.blob(file_path)  # file_path에 해당하는 파일 객체(blob)를 가져옴
            return blob.generate_signed_url(expiration=3600)  # 1시간 동안 해당 mp3 파일을 다운로드 가능하게 함
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"Firebase Storage URL 생성 실패: {str(e)}")

    @classmethod
    def get_recent_uploads(cls):
        """최근 업로드된 음악 목록을 가져옵니다"""
        try:
            music_ref = db.collection("preparemymusic")
            docs = music_ref.order_by("upload_time", direction=firestore.Query.DESCENDING).limit(10).stream()
            music_list = []
            for doc in docs:
                data = doc.to_dict()
                music_list.append({
                    "title": data.get("title", "제목 없음"),
                    "artist": data.get("artist", "알 수 없음"),
                    "album_cover": data.get("album_cover", ""),
                    "file_url": cls.get_storage_url(data.get("file_path", ""))
                })
            return {"recent_music": music_list}
        except requests.RequestException as e:
            raise HTTPException(status_code=500, detail=f"신곡 데이터 요청 실패: {str(e)}")
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"최근 업로드 데이터 조회 실패: {str(e)}")