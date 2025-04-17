from fastapi import APIRouter, HTTPException
from manager.ranking_service import RankingService

router = APIRouter()

@router.get("/rank", tags=["Rank"])
async def get_rank(song_name: str):
    """특정 곡의 랭킹 정보를 가져옵니다."""
    return await RankingService.get_rank(song_name)