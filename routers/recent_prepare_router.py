from fastapi import FastAPI, HTTPException, APIRouter
import requests
import os
from dotenv import load_dotenv

import firebase_admin
from firebase_admin import firestore, storage
from firebase_admin import credentials

from manager.firebase_manager import db

load_dotenv()
music_ref = db.collection("preparemymusic")
recent_prepare_router = APIRouter(prefix="/api/prepare", tags=["RecentPrepare"])

def get_storage_url(file_path : str):
    try:
        bucket = storage.bucket()
        blob = bucket.blob(file_path) #file_path에 해당하는 파일 객체(blob을 가져옴)
        return blob.generate_signed_url(expiration=3600) #1시간 동안 해당 mp3 파일을 다운로드 가능하게 함
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Firebase Storage URL 생성 실패: {str(e)}")

#최신곡 가져오는 코드
@spotify_router.get("/recent_music")
def get_recent_uploads():
    try:
        docs = music_ref.order_by("upload_time", direction=firestore.Query.DESCENDING).limit(10).stream()
        music_list = []
        for doc in docs:
            data = doc.to_dict()
            music_list.append({
                "title": data.get("title", "제목 없음"),
                "artist": data.get("artist", "알 수 없음"),
                "album_cover": data.get("album_cover", ""),
                "file_url" : get_storage_url(data.get("file_path", ""))
            }) 
        return {"recent_music": music_list}
    except requests.RequestException as e:
        raise HTTPException(status_code=500, detail=f"Spotify 신곡 데이터 요청 실패: {str(e)}")


#서버 실행 명령어 : uvicorn spotify_manager:app --reload
