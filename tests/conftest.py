import pytest
from unittest.mock import patch, MagicMock
import os
import sys

# 테스트에서 PYTHONPATH 설정
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# 테스트 환경에서 파이어베이스 관련 모듈 로딩 전에 환경 변수를 설정합니다
os.environ["TESTING"] = "1"

@pytest.fixture(scope="session")
def setup_test_env():
    """테스트 환경 설정"""
    # 테스트 실행 전에 필요한 환경 설정 (이미 위에서 설정했습니다)
    yield
    
    # 테스트 실행 후 정리 작업
    # 테스트가 실행된 후에도 TESTING 환경 변수는 유지하여 다른 테스트에 영향을 주지 않도록 합니다
    
@pytest.fixture
def mock_firebase_config():
    """테스트용 Firebase 설정"""
    return {
        "type": "service_account",
        "project_id": "test-project",
        "private_key_id": "key_id",
        "private_key": "-----BEGIN PRIVATE KEY-----\nTEST\n-----END PRIVATE KEY-----",
        "client_email": "test@test-project.iam.gserviceaccount.com",
        "client_id": "123456789",
        "auth_uri": "https://accounts.google.com/o/oauth2/auth",
        "token_uri": "https://oauth2.googleapis.com/token",
        "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
        "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/test",
    }

# pytest.ini 파일을 생성하는 것이 더 좋지만, 지금은 이렇게 설정합니다.
def pytest_configure(config):
    """pytest 전역 설정"""
    # asyncio 관련 경고 메시지 대응
    config._inicache["asyncio_mode"] = "strict"
    config._inicache["asyncio_default_fixture_loop_scope"] = "function"