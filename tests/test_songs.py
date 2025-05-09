import pytest
from fastapi.testclient import TestClient
from main import app
from .conftest import client

def test_get_songs(client):
    """곡 목록 조회 API 테스트"""
    response = client.get("/api/v1/songs")
    assert response.status_code == 200
    
    data = response.json()
    assert isinstance(data, list)
    assert len(data) > 0
    
    # 기본 필드 확인
    first_song = data[0]
    assert "song_id" in first_song
    assert "title" in first_song
    assert "artist" in first_song

def test_get_song(client):
    """개별 곡 조회 API 테스트"""
    # 첫 번째 테스트 곡 ID 사용
    song_id = "test-song-1"
    response = client.get(f"/api/v1/songs/{song_id}")
    
    assert response.status_code == 200
    
    song = response.json()
    assert song["song_id"] == song_id
    assert song["title"] == "테스트 곡 1"
    assert song["artist"] == "테스트 아티스트"

def test_get_not_found_song(client):
    """존재하지 않는 곡 조회 시 404 응답 테스트"""
    response = client.get("/api/v1/songs/not-existing-id")
    assert response.status_code == 404 