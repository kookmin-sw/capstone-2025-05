"""
Celery 모듈을 모의 처리하는 파일입니다.
테스트 환경에서 실제 Celery가 없을 때 사용됩니다.
"""
import sys
from unittest.mock import MagicMock


class MockTask(MagicMock):
    """모의 Celery 태스크 클래스"""
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.id = "test_task_id"
        self.state = "SUCCESS"
        self.info = {"progress": 100}
        self.result = {
            "tempo": 120,
            "onsets": [0.1, 0.5, 1.0],
            "duration": 2.5,
            "techniques": [["normal"], ["bend"], ["slide"]],
            "metadata": {
                "user_id": "test_user",
                "song_id": "test_song"
            }
        }
    
    def delay(self, *args, **kwargs):
        """모의 delay 메서드"""
        return self
    
    def AsyncResult(self, task_id):
        """모의 AsyncResult 메서드"""
        result = MagicMock()
        result.id = task_id
        result.state = "SUCCESS"
        result.result = self.result
        return result


class Celery(MagicMock):
    """모의 Celery 클래스"""
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.tasks = {}
    
    def task(self, *args, **kwargs):
        """함수를 태스크로 등록하는 데코레이터"""
        def decorator(func):
            mock_task = MockTask()
            self.tasks[func.__name__] = mock_task
            return mock_task
        return decorator
    
    def config_from_object(self, config_name):
        """설정 파일에서 구성 가져오기"""
        return self  # 설정 로드 없이 자기 자신 반환


# 가짜 celery.utils 모듈 설정
mock_utils = MagicMock()
mock_log = MagicMock()
mock_log.get_task_logger = MagicMock(return_value=MagicMock())
mock_utils.log = mock_log

# 각종 모의 하위 모듈 설정
mock_result = MagicMock()
mock_result.AsyncResult = MockTask().AsyncResult

# 가짜 celeryconfig 모듈 생성
mock_celeryconfig = MagicMock()
mock_celeryconfig.broker_url = "memory://"
mock_celeryconfig.result_backend = "rpc://"
mock_celeryconfig.task_serializer = "json"
mock_celeryconfig.result_serializer = "json"
mock_celeryconfig.accept_content = ["json"]
mock_celeryconfig.enable_utc = True
mock_celeryconfig.task_track_started = True
mock_celeryconfig.task_time_limit = 30 * 60  # 30분
mock_celeryconfig.worker_prefetch_multiplier = 1
mock_celeryconfig.task_acks_late = True

# 이미 celery가 sys.modules에 등록되어 있는지 확인
if 'celery' not in sys.modules:
    # 가짜 celery 모듈 생성
    mock_celery = MagicMock()
    mock_celery.Celery = Celery
    mock_celery.Task = MockTask
    mock_celery.utils = mock_utils
    mock_celery.result = mock_result
    
    # 모듈 등록
    sys.modules['celery'] = mock_celery
    sys.modules['celery.utils'] = mock_utils
    sys.modules['celery.utils.log'] = mock_log
    sys.modules['celery.result'] = mock_result
    sys.modules['celeryconfig'] = mock_celeryconfig 

# workers.tasks 모듈에 가짜 함수 등록
try:
    import workers.tasks
    # 모의 태스크 객체 생성 (이미 존재할 수 있으므로 AttributeError 무시)
    try:
        workers.tasks.analyze_reference_audio = MockTask()
        print("Successfully mocked analyze_reference_audio")
    except AttributeError:
        pass
except ImportError:
    print("Warning: Could not import workers.tasks module to mock functions") 