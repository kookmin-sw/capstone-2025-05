import pytest
from fastapi.testclient import TestClient
from main import app

client = TestClient(app)

def test_get_songs():
    """곡 목록 조회 API 테스트"""
    response = client.get("/api/v1/songs")
    assert response.status_code == 200
    assert isinstance(response.json(), list)
    assert len(response.json()) > 0

def test_get_song():
    """개별 곡 조회 API 테스트"""
    # 먼저 모든 곡 목록을 가져옴
    response = client.get("/api/v1/songs")
    
    # 응답 상태 코드 확인
    if response.status_code != 200:
        pytest.fail(f"곡 목록 조회 실패: {response.status_code}, {response.text}")
    
    songs = response.json()
    if not songs:
        pytest.fail("곡 목록이 비어 있습니다.")
    
    # 첫 번째 곡의 ID를 사용하여 개별 곡 조회
    first_song_id = songs[0]["song_id"]
    response = client.get(f"/api/v1/songs/{first_song_id}")
    
    assert response.status_code == 200
    assert response.json()["song_id"] == first_song_id

def test_get_not_found_song():
    """존재하지 않는 곡 조회 시 404 응답 테스트"""
    response = client.get("/api/v1/songs/not-existing-id")
    assert response.status_code == 404 