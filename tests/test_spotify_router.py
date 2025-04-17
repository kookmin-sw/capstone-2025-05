import pytest
from unittest.mock import patch, MagicMock
from fastapi import HTTPException, FastAPI
from fastapi.testclient import TestClient
import json
import os

from routers.spotify_router import router, get_spotify_token
from app import app
from manager.spotify_service import SpotifyService

client = TestClient(app)

class TestSpotifyRouter:
    """Spotify API 라우터 테스트"""
    
    @pytest.fixture
    def mock_token_response(self):
        return {
            "access_token": "test_access_token",
            "token_type": "Bearer",
            "expires_in": 3600
        }
    
    @pytest.fixture
    def mock_top_tracks_response(self):
        return {
            "playlists": {
                "href": "https://api.spotify.com/v1/browse/categories/toplists/playlists",
                "items": [
                    {
                        "id": "playlist1",
                        "name": "Top 50 글로벌",
                        "description": "인기 곡 모음"
                    },
                    {
                        "id": "playlist2",
                        "name": "Top 50 한국",
                        "description": "한국 인기 곡 모음"
                    }
                ]
            }
        }
    
    @pytest.fixture
    def mock_new_releases_response(self):
        return {
            "albums": {
                "href": "https://api.spotify.com/v1/browse/new-releases",
                "items": [
                    {
                        "id": "album1",
                        "name": "새 앨범 1",
                        "artists": [{"name": "아티스트 1"}]
                    },
                    {
                        "id": "album2",
                        "name": "새 앨범 2",
                        "artists": [{"name": "아티스트 2"}]
                    }
                ]
            }
        }
    
    @pytest.fixture
    def mock_search_response(self):
        return {
            "tracks": {
                "href": "https://api.spotify.com/v1/search?q=test&type=track&limit=10",
                "items": [
                    {
                        "id": "track1",
                        "name": "테스트 곡 1",
                        "artists": [{"name": "아티스트 1"}]
                    },
                    {
                        "id": "track2",
                        "name": "테스트 곡 2",
                        "artists": [{"name": "아티스트 2"}]
                    }
                ]
            }
        }
    
    @pytest.fixture
    def mock_recommendations_response(self):
        return {
            "tracks": [
                {
                    "id": "rec1",
                    "name": "추천 곡 1",
                    "artists": [{"name": "아티스트 1"}],
                    "album": {"name": "앨범 1"},
                    "preview_url": "https://example.com/preview1"
                },
                {
                    "id": "rec2",
                    "name": "추천 곡 2",
                    "artists": [{"name": "아티스트 2"}],
                    "album": {"name": "앨범 2"},
                    "preview_url": "https://example.com/preview2"
                }
            ]
        }
    
    @patch('manager.spotify_service.requests.post')
    def test_get_spotify_token_success(self, mock_post):
        # 성공 응답 모킹
        mock_response = MagicMock()
        mock_response.json.return_value = {"access_token": "test_token"}
        mock_response.raise_for_status.return_value = None
        mock_post.return_value = mock_response
        
        # 직접 함수 호출을 통한 테스트
        token = SpotifyService.get_spotify_token()
        
        assert token == "test_token"
        assert mock_post.called

    @patch('manager.spotify_service.requests.post')
    def test_get_spotify_token_error(self, mock_post):
        # 실패 응답 모킹
        mock_post.side_effect = Exception("API 호출 오류")
        
        # 예외 발생 테스트
        with pytest.raises(Exception):
            SpotifyService.get_spotify_token()

    @patch("manager.spotify_service.SpotifyService.get_spotify_token", return_value="test_token")
    @patch("manager.spotify_service.requests.get")
    def test_get_top_tracks_success(self, mock_get, mock_token, mock_top_tracks_response):
        """인기 트랙 가져오기 성공 테스트"""
        mock_response = MagicMock()
        mock_response.json.return_value = mock_top_tracks_response
        mock_response.raise_for_status.return_value = None
        mock_get.return_value = mock_response
        
        response = client.get("/api/spotify/top-tracks")
        
        assert response.status_code == 200
        assert response.json() == mock_top_tracks_response
        mock_get.assert_called_once_with(
            "https://api.spotify.com/v1/playlists/37i9dQZEVXbMDoHDwVN2tF",
            headers={"Authorization": "Bearer test_token"},
            timeout=10
        )
    
    @patch('manager.spotify_service.SpotifyService.get_spotify_token')
    @patch('manager.spotify_service.requests.get')
    def test_get_top_tracks_error(self, mock_get, mock_token):
        # 토큰은 성공적으로 얻지만 API 호출 실패 모킹
        mock_token.return_value = "test_token"
        mock_get.side_effect = Exception("API 호출 오류")
        
        # API 엔드포인트 호출
        response = client.get("/api/spotify/top-tracks")
        
        # 검증
        assert response.status_code == 500
        assert "인기 트랙을 가져오는데 실패했습니다" in response.json()["details"]["detail"]

    @patch("manager.spotify_service.SpotifyService.get_spotify_token", return_value="test_token")
    @patch("manager.spotify_service.requests.get")
    def test_get_new_releases_success(self, mock_get, mock_token, mock_new_releases_response):
        """신규 발매곡 가져오기 성공 테스트"""
        mock_response = MagicMock()
        mock_response.json.return_value = mock_new_releases_response
        mock_response.raise_for_status.return_value = None
        mock_get.return_value = mock_response
        
        response = client.get("/api/spotify/new-releases")
        
        assert response.status_code == 200
        assert response.json() == mock_new_releases_response
        mock_get.assert_called_once_with(
            "https://api.spotify.com/v1/browse/new-releases",
            headers={"Authorization": "Bearer test_token"},
            timeout=10
        )
    
    @patch('manager.spotify_service.SpotifyService.get_spotify_token')
    @patch('manager.spotify_service.requests.get')
    def test_get_new_releases_error(self, mock_get, mock_token):
        # 토큰은 성공적으로 얻지만 API 호출 실패 모킹
        mock_token.return_value = "test_token"
        mock_get.side_effect = Exception("API 호출 오류")
        
        # API 엔드포인트 호출
        response = client.get("/api/spotify/new-releases")
        
        # 검증
        assert response.status_code == 500
        assert "신규 발매곡을 가져오는데 실패했습니다" in response.json()["details"]["detail"]

    @patch("manager.spotify_service.SpotifyService.get_spotify_token", return_value="test_token")
    @patch("manager.spotify_service.requests.get")
    def test_search_tracks_success(self, mock_get, mock_token, mock_search_response):
        """트랙 검색 성공 테스트"""
        mock_response = MagicMock()
        mock_response.json.return_value = mock_search_response
        mock_response.raise_for_status.return_value = None
        mock_get.return_value = mock_response
        
        response = client.get("/api/spotify/search?query=test")
        
        assert response.status_code == 200
        assert response.json() == mock_search_response
        mock_get.assert_called_once_with(
            "https://api.spotify.com/v1/search?q=test&type=track&limit=10",
            headers={"Authorization": "Bearer test_token"},
            timeout=10
        )
    
    @patch('manager.spotify_service.SpotifyService.get_spotify_token')
    @patch('manager.spotify_service.requests.get')
    def test_search_tracks_error(self, mock_get, mock_token):
        # 토큰은 성공적으로 얻지만 API 호출 실패 모킹
        mock_token.return_value = "test_token"
        mock_get.side_effect = Exception("API 호출 오류")
        
        # API 엔드포인트 호출
        response = client.get("/api/spotify/search?query=test")
        
        # 검증
        assert response.status_code == 500
        assert "트랙을 검색하는데 실패했습니다" in response.json()["details"]["detail"]

    @patch("manager.spotify_service.SpotifyService.get_spotify_token", return_value="test_token")
    @patch("manager.spotify_service.requests.get")
    def test_get_recommendations_success(self, mock_get, mock_token, mock_recommendations_response):
        """추천 트랙 가져오기 성공 테스트"""
        mock_response = MagicMock()
        mock_response.json.return_value = mock_recommendations_response
        mock_response.raise_for_status.return_value = None
        mock_get.return_value = mock_response
        
        response = client.get("/api/spotify/recommendations?seed_artists=artist123&limit=5")
        
        assert response.status_code == 200
        assert response.json() == mock_recommendations_response
        mock_get.assert_called_once_with(
            "https://api.spotify.com/v1/recommendations?seed_artists=artist123&limit=5",
            headers={"Authorization": "Bearer test_token"},
            timeout=10
        )
        
    def test_get_recommendations_missing_seeds(self):
        """추천 트랙 가져오기 실패 테스트 - 필수 seed 파라미터 누락"""
        response = client.get("/api/spotify/recommendations")
        
        assert response.status_code == 400
        assert "최소한 하나의 seed 파라미터" in response.json()["details"]["detail"]