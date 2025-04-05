import pytest
from unittest.mock import patch, MagicMock
from fastapi import HTTPException
from fastapi.testclient import TestClient

from app import app
from routers.recent_prepare_router import router

client = TestClient(app)

class TestRecentPrepareRouter:
    """최근 업로드 준비된 음악 라우터 테스트"""
    
    @pytest.fixture
    def mock_recent_music_response(self):
        """최근 업로드된 음악 목록 모킹"""
        return {
            "recent_music": [
                {
                    "title": "테스트 곡 1",
                    "artist": "테스트 아티스트 1",
                    "album_cover": "http://example.com/album1.jpg",
                    "file_url": "http://example.com/file1.mp3"
                },
                {
                    "title": "테스트 곡 2",
                    "artist": "테스트 아티스트 2",
                    "album_cover": "http://example.com/album2.jpg",
                    "file_url": "http://example.com/file2.mp3"
                }
            ]
        }
    
    @patch("manager.recent_prepare_service.RecentPrepareService.get_recent_uploads")
    def test_get_recent_uploads_success(self, mock_get_recent_uploads, mock_recent_music_response):
        """최근 음악 목록 가져오기 성공 테스트"""
        # 모킹 설정
        mock_get_recent_uploads.return_value = mock_recent_music_response
        
        # API 엔드포인트 호출
        response = client.get("/api/prepare/recent_music")
        
        # 검증
        assert response.status_code == 200
        assert response.json() == mock_recent_music_response
        mock_get_recent_uploads.assert_called_once()
    
    @patch("manager.recent_prepare_service.RecentPrepareService.get_recent_uploads")
    def test_get_recent_uploads_error(self, mock_get_recent_uploads):
        """최근 음악 목록 가져오기 실패 테스트"""
        # 모킹 설정 - 예외 발생
        mock_get_recent_uploads.side_effect = HTTPException(status_code=500, detail="최근 업로드 데이터 조회 실패")
        
        # API 엔드포인트 호출
        response = client.get("/api/prepare/recent_music")
        
        # 검증
        assert response.status_code == 500
        assert "최근 업로드 데이터 조회 실패" in response.json()["details"]["detail"]