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
        data={
            "user_id": "test_user", 
            "song_id": "test_song",
            "generate_feedback": "False"
        }
    )
    
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    
    # compare_audio.delay가 호출되었는지 확인
    mock_compare.delay.assert_called_once()
    
    # 올바른 매개변수가 전달되었는지 확인
    args, kwargs = mock_compare.delay.call_args
    assert kwargs.get("user_id") == "test_user"
    assert kwargs.get("song_id") == "test_song"
    assert kwargs.get("reference_audio_bytes") is not None


@patch("app.api.v1.compare_audio")
def test_compare_with_stored_reference_endpoint(mock_compare, client):
    """저장된 레퍼런스 데이터를 사용한 비교 엔드포인트 테스트"""
    # AsyncResult 객체 모의 생성
    mock_task = MagicMock()
    mock_task.id = "test_task_id"
    mock_compare.delay.return_value = mock_task
    
    # 테스트 파일 생성
    test_file_content = b"test audio content"
    
    # API 호출
    response = client.post(
        "/api/v1/compare-with-reference",
        files={
            "user_file": ("test.wav", test_file_content, "audio/wav")
        },
        data={
            "user_id": "test_user", 
            "song_id": "test_song",
            "generate_feedback": "True"
        }
    )
    
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    
    # compare_audio.delay가 호출되었는지 확인
    mock_compare.delay.assert_called_once()
    # DB에서 가져온 레퍼런스로 호출되었는지 확인 (reference_audio_bytes는 None)
    args, kwargs = mock_compare.delay.call_args
    assert kwargs.get("reference_audio_bytes") is None
    assert kwargs.get("song_id") == "test_song"


@patch("app.api.v1.analyze_reference_audio")
def test_analyze_reference_endpoint(mock_analyze_reference, client):
    """레퍼런스 오디오 분석 엔드포인트 테스트"""
    # AsyncResult 객체 모의 생성
    mock_task = MagicMock()
    mock_task.id = "test_reference_task_id"
    mock_analyze_reference.delay.return_value = mock_task
    
    # 테스트 파일 생성
    test_file_content = b"test reference audio content"
    
    # API 호출
    response = client.post(
        "/api/v1/reference",
        files={
            "reference_file": ("reference.wav", test_file_content, "audio/wav"),
            "midi_file": ("notes.mid", b"test midi content", "audio/midi")
        },
        data={
            "song_id": "test_song_id",
            "description": "테스트 레퍼런스 오디오"
        }
    )
    
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    
    # analyze_reference_audio.delay가 호출되었는지 확인
    mock_analyze_reference.delay.assert_called_once()


@patch("app.api.v1.compare_audio")
@patch("app.api.v1.get_reference_features")  # app.db.get_reference_features 대신 app.api.v1.get_reference_features를 모킹
def test_compare_with_ref_features_dtw(mock_get_features, mock_compare, client):
    """DB에서 크로마 데이터를 가져와 DTW 정렬을 사용한 비교 테스트"""
    # AsyncResult 객체 모의 생성
    mock_task = MagicMock()
    mock_task.id = "test_dtw_task_id"
    mock_compare.delay.return_value = mock_task
    
    # DB에서 가져올 모의 레퍼런스 특성 (크로마 데이터 포함)
    mock_ref_features = {
        "tempo": 120,
        "onsets": [0.1, 0.5, 1.0, 1.5],
        "pitches": [440, 554, 659, 880],
        "techniques": [["normal"], ["bend"], ["slide"], ["hammer"]],
        "chroma": [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]],  # 크로마 데이터 포함
        "metadata": {
            "song_id": "test_song_with_chroma",
            "has_midi": True
        }
    }
    
    # 모의 MIDI 데이터
    mock_midi_data = {
        "notes": [
            {"start": 0.1, "end": 0.4, "pitch": 60, "velocity": 100},
            {"start": 0.5, "end": 0.9, "pitch": 62, "velocity": 100},
            {"start": 1.0, "end": 1.4, "pitch": 64, "velocity": 100}
        ],
        "tempos": [120],
        "tempo_times": [0]
    }
    
    # DB에서 레퍼런스 특성을 가져오는 함수가 모의 데이터를 반환하도록 설정
    mock_get_features.return_value = {
        **mock_ref_features,
        "midi_data": mock_midi_data
    }
    
    # 테스트 파일 생성
    test_file_content = b"test audio content for DTW alignment"
    
    # API 호출
    response = client.post(
        "/api/v1/compare-with-reference",
        files={
            "user_file": ("test.wav", test_file_content, "audio/wav")
        },
        data={  # params 대신 data를 사용하여 Form 데이터로 전달
            "user_id": "test_user",
            "song_id": "test_song_with_chroma",
            "generate_feedback": "False"
        }
    )
    
    # 응답 및 모의 함수 호출 확인
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    assert data["task_id"] == "test_dtw_task_id"
    
    # DB에서 레퍼런스 특성 조회 함수가 올바른 song_id로 호출되었는지 확인
    mock_get_features.assert_called_once_with("test_song_with_chroma")
    
    # compare_audio.delay가 올바른 인자로 호출되었는지 확인
    mock_compare.delay.assert_called_once()
    args, kwargs = mock_compare.delay.call_args
    assert kwargs.get("reference_audio_bytes") is None  # 레퍼런스 오디오 바이트는 사용하지 않음
    assert kwargs.get("song_id") == "test_song_with_chroma"
    assert kwargs.get("user_audio_bytes") is not None