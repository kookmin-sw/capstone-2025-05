from fastapi import FastAPI, HTTPException, APIRouter
import requests
import os
from dotenv import load_dotenv

load_dotenv()

router = APIRouter()

def get_spotify_token():
    auth_url = "https://accounts.spotify.com/api/token"
    client_id = os.getenv("REACT_APP_SPOTIFY_CLIENT_ID")
    client_secret = os.getenv("REACT_APP_SPOTIFY_CLIENT_SECRET")

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

@router.get("/new-releases", tags=["Spotify"])
def get_new_releases():
    token = get_spotify_token()
    url = "https://api.spotify.com/v1/browse/new-releases"
    headers = {"Authorization": f"Bearer {token}"}

    try:
        response = requests.get(url, headers=headers, timeout=10)
        response.raise_for_status()
        data = response.json()

        albums = data.get("albums", {}).get("items", [])
        result = []

        for album in albums:
            album_id = album.get("id")
            album_title = album.get("name")
            album_cover = album.get("images", [{}])[0].get("url", "")
            artists = ", ".join([artist["name"] for artist in album.get("artists", [])])
            
            tracks_url = f"https://api.spotify.com/v1/albums/{album_id}/tracks"
            track_response = requests.get(tracks_url, headers=headers, timeout=10)
            track_response.raise_for_status()
            tracks_data = track_response.json().get("items", [])

            if tracks_data:
                track = tracks_data[0]
                track_name = track.get("name", "")
                preview_url = track.get("preview_url")  
                spotify_url = track.get("external_urls", {}).get("spotify", "")

                result.append({
                    "album_title": album_title,
                    "track_title": track_name,
                    "artist": artists,
                    "album_cover": album_cover,
                    "preview_url": preview_url,      
                    "spotify_url": spotify_url         
                })

        return {"new_releases": result}

    except requests.RequestException as e:
        raise HTTPException(status_code=500, detail=f"Spotify 신곡 데이터 요청 실패: {str(e)}")
