import pytest
from unittest.mock import patch, MagicMock
from fastapi import HTTPException

from manager.ranking_service import RankingService

class TestRankingService:
    """랭킹 서비스 테스트 클래스"""

    @pytest.mark.asyncio
    @patch('manager.ranking_service.db.reference')
    @patch('manager.ranking_service.storage.bucket')
    async def test_get_rank_success(self, mock_bucket, mock_reference):
        """랭킹 정보 가져오기 성공 테스트"""
        # 모킹 설정
        mock_ref = MagicMock()
        mock_reference.return_value = mock_ref
        mock_ref.get.return_value = {
            "user1": {"nickname": "사용자1", "score": 95},
            "user2": {"nickname": "사용자2", "score": 98},
            "user3": {"nickname": "사용자3", "score": 90},
            "user4": {"nickname": "사용자4", "score": 99}
        }
        
        # 프로필 이미지 URL 모킹
        mock_blob = MagicMock()
        mock_blob.generate_signed_url.return_value = "https://example.com/profile.jpg"
        mock_bucket_instance = MagicMock()
        mock_bucket_instance.blob.return_value = mock_blob
        mock_bucket.return_value = mock_bucket_instance
        
        # 테스트 실행
        result = await RankingService.get_rank("테스트곡")
        
        # 결과 검증
        assert len(result) == 4
        
        # 점수 내림차순 검증
        assert result[0]["score"] == 99
        assert result[1]["score"] == 98
        assert result[2]["score"] == 95
        assert result[3]["score"] == 90
        
        # 상위 3명 프로필 URL 검증
        assert "profile_url" in result[0]
        assert "profile_url" in result[1]
        assert "profile_url" in result[2]
        
        # 순위 정보 검증
        assert result[0]["rank"] == 1
        assert result[3]["rank"] == 4
        
        # 호출 검증
        mock_reference.assert_called_once_with("/rank/테스트곡")
    
    @pytest.mark.asyncio
    @patch('manager.ranking_service.db.reference')
    async def test_get_rank_not_found(self, mock_reference):
        """랭킹 정보 없을 때 테스트"""
        # 모킹 설정
        mock_ref = MagicMock()
        mock_reference.return_value = mock_ref
        mock_ref.get.return_value = None
        
        # 예외 발생 검증
        with pytest.raises(HTTPException) as excinfo:
            await RankingService.get_rank("존재하지않는곡")
        
        assert excinfo.value.status_code == 404
        assert "해당 곡의 랭킹 정보가 없습니다" in excinfo.value.detail
        
        # 호출 검증
        mock_reference.assert_called_once_with("/rank/존재하지않는곡")
    
    @pytest.mark.asyncio
    @patch('manager.ranking_service.db.reference')
    @patch('manager.ranking_service.storage.bucket')
    async def test_get_rank_with_profile_error(self, mock_bucket, mock_reference):
        """프로필 이미지 URL 생성 실패 테스트"""
        # 모킹 설정
        mock_ref = MagicMock()
        mock_reference.return_value = mock_ref
        mock_ref.get.return_value = {
            "user1": {"nickname": "사용자1", "score": 95}
        }
        
        # 프로필 이미지 URL 생성 실패 모킹
        mock_bucket_instance = MagicMock()
        mock_bucket_instance.blob.side_effect = Exception("Storage 접근 오류")
        mock_bucket.return_value = mock_bucket_instance
        
        # 테스트 실행
        result = await RankingService.get_rank("테스트곡")
        
        # 결과 검증 - 프로필 이미지 에러가 있어도 랭킹 정보는 정상 반환
        assert len(result) == 1
        assert result[0]["profile_url"] is None