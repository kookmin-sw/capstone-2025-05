import os
import firebase_admin
from firebase_admin import credentials, firestore, db, storage
from dotenv import load_dotenv


class FirebaseManager:
    """Firebase 연결 및 관리를 위한 클래스"""
    _instance = None
    _initialized = False
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(FirebaseManager, cls).__new__(cls)
        return cls._instance
    
    def __init__(self):
        if not FirebaseManager._initialized:
            self._load_env_vars()
            # 테스트 모드가 아닐 때만 실제 Firebase 초기화
            if os.environ.get("TESTING") != "1":
                self._initialize_firebase()
            else:
                # 테스트 모드일 때는 Mock 객체만 생성
                self._initialize_mock_firebase()
            FirebaseManager._initialized = True
    
    def _load_env_vars(self):
        """환경 변수 로드"""
        dotenv_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".env"))
        load_dotenv(dotenv_path)
        self.firebase_config = {
            "type": os.getenv("TYPE"),
            "project_id": os.getenv("PROJECT_ID"),
            "private_key_id": os.getenv("PRIVATE_KEY_ID"),
            "private_key": os.getenv("PRIVATE_KEY", "").replace('\\n', '\n'),
            "client_email": os.getenv("CLIENT_EMAIL"),
            "client_id": os.getenv("CLIENT_ID"),
            "auth_uri": os.getenv("AUTH_URI"),
            "token_uri": os.getenv("TOKEN_URI"),
            "auth_provider_x509_cert_url": os.getenv("AUTH_PROVIDER_CERT_URL"),
            "client_x509_cert_url": os.getenv("CLIENT_CERT_URL")
        }
    
    def _initialize_firebase(self):
        """Firebase 초기화"""
        if not firebase_admin._apps:
            cred = credentials.Certificate(self.firebase_config)
            firebase_admin.initialize_app(cred, {
                "databaseURL": os.getenv("DATABASE_URL"),
                "storageBucket": os.getenv("STORAGE_BUCKET")
            })
        self.firestore_db = firestore.client()
        self.realtime_db = db.reference("/")
        self.storage_bucket = storage.bucket()
        
    def _initialize_mock_firebase(self):
        """테스트용 Mock Firebase 초기화"""
        # 테스트용 Mock 객체 생성
        class MockDB:
            def collection(self, *args, **kwargs):
                return self
                
            def document(self, *args, **kwargs):
                return self
                
            def set(self, *args, **kwargs):
                return None
                
            def get(self, *args, **kwargs):
                class MockSnapshot:
                    def __init__(self):
                        self.exists = True
                return MockSnapshot()
                
            def stream(self, *args, **kwargs):
                return []
                
            def child(self, *args, **kwargs):
                return self
                
            def delete(self, *args, **kwargs):
                return None
        
        class MockBucket:
            def blob(self, *args, **kwargs):
                return MockBlob()
                
            def list_blobs(self, *args, **kwargs):
                return []
        
        class MockBlob:
            def upload_from_filename(self, *args, **kwargs):
                return None
                
            def delete(self, *args, **kwargs):
                return None
        
        self.firestore_db = MockDB()
        self.realtime_db = MockDB()
        self.storage_bucket = MockBucket()
    
    @property
    def firestore(self):
        """Firestore 데이터베이스 클라이언트 반환"""
        return self.firestore_db
    
    @property
    def rtdb(self):
        """실시간 데이터베이스 참조 반환"""
        return self.realtime_db
    
    @property
    def storage(self):
        """Storage 버킷 반환"""
        return self.storage_bucket


# 싱글톤 인스턴스 생성
firebase_manager = FirebaseManager()

# 외부에서 쉽게 접근할 수 있도록 변수 제공
firestore_db = firebase_manager.firestore
realtime_db = firebase_manager.rtdb
storage_bucket = firebase_manager.storage