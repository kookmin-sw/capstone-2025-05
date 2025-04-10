from fastapi import FastAPI, HTTPException, Query, APIRouter
from firebase_admin import firestore
from manager.firebase_manager import firestore_db
from dotenv import load_dotenv
from manager.firebase_manager import storage_bucket
from io import BytesIO
import urllib.parse
load_dotenv()

mainsearch_router = APIRouter(prefix="/api", tags=["MainSearch"])
music_ref = firestore_db.collection("recent_preparse_music") #최근준비음원 db
# 🔽 여기에 직접 추가
def get_storage_url(file_path: str):
    try:
        blob = storage_bucket.blob(file_path)
        return blob.generate_signed_url(expiration=3600)
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Firebase Storage URL 생성 실패: {str(e)}")

@mainsearch_router.get("/search", tags=["MainSearch"])
def search_music(query: str = Query(..., description="검색어 입력")):
    try:
        blobs = storage_bucket.list_blobs(prefix="recent_prepare_music/")
        results = []

        for blob in blobs:
            if not blob.name.endswith("/"):
                file_name = blob.name.split("/")[-1]
                if query.lower() in file_name.lower():
                    results.append({
                        "title": file_name
                    })

        return {"results": results}
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"검색 실패: {str(e)}")
@mainsearch_router.get("/autocomplete", tags=["MainSearch"])
def autocomplete(query: str = Query(..., description="자동완성 검색어 입력")):
    try:
        blobs = storage_bucket.list_blobs(prefix="recent_prepare_music/")
        suggestions = set()
        
        for blob in blobs:
            if not blob.name.endswith("/"):
                file_name = blob.name.split("/")[-1]
                if file_name.lower().startswith(query.lower()):
                    suggestions.add(file_name)

        return {"suggestions": list(suggestions)}
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"자동완성 실패: {str(e)}")