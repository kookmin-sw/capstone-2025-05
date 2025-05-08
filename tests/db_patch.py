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
    
    # 레퍼런스 특성 컬렉션을 위한 모의 객체 생성
    mock_reference_collection = MagicMock()
    # 일반적인 song_id에 대한 모의 데이터
    mock_reference_collection.find_one.return_value = {
        "_id": "test_ref_id",
        "song_id": "test_song",
        "created_at": "2023-01-01T00:00:00",
        "tempo": 120,
        "onsets": [0.1, 0.5, 1.0],
        "pitches": [440, 554, 659],
        "techniques": [["normal"], ["bend"], ["slide"]],
        "chroma": [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]],
        "metadata": {
            "song_id": "test_song",
            "has_midi": True
        },
        "midi_data": {
            "notes": [
                {"start": 0.1, "end": 0.4, "pitch": 60, "velocity": 100},
                {"start": 0.5, "end": 0.9, "pitch": 62, "velocity": 100},
                {"start": 1.0, "end": 1.4, "pitch": 64, "velocity": 100}
            ],
            "tempos": [120],
            "tempo_times": [0]
        }
    }
    
    # test_song_with_chroma에 대한 모의 데이터
    def mock_find_one_side_effect(query):
        song_id = query.get("song_id")
        if song_id == "test_song_with_chroma":
            return {
                "_id": "test_ref_id_chroma",
                "song_id": "test_song_with_chroma",
                "created_at": "2023-01-01T00:00:00",
                "tempo": 120,
                "onsets": [0.1, 0.5, 1.0, 1.5],
                "pitches": [440, 554, 659, 880],
                "techniques": [["normal"], ["bend"], ["slide"], ["hammer"]],
                "chroma": [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]],
                "metadata": {
                    "song_id": "test_song_with_chroma",
                    "has_midi": True
                },
                "midi_data": {
                    "notes": [
                        {"start": 0.1, "end": 0.4, "pitch": 60, "velocity": 100},
                        {"start": 0.5, "end": 0.9, "pitch": 62, "velocity": 100},
                        {"start": 1.0, "end": 1.4, "pitch": 64, "velocity": 100}
                    ],
                    "tempos": [120],
                    "tempo_times": [0]
                }
            }
        return mock_reference_collection.find_one.return_value
    
    mock_reference_collection.find_one.side_effect = mock_find_one_side_effect
    
    # 레퍼런스 목록을 위한 모의 데이터
    mock_ref_cursor = MagicMock()
    mock_ref_cursor.limit.return_value = [
        {
            "_id": "test_ref_id_1",
            "song_id": "test_song_1",
            "created_at": "2023-01-01T00:00:00",
            "tempo": 120,
            "onsets": [0.1, 0.5, 1.0],
            "metadata": {"song_id": "test_song_1"}
        },
        {
            "_id": "test_ref_id_2",
            "song_id": "test_song_2",
            "created_at": "2023-01-02T00:00:00",
            "tempo": 130,
            "onsets": [0.2, 0.6, 1.1],
            "metadata": {"song_id": "test_song_2"}
        }
    ]
    mock_reference_collection.find.return_value = mock_ref_cursor
    
    # app.db 모듈에 패치 적용
    patches = [
        patch("app.db.analysis_collection", mock_collection),
        patch("app.db.comparison_collection", mock_collection),
        patch("app.db.feedback_collection", mock_collection),
        patch("app.db.reference_features_collection", mock_reference_collection)
    ]
    
    for p in patches:
        p.start()
    
    return patches 