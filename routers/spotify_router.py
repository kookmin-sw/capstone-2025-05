from fastapi import HTTPException, APIRouter
import requests
import os
from dotenv import load_dotenv

load_dotenv()

router = APIRouter()

spotify_router = APIRouter(prefix="/api/spotify", tags=["Spotify"])

def get_spotify_token():
    auth_url = "https://accounts.spotify.com/api/token"
    client_id = os.getenv("NEXT_PUBLIC_SPOTIFY_CLIENT_ID")
    client_secret = os.getenv("NEXT_PUBLIC_SPOTIFY_CLIENT_SECRET")

    if not client_id or not client_secret:
        raise HTTPException(status_code=500, detail="Spotify API 인증 정보가 누락되었습니다.")

    try:
        response = requests.post(
        auth_url,
        headers = {"Content-Type" : "application/x-www-form-urlencoded"},
        data = {
            "grant_type": "client_credentials",
            "client_id": client_id,
            "client_secret" : client_secret,
        },
        timeout = 10
        )
        response.raise_for_status()
        print("Spotify Token Response:", response.json())
        return response.json().get('access_token')
    except requests.RequestException as e:
        raise HTTPException(status_code=500, detail="Spotify API 토큰 요청 실패 : {str(e)}")
@spotify_router.get("/top-tracks")
def get_top_tracks():
    token = get_spotify_token()
    url = "https://api.spotify.com/v1/browse/categories/toplists/playlists"
    headers = {"Authorization": f"Bearer {token}"}
    try:
        response = requests.get(url, headers=headers, timeout=10)
        print(f"Spotify API Response: {response.status_code}, {response.text}")
        response.raise_for_status()
        return response.json()
    except requests.RequestException as e:
        raise HTTPException(status_code=500, detail=f"Spotify 인기곡 데이터 요청 실패: {str(e)}")

#최신곡 가져오는 코드
@spotify_router.get("/new-releases")
def get_new_releases():
    token = get_spotify_token()
    url = "https://api.spotify.com/v1/browse/new-releases"

    headers = {"Authorization": f"Bearer {token}"}
    
    try:
        response = requests.get(url, headers=headers, timeout = 10)
        print(f"Spotify API Response: {response.status_code}, {response.text}")
        response.raise_for_status()
        return response.json()
    except requests.RequestException as e:
        raise HTTPException(status_code=500, detail=f"Spotify 신곡 데이터 요청 실패: {str(e)}")