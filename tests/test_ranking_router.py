import pytest
from unittest.mock import AsyncMock, patch
from fastapi import FastAPI, HTTPException
from fastapi.testclient import TestClient

from routers.ranking_router import router
from manager.ranking_service import RankingService

# 독립적인 FastAPI 앱 생성하여 테스트
app = FastAPI()
app.include_router(router, prefix="/ranking")
test_client = TestClient(app)

class TestRankingRouter:
    """랭킹 라우터 테스트 클래스"""
    
    @pytest.mark.asyncio
    @patch.object(RankingService, "get_rank", new_callable=AsyncMock)
    async def test_get_rank_success(self, mock_service):
        """랭킹 정보 가져오기 성공 테스트"""
        # 모킹 설정
        mock_data = [
            {"uid": "user4", "nickname": "사용자4", "score": 99, "rank": 1, "profile_url": "https://example.com/profile4.jpg"},
            {"uid": "user2", "nickname": "사용자2", "score": 98, "rank": 2, "profile_url": "https://example.com/profile2.jpg"},
            {"uid": "user1", "nickname": "사용자1", "score": 95, "rank": 3, "profile_url": "https://example.com/profile1.jpg"},
            {"uid": "user3", "nickname": "사용자3", "score": 90, "rank": 4}
        ]
        mock_service.return_value = mock_data
        
        # 테스트 실행
        response = test_client.get("/ranking/rank?song_name=테스트곡")
        
        # 검증
        mock_service.assert_awaited_once_with("테스트곡")
        assert response.status_code == 200
        assert response.json() == mock_data
    
    @pytest.mark.asyncio
    @patch.object(RankingService, "get_rank", new_callable=AsyncMock)
    async def test_get_rank_not_found(self, mock_service):
        """랭킹 정보 없을 때 테스트"""
        # 모킹 설정 - 예외 발생
        mock_service.side_effect = HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")
        
        # 테스트 실행
        response = test_client.get("/ranking/rank?song_name=존재하지않는곡")
        
        # 검증
        mock_service.assert_awaited_once_with("존재하지않는곡")
        assert response.status_code == 404
        assert "해당 곡의 랭킹 정보가 없습니다" in response.json()["detail"]