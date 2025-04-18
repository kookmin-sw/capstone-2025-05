import os
import pytest
from fastapi.testclient import TestClient
from unittest.mock import patch, MagicMock

from app.main import app

client = TestClient(app)


def test_health_endpoint():
    """Test that the health endpoint returns a healthy status."""
    response = client.get("/health")
    assert response.status_code == 200
    assert response.json() == {"status": "healthy"}


def test_root_endpoint():
    """Test that the root endpoint returns API information."""
    response = client.get("/")
    assert response.status_code == 200
    data = response.json()
    assert "name" in data
    assert "version" in data
    assert "status" in data
    assert data["status"] == "online"


@pytest.fixture
def mock_audio_file():
    """Create a mock audio file for testing."""
    # This is a minimal WAV file header (44 bytes) + some silent audio data
    minimal_wav_bytes = (
        b'RIFF\x24\x00\x00\x00WAVEfmt \x10\x00\x00\x00\x01\x00\x01\x00'
        b'\x44\xAC\x00\x00\x88\x58\x01\x00\x02\x00\x10\x00data\x00\x00\x00\x00'
    )
    return minimal_wav_bytes


@patch('workers.tasks.analyze_audio.delay')
def test_analyze_endpoint(mock_analyze_task, mock_audio_file):
    """Test that the analyze endpoint accepts uploads and returns a task ID."""
    # Setup mock task
    mock_task = MagicMock()
    mock_task.id = "test-task-id"
    mock_analyze_task.return_value = mock_task
    
    # Call API
    response = client.post(
        "/api/v1/analyze",
        files={"file": ("test.wav", mock_audio_file, "audio/wav")},
        params={"user_id": "test-user", "song_id": "test-song"}
    )
    
    # Check response
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    assert data["task_id"] == "test-task-id"
    
    # Verify task was called correctly
    mock_analyze_task.assert_called_once()


@patch('workers.tasks.compare_audio.delay')
def test_compare_endpoint(mock_compare_task, mock_audio_file):
    """Test that the compare endpoint accepts uploads and returns a task ID."""
    # Setup mock task
    mock_task = MagicMock()
    mock_task.id = "test-compare-task-id"
    mock_compare_task.return_value = mock_task
    
    # Call API
    response = client.post(
        "/api/v1/compare",
        files={
            "user_file": ("user.wav", mock_audio_file, "audio/wav"),
            "reference_file": ("reference.wav", mock_audio_file, "audio/wav")
        },
        params={"user_id": "test-user", "song_id": "test-song"}
    )
    
    # Check response
    assert response.status_code == 200
    data = response.json()
    assert "task_id" in data
    assert data["task_id"] == "test-compare-task-id"
    
    # Verify task was called correctly
    mock_compare_task.assert_called_once()