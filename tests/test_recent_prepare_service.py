import pytest
from unittest.mock import patch, MagicMock
from fastapi import HTTPException
from fastapi.testclient import TestClient

from manager.recent_prepare_service import RecentPrepareService
from firebase_admin import firestore

class TestRecentPrepareService:
    """최근 준비된 음악 서비스 테스트"""
    
    @patch('manager.recent_prepare_service.storage.bucket')
    def test_get_storage_url_success(self, mock_bucket):
        """Storage URL 생성 성공 테스트"""
        # 모킹 설정
        mock_blob = MagicMock()
        mock_blob.generate_signed_url.return_value = "https://example.com/test-file.mp3"
        mock_bucket_instance = MagicMock()
        mock_bucket_instance.blob.return_value = mock_blob
        mock_bucket.return_value = mock_bucket_instance
        
        # 메서드 호출 및 검증
        result = RecentPrepareService.get_storage_url("test/path/file.mp3")
        
        assert result == "https://example.com/test-file.mp3"
        mock_bucket_instance.blob.assert_called_once_with("test/path/file.mp3")
        mock_blob.generate_signed_url.assert_called_once_with(expiration=3600)
    
    @patch('manager.recent_prepare_service.storage.bucket')
    def test_get_storage_url_error(self, mock_bucket):
        """Storage URL 생성 실패 테스트"""
        # 모킹 설정 - 예외 발생
        mock_bucket.side_effect = Exception("Storage 접근 오류")
        
        # 예외 발생 검증
        with pytest.raises(HTTPException) as exc_info:
            RecentPrepareService.get_storage_url("test/path/file.mp3")
        
        assert exc_info.value.status_code == 500
        assert "Firebase Storage URL 생성 실패" in exc_info.value.detail
    
    @patch('manager.recent_prepare_service.db')
    @patch('manager.recent_prepare_service.RecentPrepareService.get_storage_url')
    def test_get_recent_uploads_success(self, mock_get_storage_url, mock_db):
        """최근 업로드 목록 조회 성공 테스트"""
        # 모킹 설정
        mock_get_storage_url.return_value = "https://example.com/test-file.mp3"
        
        # Firestore 응답 모킹
        mock_docs = [
            MagicMock(to_dict=lambda: {
                "title": "테스트 곡 1",
                "artist": "테스트 아티스트 1",
                "album_cover": "https://example.com/album1.jpg",
                "file_path": "music/test1.mp3"
            }),
            MagicMock(to_dict=lambda: {
                "title": "테스트 곡 2", 
                "artist": "테스트 아티스트 2",
                "album_cover": "https://example.com/album2.jpg",
                "file_path": "music/test2.mp3"
            })
        ]
        
        # Firestore 쿼리 체인 모킹
        mock_limit = MagicMock()
        mock_limit.stream.return_value = mock_docs
        
        mock_order_by = MagicMock()
        mock_order_by.limit.return_value = mock_limit
        
        mock_collection = MagicMock()
        mock_collection.order_by.return_value = mock_order_by
        
        mock_db.collection.return_value = mock_collection
        
        # 메서드 호출
        result = RecentPrepareService.get_recent_uploads()
        
        # 검증
        assert "recent_music" in result
        assert len(result["recent_music"]) == 2
        assert result["recent_music"][0]["title"] == "테스트 곡 1"
        assert result["recent_music"][1]["title"] == "테스트 곡 2"
        mock_db.collection.assert_called_once_with("preparemymusic")
        mock_collection.order_by.assert_called_once_with("upload_time", direction=firestore.Query.DESCENDING)
        mock_order_by.limit.assert_called_once_with(10)
        assert mock_get_storage_url.call_count == 2
    
    @patch('manager.recent_prepare_service.db')
    def test_get_recent_uploads_error(self, mock_db):
        """최근 업로드 목록 조회 실패 테스트"""
        # 모킹 설정 - 예외 발생
        mock_db.collection.side_effect = Exception("Firestore 접근 오류")
        
        # 예외 발생 검증
        with pytest.raises(HTTPException) as exc_info:
            RecentPrepareService.get_recent_uploads()
        
        assert exc_info.value.status_code == 500
        assert "최근 업로드 데이터 조회 실패" in exc_info.value.detail