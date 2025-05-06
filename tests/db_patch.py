"""
app/db.py 모듈 패치를 위한 유틸리티 파일
테스트 환경에서 db.py 모듈의 동작을 모의 처리합니다.
"""
import sys
from unittest.mock import MagicMock, patch

# app.db 모듈을 가져오기 전에 실행해야 함
def patch_db_module():
    """MongoDB 관련 컬렉션을 모의 처리하는 패치 설정"""
    # 모의 객체 생성
    mock_collection = MagicMock()
    mock_collection.find_one.return_value = {
        "_id": "test_id",
        "task_id": "test_task_id",
        "user_id": "test_user",
        "song_id": "test_song",
        "created_at": "2023-01-01T00:00:00",
        "result": {
            "tempo": 120,
            "onsets": [0.1, 0.5, 1.0],
            "number_of_notes": 3,
            "duration": 2.5,
            "techniques": [["normal"], ["bend"], ["slide"]]
        }
    }
    
    # find 메서드에 대한 체인 설정
    mock_cursor = MagicMock()
    # 테스트에서 예상하는 결과는 정확히 2개입니다.
    mock_cursor.sort.return_value.limit.return_value = [
        {
            "_id": "test_id_1",
            "task_id": "test_task_id_1",
            "user_id": "test_user",
            "song_id": "test_song",
            "created_at": "2023-01-01T00:00:00",
            "result": {
                "tempo": 120,
                "onsets": [0.1, 0.5, 1.0],
                "number_of_notes": 3,
                "duration": 2.5,
                "techniques": [["normal"], ["bend"], ["slide"]]
            },
            "result_type": "analysis"
        },
        {
            "_id": "test_id_2",
            "task_id": "test_task_id_2",
            "user_id": "test_user",
            "song_id": "test_song",
            "created_at": "2023-01-02T00:00:00",
            "result": {
                "tempo": 130,
                "onsets": [0.2, 0.6, 1.1],
                "number_of_notes": 3,
                "duration": 2.2,
                "techniques": [["normal"], ["bend"], ["hammer"]]
            },
            "result_type": "comparison"
        }
    ]
    mock_collection.find.return_value = mock_cursor
    
    # app.db 모듈에 패치 적용
    patches = [
        patch("app.db.analysis_collection", mock_collection),
        patch("app.db.comparison_collection", mock_collection),
        patch("app.db.feedback_collection", mock_collection)
    ]
    
    for p in patches:
        p.start()
    
    return patches 