import pytest
from unittest.mock import AsyncMock, patch
from fastapi import FastAPI
from fastapi.testclient import TestClient
from io import BytesIO

from routers.myPage_router import router

# 독립적인 FastAPI 앱 생성하여 테스트
app = FastAPI()
app.include_router(router, prefix="/mypage")  # mypage prefix 추가
test_client = TestClient(app)

# MyPageService 모의 객체 생성
class MyPageServiceMock:
    @staticmethod
    async def edit_nickname(uid, nickname):
        pass
    
    @staticmethod
    async def edit_interest_genre(uid, interest_genre):
        pass
    
    @staticmethod
    async def edit_level(uid, level):
        pass
    
    @staticmethod
    async def change_profile_image(uid, file):
        pass
    
    @staticmethod
    async def get_all_records(uid):
        pass
    
    @staticmethod
    async def get_specific_record(uid, song_name, upload_count):
        pass
    
    @staticmethod
    async def get_record_audio(uid, song_name, upload_count):
        pass
    
    @staticmethod
    async def get_my_posts(uid):
        pass
    
    @staticmethod
    async def get_my_scraps(uid):
        pass
    
    @staticmethod
    async def get_my_likes(uid):
        pass

# 라우터 함수들을 패치할 경로
ROUTER_PATH = "routers.myPage_router"

class TestMyPageRouter:
    """마이페이지 라우터 테스트"""
    
    @pytest.mark.asyncio
    async def test_edit_nickname(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.edit_nickname", new_callable=AsyncMock) as mock_func:
            mock_func.return_value = {"message": "닉네임 변경 완료"}
            
            # 테스트
            response = test_client.put("/mypage/edit-user/nickname?uid=test_uid&nickname=new_name")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == {"message": "닉네임 변경 완료"}
    
    @pytest.mark.asyncio
    async def test_edit_interest_genre(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.edit_interest_genre", new_callable=AsyncMock) as mock_func:
            mock_func.return_value = {"message": "관심 장르 변경 완료"}
            
            # 테스트
            response = test_client.put("/mypage/edit-user/interest-genre?uid=test_uid", json=[1, 2, 3])
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == {"message": "관심 장르 변경 완료"}
    
    @pytest.mark.asyncio
    async def test_edit_level(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.edit_level", new_callable=AsyncMock) as mock_func:
            mock_func.return_value = {"message": "실력 변경 완료"}
            
            # 테스트
            response = test_client.put("/mypage/edit-user/level?uid=test_uid&level=3")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == {"message": "실력 변경 완료"}
    
    @pytest.mark.asyncio
    async def test_change_profile_image(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.change_profile_image", new_callable=AsyncMock) as mock_func:
            mock_func.return_value = {
                "message": "프로필 사진 변경 완료",
                "profile_image_url": "https://example.com/profile.jpg"
            }
            
            # 테스트
            test_file = BytesIO(b"test image content")
            response = test_client.post(
                "/mypage/change-profile-image?uid=test_uid",
                files={"file": ("profile.jpg", test_file, "image/jpeg")}
            )
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json()["message"] == "프로필 사진 변경 완료"
            assert response.json()["profile_image_url"] == "https://example.com/profile.jpg"
    
    @pytest.mark.asyncio
    async def test_get_all_records(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.get_all_records", new_callable=AsyncMock) as mock_func:
            mock_data = {
                "records": {
                    "song1": [
                        {
                            "upload_count": 1,
                            "tempo": 120,
                            "beat": 0.85,
                            "interval": 0.75,
                            "date": "2025-04-05",
                            "audio_url": "https://example.com/song1_1.mp3"
                        }
                    ]
                }
            }
            mock_func.return_value = mock_data
            
            # 테스트
            response = test_client.get("/mypage/records/all?uid=test_uid")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == mock_data
    
    @pytest.mark.asyncio
    async def test_get_specific_record(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.get_specific_record", new_callable=AsyncMock) as mock_func:
            mock_data = {
                "upload_count": 1,
                "tempo": 120,
                "beat": 0.85,
                "interval": 0.75,
                "date": "2025-04-05"
            }
            mock_func.return_value = mock_data
            
            # 테스트
            response = test_client.get("/mypage/records/specific?uid=test_uid&song_name=song1&upload_count=1")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == mock_data
    
    @pytest.mark.asyncio
    async def test_get_record_audio(self):
        # 모의 객체 설정
        from fastapi.responses import StreamingResponse
        
        async def mock_streaming_response():
            def iter_bytes():
                yield b"fake audio data"
            
            return StreamingResponse(iter_bytes(), media_type="audio/mpeg")
        
        with patch(f"{ROUTER_PATH}.get_record_audio", new_callable=AsyncMock) as mock_func:
            mock_func.return_value = await mock_streaming_response()
            
            # 테스트
            response = test_client.get("/mypage/records/audio?uid=test_uid&song_name=song1&upload_count=1")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.headers["content-type"] == "audio/mpeg"
    
    @pytest.mark.asyncio
    async def test_get_my_posts(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.get_my_posts", new_callable=AsyncMock) as mock_func:
            mock_posts = {
                "my_posts": [
                    {"post_id": "post1", "title": "첫 번째 게시글"},
                    {"post_id": "post2", "title": "두 번째 게시글"}
                ]
            }
            mock_func.return_value = mock_posts
            
            # 테스트
            response = test_client.get("/mypage/my-posts?uid=test_uid")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == mock_posts
    
    @pytest.mark.asyncio
    async def test_get_my_scraps(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.get_my_scraps", new_callable=AsyncMock) as mock_func:
            mock_scraps = {
                "my_scraps": [
                    {"post_id": "post1", "title": "스크랩한 게시글 1"},
                    {"post_id": "post2", "title": "스크랩한 게시글 2"}
                ]
            }
            mock_func.return_value = mock_scraps
            
            # 테스트
            response = test_client.get("/mypage/my-scraps?uid=test_uid")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == mock_scraps
    
    @pytest.mark.asyncio
    async def test_get_my_likes(self):
        # 모의 객체 설정
        with patch(f"{ROUTER_PATH}.get_my_likes", new_callable=AsyncMock) as mock_func:
            mock_likes = {
                "my_likes": [
                    {"post_id": "post1", "title": "좋아요한 게시글 1"},
                    {"post_id": "post2", "title": "좋아요한 게시글 2"}
                ]
            }
            mock_func.return_value = mock_likes
            
            # 테스트
            response = test_client.get("/mypage/my-likes?uid=test_uid")
            
            # 검증
            assert mock_func.called
            assert response.status_code == 200
            assert response.json() == mock_likes