from fastapi import APIRouter, HTTPException
from manager.spotify_service import SpotifyService

router = APIRouter(prefix="/api/spotify", tags=["Spotify"])

# SpotifyService 클래스의 get_spotify_token 메서드를 직접 참조
get_spotify_token = SpotifyService.get_spotify_token

@router.get("/top-tracks")
async def get_top_tracks():
    """인기 트랙 가져오기"""
    return SpotifyService.get_top_tracks()

@router.get("/new-releases")
async def get_new_releases():
    """신규 발매곡 가져오기"""
    return SpotifyService.get_new_releases()

@router.get("/search")
async def search_tracks(query: str):
    """트랙 검색"""
    return SpotifyService.search_tracks(query)

@router.get("/recommendations")
async def get_recommendations(
    seed_tracks: str = None,
    seed_artists: str = None, 
    seed_genres: str = None,
    limit: int = 10
):
    """
    트랙, 아티스트 또는 장르를 기반으로 추천 트랙을 가져옵니다.
    최소한 하나의 seed 파라미터가 필요합니다.
    """
    return SpotifyService.get_recommendations(
        seed_tracks=seed_tracks,
        seed_artists=seed_artists,
        seed_genres=seed_genres,
        limit=limit
    )
