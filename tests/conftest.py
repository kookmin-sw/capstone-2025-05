import os
import sys
import pytest
from unittest.mock import MagicMock, patch
import json
from fastapi.testclient import TestClient
import numpy as np

# 프로젝트 루트 디렉토리를 Python 경로에 추가
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

# bson 및 pymongo 모의 객체 생성
class MockObjectId:
    def __init__(self, oid=None):
        self.oid = oid or "test-object-id"
    
    def __str__(self):
        return str(self.oid)

# 가짜 bson.errors 모듈
class MockBsonErrors:
    class InvalidDocument(Exception): pass
    class BSONError(Exception): pass

# 가짜 pymongo.errors 모듈
class MockPymongoErrors:
    class ConfigurationError(Exception): pass
    class ConnectionFailure(Exception): pass
    class OperationFailure(Exception): pass

# pytest 구성 후크 - 모든 테스트 전에 실행됨
def pytest_configure(config):
    """pytest 실행 시 가장 먼저 실행되는 구성 함수"""
    print("\n테스트 준비: pymongo 및 bson 가짜 모듈 설정")
    
    # bson 모듈 설정
    mock_bson = MagicMock()
    mock_bson.ObjectId = MockObjectId
    mock_bson.errors = MockBsonErrors
    
    # pymongo 모듈 및 관련 모듈 설정
    mock_pymongo = MagicMock()
    
    # MockMongoClient 클래스를 생성하여 __getitem__ 메서드 구현
    class MockMongoClient(MagicMock):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs)
            self.mock_db = MagicMock()
            
            # 컬렉션 접근을 위한 중첩된 __getitem__ 구현
            self.mock_db.__getitem__ = lambda collection_name: MagicMock()
        
        def __getitem__(self, db_name):
            return self.mock_db
    
    mock_pymongo.MongoClient = MockMongoClient
    mock_pymongo.errors = MockPymongoErrors
    mock_pymongo.collection = MagicMock()
    mock_pymongo.collection.Collection = MagicMock
    mock_pymongo.write_concern = MagicMock()
    mock_pymongo.write_concern.WriteConcern = MagicMock
    mock_pymongo._csot = MagicMock()
    
    # Celery 모의 모듈 가져오기
    try:
        import tests.celery_mock
    except ImportError:
        print("경고: tests.celery_mock 모듈을 가져올 수 없습니다.")
    
    # 모듈 모의 적용
    sys.modules["bson"] = mock_bson
    sys.modules["bson.errors"] = mock_bson.errors
    sys.modules["pymongo"] = mock_pymongo
    sys.modules["pymongo.errors"] = mock_pymongo.errors
    sys.modules["pymongo.collection"] = mock_pymongo.collection
    sys.modules["pymongo.write_concern"] = mock_pymongo.write_concern
    sys.modules["pymongo._csot"] = mock_pymongo._csot

    # timeout 마커 등록
    config.addinivalue_line(
        "markers", "timeout(seconds): 테스트 실행 제한 시간을 설정합니다."
    )

# 모듈 모의 처리 설정
@pytest.fixture(autouse=True, scope="session")
def mock_imports():
    # pytest_configure에서 이미 설정했으므로 여기서는 아무것도 하지 않음
    yield

# Celery 모의 객체 설정
@pytest.fixture(scope="function")
def mock_celery():
    """Celery 태스크 및 결과 객체를 모의합니다."""
    mock_task = MagicMock()
    mock_task.id = "test_task_id"
    mock_task.state = "SUCCESS"
    mock_task.result = {
        "tempo": 120,
        "onsets": [0.1, 0.5, 1.0],
        "number_of_notes": 3,
        "duration": 2.5,
        "techniques": [["normal"], ["bend"], ["slide"]],
        "metadata": {
            "user_id": "test_user",
            "song_id": "test_song",
            "task_id": "test_task_id"
        }
    }
    mock_task.info = {"progress": 100}
    
    # AsyncResult 클래스 모의
    class MockAsyncResult:
        def __init__(self, task_id):
            self.id = task_id
            self.state = "SUCCESS"
            self.result = mock_task.result
            self.info = {"progress": 100}
    
    # DSP 함수 모의 - extract_chroma를 추가합니다
    with patch("workers.dsp.extract_chroma") as mock_extract_chroma:
        # 모의 크로마 데이터 반환
        mock_extract_chroma.return_value = np.array([[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]])
        
        # analyze_audio, compare_audio 및 analyze_reference_audio 모의
        with patch("workers.tasks.analyze_audio") as mock_analyze, \
             patch("workers.tasks.compare_audio") as mock_compare, \
             patch("workers.tasks.analyze_reference_audio") as mock_analyze_reference:
            # delay 메서드가 모의 태스크를 반환하도록 설정
            mock_analyze.delay.return_value = mock_task
            mock_compare.delay.return_value = mock_task
            mock_analyze_reference.delay.return_value = mock_task
            
            # AsyncResult 메서드가 모의 결과를 반환하도록 설정
            mock_analyze.AsyncResult.side_effect = MockAsyncResult
            mock_compare.AsyncResult.side_effect = MockAsyncResult
            mock_analyze_reference.AsyncResult.side_effect = MockAsyncResult
            
            yield

# app.db 모듈에 대한 모의 객체 생성
@pytest.fixture(scope="function")
def mock_db():
    # db_patch 모듈에서 패치 설정
    try:
        from tests.db_patch import patch_db_module
        patches = patch_db_module()
        yield
        # 패치 중지
        for p in patches:
            p.stop()
    except ImportError:
        print("경고: tests.db_patch 모듈을 가져올 수 없습니다.")
        yield
        
# 앱 클라이언트 픽스처
@pytest.fixture
def client(mock_db, mock_celery):
    # 실제 앱을 사용하기 전에 db 모의 객체가 설정되도록 여기서 임포트
    from app.main import app
    with TestClient(app) as test_client:
        yield test_client