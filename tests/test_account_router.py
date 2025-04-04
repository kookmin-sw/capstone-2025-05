import pytest
from unittest.mock import patch, MagicMock
from fastapi import FastAPI
from fastapi.testclient import TestClient

# 직접 라우터 함수와 클래스를 테스트
from routers.account_router import (
    router as account_router,
    google_login,
    google_auth_callback,
    email_sign_up,
    set_user_info,
    email_login,
    delete_user
)
from models.response_models import StandardResponse
from manager.account_service import UserEmailSignUp, UserData


@pytest.fixture
def client():
    """테스트용 FastAPI 앱을 생성하고 라우터를 등록합니다."""
    app = FastAPI()
    app.include_router(account_router)
    return TestClient(app)


class TestAccountRouter:
    @pytest.mark.asyncio
    async def test_google_login_function(self):
        """google_login 함수가 올바른 리디렉션 응답을 반환하는지 테스트"""
        with patch("routers.account_router.AccountService.get_google_auth_url", 
                  return_value="https://google.com/auth"):
            # 비동기 함수 호출
            response = await google_login()
            
            # RedirectResponse가 올바르게 생성되는지 확인
            assert response.status_code == 307  # FastAPI의 RedirectResponse 기본값
            assert response.headers["location"] == "https://google.com/auth"

    @pytest.mark.asyncio
    async def test_google_auth_callback_function(self):
        """google_auth_callback 함수를 테스트"""
        with patch("routers.account_router.AccountService.process_google_auth", 
                  return_value={"uid": "test_uid_123"}):
            # 함수 직접 호출
            result = await google_auth_callback("test_code")
            
            # 응답 확인
            assert isinstance(result, StandardResponse)
            assert result.success is True
            assert result.message == "로그인 성공"
            assert result.data == {"uid": "test_uid_123"}

    # API 엔드포인트 통합 테스트
    def test_google_auth_callback_endpoint(self, client):
        """Google 인증 콜백 엔드포인트를 테스트"""
        with patch("routers.account_router.AccountService.process_google_auth", 
                  return_value={"uid": "test_uid_123"}):
            response = client.get("/google-auth-callback?code=test_code")
            
            assert response.status_code == 200
            assert response.json() == {
                "success": True,
                "message": "로그인 성공",
                "data": {"uid": "test_uid_123"}
            }

    def test_email_sign_up_endpoint(self, client):
        """이메일 회원가입 엔드포인트를 테스트"""
        test_data = {"email": "test@example.com", "password": "password123"}
        
        with patch("routers.account_router.AccountService.create_email_user", 
                  return_value={"uid": "test_uid_123"}):
            response = client.post("/email-sign-up", json=test_data)
            
            assert response.status_code == 200
            assert response.json() == {
                "success": True,
                "message": "회원가입 성공",
                "data": {"uid": "test_uid_123"}
            }

    def test_set_user_info_endpoint(self, client):
        """사용자 정보 설정 엔드포인트를 테스트"""
        test_data = {
            "uid": "test_uid_123",
            "nickname": "테스트유저",
            "interest_genre": [1, 3, 5],
            "level": 2
        }
        
        with patch("routers.account_router.AccountService.set_user_information", 
                  return_value={"uid": "test_uid_123"}):
            response = client.post("/set-user-info", json=test_data)
            
            assert response.status_code == 200
            assert response.json() == {
                "success": True,
                "message": "유저 정보 입력 완료",
                "data": {"uid": "test_uid_123"}
            }

    def test_email_login_endpoint(self, client):
        """이메일 로그인 엔드포인트를 테스트"""
        test_data = {"email": "test@example.com", "password": "password123"}
        
        with patch("routers.account_router.AccountService.login_with_email", 
                  return_value={"uid": "test_uid_123"}):
            response = client.post("/email-login", json=test_data)
            
            assert response.status_code == 200
            assert response.json() == {
                "success": True,
                "message": "로그인 성공",
                "data": {"uid": "test_uid_123"}
            }

    def test_delete_user_endpoint(self, client):
        """사용자 삭제 엔드포인트를 테스트"""
        with patch("routers.account_router.AccountService.delete_user_account", 
                  return_value={"uid": "test_uid_123"}):
            response = client.delete("/delete-user/test_uid_123")
            
            assert response.status_code == 200
            assert response.json() == {
                "success": True,
                "message": "사용자 삭제 완료",
                "data": {"uid": "test_uid_123"}
            }