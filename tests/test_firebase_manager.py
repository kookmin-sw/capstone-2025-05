import pytest
from unittest.mock import patch, MagicMock, mock_open
import os
import firebase_admin

from manager.firebase_manager import FirebaseManager


class TestFirebaseManager:
    @pytest.fixture
    def mock_load_dotenv(self):
        with patch('manager.firebase_manager.load_dotenv') as mock_load:
            yield mock_load

    @pytest.fixture
    def mock_os_env(self):
        mock_env = {
            "TYPE": "service_account",
            "PROJECT_ID": "test-project",
            "PRIVATE_KEY_ID": "key_id",
            "PRIVATE_KEY": "-----BEGIN PRIVATE KEY-----\\nTEST\\n-----END PRIVATE KEY-----",
            "CLIENT_EMAIL": "test@test-project.iam.gserviceaccount.com",
            "CLIENT_ID": "123456789",
            "AUTH_URI": "https://accounts.google.com/o/oauth2/auth",
            "TOKEN_URI": "https://oauth2.googleapis.com/token",
            "AUTH_PROVIDER_CERT_URL": "https://www.googleapis.com/oauth2/v1/certs",
            "CLIENT_CERT_URL": "https://www.googleapis.com/robot/v1/metadata/x509/test",
            "DATABASE_URL": "https://test-project.firebaseio.com",
            "STORAGE_BUCKET": "test-project.appspot.com"
        }
        with patch.dict(os.environ, mock_env):
            yield mock_env

    @pytest.fixture
    def mock_firebase_admin(self):
        with patch('manager.firebase_manager.firebase_admin') as mock_firebase:
            mock_firebase._apps = []
            yield mock_firebase

    @pytest.fixture
    def mock_credentials(self):
        with patch('manager.firebase_manager.credentials') as mock_creds:
            yield mock_creds

    @pytest.fixture
    def mock_firestore(self):
        with patch('manager.firebase_manager.firestore') as mock_fs:
            yield mock_fs

    @pytest.fixture
    def mock_db(self):
        with patch('manager.firebase_manager.db') as mock_db:
            yield mock_db

    @pytest.fixture
    def mock_storage(self):
        with patch('manager.firebase_manager.storage') as mock_storage:
            yield mock_storage
            
    def test_mock_firebase_initialization(self):
        """테스트 모드에서 Mock Firebase 초기화 테스트"""
        # 테스트 모드에서는 이미 TESTING=1 환경 변수가 설정되어 있음
        
        # Reset the singleton instance for testing
        FirebaseManager._instance = None
        FirebaseManager._initialized = False
        
        # Create an instance in test mode
        firebase_manager = FirebaseManager()
        
        # 실제 Mock 객체가 생성되었는지 확인
        assert hasattr(firebase_manager, 'firestore_db')
        assert hasattr(firebase_manager, 'realtime_db')
        assert hasattr(firebase_manager, 'storage_bucket')
        
        # Mock 객체의 메소드 확인
        assert firebase_manager.firestore_db.collection() is not None
        assert firebase_manager.realtime_db.child('test') is not None
        assert firebase_manager.storage_bucket.blob('test') is not None
    
    def test_property_getters(self):
        """프로퍼티 게터가 제대로 작동하는지 테스트"""
        # Reset the singleton instance for testing
        FirebaseManager._instance = None
        FirebaseManager._initialized = False
        
        # Create an instance
        firebase_manager = FirebaseManager()
        
        # Call the property getters
        firestore_db = firebase_manager.firestore
        rtdb = firebase_manager.rtdb
        storage_bucket = firebase_manager.storage
        
        # Verify that the getters return the expected values
        assert firestore_db == firebase_manager.firestore_db
        assert rtdb == firebase_manager.realtime_db
        assert storage_bucket == firebase_manager.storage_bucket