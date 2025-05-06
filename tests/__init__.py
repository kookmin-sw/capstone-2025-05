# 테스트 환경에서 bson과 pymongo를 모의 처리합니다.
# 모든 테스트 관련 설정은 conftest.py에서 처리합니다.

import sys
from unittest.mock import MagicMock

# Celery 모의 모듈 가져오기
try:
    import tests.celery_mock
except ImportError:
    print("Celery 모의 모듈을 불러올 수 없습니다.")

# 테스트 실행 전 검증
if 'bson' not in sys.modules or 'pymongo' not in sys.modules:
    # 모듈이 아직 가져와지지 않은 경우 임시 모의 객체 생성
    # 이는 conftest.py의 pytest_configure()가 실행되기 전에 보호하기 위함입니다
    
    # 가짜 bson 모듈 객체
    mock_bson = MagicMock()
    mock_bson_errors = MagicMock()
    mock_bson.errors = mock_bson_errors
    
    # 가짜 pymongo 모듈 객체
    mock_pymongo = MagicMock()
    mock_pymongo_errors = MagicMock()
    mock_pymongo.errors = mock_pymongo_errors
    
    # MongoClient 모의 객체에 올바른 __getitem__ 구현
    class MockMongoClient(MagicMock):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)
            self.mock_db = MagicMock()
        
        def __getitem__(self, name):
            return self.mock_db
    
    mock_pymongo.MongoClient = MockMongoClient
    
    # 모듈 등록
    sys.modules['bson'] = mock_bson
    sys.modules['bson.errors'] = mock_bson_errors
    sys.modules['pymongo'] = mock_pymongo
    sys.modules['pymongo.errors'] = mock_pymongo_errors 