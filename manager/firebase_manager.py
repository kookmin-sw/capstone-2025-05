import os
import firebase_admin
from firebase_admin import credentials, firestore, db, storage
import logging

# 로깅 설정
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

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
            # 환경 변수 로그 출력 (디버깅용)
            logger.info(f"DEVELOPMENT env: {os.environ.get('DEVELOPMENT')}")
            logger.info(f"TESTING env: {os.environ.get('TESTING')}")
            
            # 테스트 모드이거나 개발 모드일 때는 Mock 객체 사용
            if os.environ.get("TESTING") == "1" or os.environ.get("DEVELOPMENT") == "1":
                logger.info("Running in test/development mode. Using mock Firebase.")
                self._initialize_mock_firebase()
            else:
                # 환경 변수 로드 시도
                try:
                    self._load_env_vars()
                    self._initialize_firebase()
                    logger.info("Firebase initialized successfully")
                except Exception as e:
                    logger.warning(f"Firebase initialization failed: {e}. Falling back to mock implementation.")
                    self._initialize_mock_firebase()
            
            FirebaseManager._initialized = True
    
    def _load_env_vars(self):
        """시스템 환경 변수 로드"""
        # 필수 환경 변수 검증
        required_env_vars = ["PROJECT_ID", "PRIVATE_KEY", "CLIENT_EMAIL"]
        missing_vars = [var for var in required_env_vars if not os.environ.get(var)]
        if missing_vars:
            raise ValueError(f"Missing required environment variables: {', '.join(missing_vars)}")
        
        # 환경 변수 로그 출력 (디버깅용)
        logger.info(f"Found PROJECT_ID: {bool(os.environ.get('PROJECT_ID'))}")
        logger.info(f"Found CLIENT_EMAIL: {bool(os.environ.get('CLIENT_EMAIL'))}")
        logger.info(f"Found PRIVATE_KEY: {bool(os.environ.get('PRIVATE_KEY'))}")
        
        self.firebase_config = {
            "type": os.environ.get("TYPE", "service_account"),
            "project_id": os.environ.get("PROJECT_ID"),
            "private_key_id": os.environ.get("PRIVATE_KEY_ID", ""),
            "private_key": os.environ.get("PRIVATE_KEY", "").replace('\\n', '\n'),
            "client_email": os.environ.get("CLIENT_EMAIL"),
            "client_id": os.environ.get("CLIENT_ID", ""),
            "auth_uri": os.environ.get("AUTH_URI", "https://accounts.google.com/o/oauth2/auth"),
            "token_uri": os.environ.get("TOKEN_URI", "https://oauth2.googleapis.com/token"),
            "auth_provider_x509_cert_url": os.environ.get("AUTH_PROVIDER_CERT_URL", "https://www.googleapis.com/oauth2/v1/certs"),
            "client_x509_cert_url": os.environ.get("CLIENT_CERT_URL", "")
        }
    
    def _initialize_firebase(self):
        """Firebase 초기화"""
        if not firebase_admin._apps:
            try:
                cred = credentials.Certificate(self.firebase_config)
                firebase_admin.initialize_app(cred, {
                    "databaseURL": os.environ.get("DATABASE_URL", ""),
                    "storageBucket": os.environ.get("STORAGE_BUCKET", "")
                })
            except ValueError as e:
                logger.error(f"Firebase credential error: {e}")
                raise
        self.firestore_db = firestore.client()
        self.realtime_db = db.reference("/")
        self.storage_bucket = storage.bucket()
        
    def _initialize_mock_firebase(self):
        """테스트용 Mock Firebase 초기화"""
        logger.info("Initializing mock Firebase objects")
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
                        
                    def to_dict(self):
                        return {"id": 1, "name": "Test Document"}
                return MockSnapshot()
                
            def stream(self, *args, **kwargs):
                class MockDoc:
                    def __init__(self, doc_id, data):
                        self.id = doc_id
                        self._data = data
                        
                    def to_dict(self):
                        return self._data
                
                # 몇 가지 샘플 문서 반환
                return [
                    MockDoc("doc1", {"id": 1, "name": "Document 1"}),
                    MockDoc("doc2", {"id": 2, "name": "Document 2"})
                ]
                
            def child(self, *args, **kwargs):
                return self
                
            def delete(self, *args, **kwargs):
                return None
                
            def update(self, *args, **kwargs):
                return None
                
            def order_by(self, *args, **kwargs):
                return self
                
            def limit(self, *args, **kwargs):
                return self
                
            def transaction(self, *args, **kwargs):
                return self
        
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
                
            def upload_from_string(self, *args, **kwargs):
                return None
                
            def download_as_string(self, *args, **kwargs):
                return b"mock file content"
                
            def download_to_filename(self, *args, **kwargs):
                with open(args[0], 'w') as f:
                    f.write("mock file content")
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


# 싱글톤 인스턴스 생성 전에 환경 변수 상태 확인
logger.info("Initializing Firebase Manager")
if "DEVELOPMENT" in os.environ:
    logger.info(f"DEVELOPMENT={os.environ['DEVELOPMENT']} is set")
if "TESTING" in os.environ:
    logger.info(f"TESTING={os.environ['TESTING']} is set")

# 싱글톤 인스턴스 생성
firebase_manager = FirebaseManager()

# 외부에서 쉽게 접근할 수 있도록 변수 제공
firestore_db = firebase_manager.firestore
realtime_db = firebase_manager.rtdb
storage_bucket = firebase_manager.storage