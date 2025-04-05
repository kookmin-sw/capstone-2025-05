from fastapi import FastAPI, HTTPException, APIRouter
import requests
from dotenv import load_dotenv
from manager.recent_prepare_service import RecentPrepareService

load_dotenv()
router = APIRouter()

@router.get("/recent_music")
def get_recent_uploads():
    """최근 업로드된 음악 목록을 가져옵니다"""
    return RecentPrepareService.get_recent_uploads()
