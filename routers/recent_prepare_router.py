from fastapi import FastAPI, HTTPException, APIRouter
from fastapi.responses import StreamingResponse
import requests
import os
from dotenv import load_dotenv
from io import BytesIO
import urllib.parse
import firebase_admin
from firebase_admin import firestore, storage
from firebase_admin import credentials

from manager.firebase_manager import firestore_db, storage_bucket


recent_prepare_router = APIRouter(prefix="/api/prepare")

def get_storage_url(file_path : str):
    try:
        blob = storage_bucket.blob(file_path)
        return blob.generate_signed_url(expiration=3600)
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Firebase Storage URL 생성 실패: {str(e)}")

@recent_prepare_router.get("/recent_music", tags=["Recent Prepare Music"])
def get_recent_music_from_storage():
    try:
        blobs = storage_bucket.list_blobs(prefix="recent_prepare_music/")
        music_files = []

        for blob in blobs:
            if not blob.name.endswith("/"):
                file_name = blob.name.split("/")[-1]
                music_files.append(file_name)

        return music_files

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"음악 리스트 가져오기 실패: {str(e)}")

@recent_prepare_router.get("/download_music/{file_name}", tags=["Recent Prepare Music"])
async def download_music(file_name: str):
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
