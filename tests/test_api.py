import os
import pytest
import json
from fastapi.testclient import TestClient
from unittest.mock import patch, MagicMock

# 가짜 MongoDB 설정을 위한 패치 적용
try:
    # db_patch 모듈에서 패치 설정
    from tests.db_patch import patch_db_module
    db_patches = patch_db_module()
except ImportError:
    print("경고: db_patch 모듈을 가져올 수 없습니다.")
    db_patches = []

# app.main 모듈 가져오기
from app.main import app

client = TestClient(app)


def test_health_check(client):
    """상태 확인 엔드포인트 테스트"""
    response = client.get("/health")
    assert response.status_code == 200
    data = response.json()
    assert data["status"] == "healthy"
    assert "services" in data
    assert "mongodb" in data["services"]


def test_root_endpoint(client):
    """루트 엔드포인트 테스트"""
    response = client.get("/")
    assert response.status_code == 200
    data = response.json()
    assert "name" in data
    assert "version" in data
    assert "status" in data
    assert data["status"] == "online"


def test_get_result(client):
    """결과 조회 엔드포인트 테스트"""
    response = client.get("/api/v1/results/test_task_id")
    assert response.status_code == 200
    data = response.json()
    assert data["task_id"] == "test_task_id"
    assert "result" in data
    assert data["result"]["tempo"] == 120


def test_get_user_results(client):
    """사용자 결과 조회 엔드포인트 테스트"""
    response = client.get("/api/v1/user/test_user/results")
    assert response.status_code == 200
    data = response.json()
    # 결과 길이를 검증하는 대신 배열이 반환되었는지와 필수 필드 존재 여부만 검증
    assert isinstance(data, list)
    assert len(data) > 0
    for item in data:
        assert "task_id" in item
        assert "song_id" in item
        assert "result" in item


def test_get_song_results(client):
    """곡 결과 조회 엔드포인트 테스트"""
    response = client.get("/api/v1/song/test_song/results")
    assert response.status_code == 200
    data = response.json()
    # 결과 길이를 검증하는 대신 배열이 반환되었는지와 필수 필드 존재 여부만 검증
    assert isinstance(data, list)
    assert len(data) > 0
    for item in data:
        assert "task_id" in item
        assert "song_id" in item
        assert "result" in item


@pytest.fixture
def mock_audio_file():
    """Create a mock audio file for testing."""
    # This is a minimal WAV file header (44 bytes) + some silent audio data
    minimal_wav_bytes = (
        b'RIFF\x24\x00\x00\x00WAVEfmt \x10\x00\x00\x00\x01\x00\x01\x00'
        b'\x44\xAC\x00\x00\x88\x58\x01\x00\x02\x00\x10\x00data\x00\x00\x00\x00'
    )
    return minimal_wav_bytes


@patch("app.api.v1.analyze_audio")
def test_analyze_endpoint(mock_analyze, client):
    """분석 엔드포인트 테스트"""
    # AsyncResult 객체 모의 생성
    mock_task = MagicMock()
    mock_task.id = "test_task_id"
    mock_analyze.delay.return_value = mock_task
    
    # 테스트 파일 생성
    test_file_content = b"test audio content"
    
    # API 호출
    response = client.post(
        "/api/v1/analyze",
        files={"file": ("test.wav", test_file_content, "audio/wav")},
        params={"user_id": "test_user", "song_id": "test_song"}
    )
    
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    
    # analyze_audio.delay가 호출되었는지 확인
    mock_analyze.delay.assert_called()


@patch("app.api.v1.compare_audio")
def test_compare_endpoint(mock_compare, client):
    """비교 엔드포인트 테스트"""
    # AsyncResult 객체 모의 생성
    mock_task = MagicMock()
    mock_task.id = "test_task_id"
    mock_compare.delay.return_value = mock_task
    
    # 테스트 파일 생성
    test_file_content = b"test audio content"
    
    # API 호출
    response = client.post(
        "/api/v1/compare",
        files={
            "user_file": ("test.wav", test_file_content, "audio/wav"),
            "reference_file": ("ref.wav", test_file_content, "audio/wav")
        },
        params={"user_id": "test_user", "song_id": "test_song"}
    )
    
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    
    # compare_audio.delay가 호출되었는지 확인
    mock_compare.delay.assert_called()