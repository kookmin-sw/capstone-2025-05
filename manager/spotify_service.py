import requests
import os
from dotenv import load_dotenv
import base64
from urllib.parse import urlencode
from fastapi import HTTPException

# 환경 변수 로드
load_dotenv()

CLIENT_ID = os.getenv("NEXT_PUBLIC_SPOTIFY_CLIENT_ID")
CLIENT_SECRET = os.getenv("NEXT_PUBLIC_SPOTIFY_CLIENT_SECRET")

class SpotifyService:
    """Spotify API와 상호작용하는 서비스 클래스"""
    
    @staticmethod
    def get_spotify_token():
        """Spotify API 접근을 위한 액세스 토큰을 얻음"""
        auth_string = f"{CLIENT_ID}:{CLIENT_SECRET}"
        auth_bytes = auth_string.encode("utf-8")
        auth_base64 = base64.b64encode(auth_bytes).decode("utf-8")
        
        url = "https://accounts.spotify.com/api/token"
        headers = {
            "Authorization": f"Basic {auth_base64}",
            "Content-Type": "application/x-www-form-urlencoded"
        }
        data = {"grant_type": "client_credentials"}
        
        try:
            response = requests.post(url, headers=headers, data=data, timeout=10)
            response.raise_for_status()
            json_result = response.json()
            return json_result.get("access_token")
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"Spotify 토큰을 가져오는데 실패했습니다: {str(e)}")

    @classmethod
    def get_top_tracks(cls):
        """인기 트랙 가져오기"""
        try:
            token = cls.get_spotify_token()
            
            url = "https://api.spotify.com/v1/playlists/37i9dQZEVXbMDoHDwVN2tF"  # Global Top 50 Playlist
            headers = {
                "Authorization": f"Bearer {token}"
            }
            
            response = requests.get(url, headers=headers, timeout=10)
            response.raise_for_status()
            
            return response.json()
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"인기 트랙을 가져오는데 실패했습니다: {str(e)}")

    @classmethod
    def get_new_releases(cls):
        """신규 발매곡 가져오기"""
        try:
            token = cls.get_spotify_token()
            
            url = "https://api.spotify.com/v1/browse/new-releases"
            headers = {
                "Authorization": f"Bearer {token}"
            }
            
            response = requests.get(url, headers=headers, timeout = 10)
            response.raise_for_status()
            
            return response.json()
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"신규 발매곡을 가져오는데 실패했습니다: {str(e)}")

    @classmethod
    def search_tracks(cls, query: str):
        """트랙 검색"""
        try:
            token = cls.get_spotify_token()
            
            url = f"https://api.spotify.com/v1/search?q={query}&type=track&limit=10"
            headers = {
                "Authorization": f"Bearer {token}"
            }
            
            response = requests.get(url, headers=headers, timeout=10)
            response.raise_for_status()
            
            return response.json()
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"트랙을 검색하는데 실패했습니다: {str(e)}")

    @classmethod
    def get_recommendations(cls, seed_tracks: str = None, seed_artists: str = None, 
                           seed_genres: str = None, limit: int = 10):
        """
        트랙, 아티스트 또는 장르를 기반으로 추천 트랙을 가져옵니다.
        최소한 하나의 seed 파라미터가 필요합니다.
        """
        try:
            if not any([seed_tracks, seed_artists, seed_genres]):
                raise HTTPException(
                    status_code=400, 
                    detail="최소한 하나의 seed 파라미터(seed_tracks, seed_artists, seed_genres)가 필요합니다."
                )
            
            token = cls.get_spotify_token()
            
            # 쿼리 파라미터 구성
            query_params = {}
            if seed_tracks:
                query_params["seed_tracks"] = seed_tracks
            if seed_artists:
                query_params["seed_artists"] = seed_artists
            if seed_genres:
                query_params["seed_genres"] = seed_genres
            if limit:
                query_params["limit"] = limit
                
            url = f"https://api.spotify.com/v1/recommendations?{urlencode(query_params)}"
            headers = {
                "Authorization": f"Bearer {token}"
            }
            
            response = requests.get(url, headers=headers, timeout=10)
            response.raise_for_status()
            
            return response.json()
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"추천 트랙을 가져오는데 실패했습니다: {str(e)}")