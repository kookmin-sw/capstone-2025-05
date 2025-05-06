import os
from pymongo import MongoClient
from pymongo.collection import Collection
from typing import Dict, Any

# MongoDB 연결 문자열을 환경 변수에서 가져옵니다
MONGO_URI = os.environ.get("MONGO_URI", "mongodb://mongo:27017/")
MONGO_DB_NAME = os.environ.get("MONGO_DB_NAME", "maple_audio_db")

# MongoDB 클라이언트와 컬렉션 초기화
client = MongoClient(MONGO_URI)
db = client[MONGO_DB_NAME]

# 분석 결과 저장을 위한 컬렉션
analysis_collection = db["analysis_results"]
# 비교 결과 저장을 위한 컬렉션
comparison_collection = db["comparison_results"]
# 피드백 저장을 위한 컬렉션
feedback_collection = db["feedback_results"]

def save_analysis_result(task_id: str, result: Dict[str, Any]) -> str:
    """
    분석 결과를 MongoDB에 저장합니다.
    
    Args:
        task_id: Celery 작업 ID
        result: 분석 결과 데이터
        
    Returns:
        저장된 문서의 ID
    """
    # task_id를 키로 사용하여 결과 저장
    document = {
        "task_id": task_id,
        "result": result,
        "created_at": result.get("created_at", None)
    }
    
    # user_id와 song_id가 있으면 저장
    if "metadata" in result and result["metadata"]:
        if "user_id" in result["metadata"]:
            document["user_id"] = result["metadata"]["user_id"]
        if "song_id" in result["metadata"]:
            document["song_id"] = result["metadata"]["song_id"]
    
    # 이미 같은 task_id로 저장된 문서가 있는지 확인
    existing = analysis_collection.find_one({"task_id": task_id})
    if existing:
        # 이미 있는 경우 업데이트
        analysis_collection.update_one(
            {"task_id": task_id},
            {"$set": document}
        )
        return str(existing["_id"])
    else:
        # 새로 저장
        inserted = analysis_collection.insert_one(document)
        return str(inserted.inserted_id)

def save_comparison_result(task_id: str, result: Dict[str, Any]) -> str:
    """
    비교 분석 결과를 MongoDB에 저장합니다.
    
    Args:
        task_id: Celery 작업 ID
        result: 비교 분석 결과 데이터
        
    Returns:
        저장된 문서의 ID
    """
    # task_id를 키로 사용하여 결과 저장
    document = {
        "task_id": task_id,
        "result": result,
        "created_at": result.get("created_at", None)
    }
    
    # user_id와 song_id가 있으면 저장
    if "metadata" in result and result["metadata"]:
        if "user_id" in result["metadata"]:
            document["user_id"] = result["metadata"]["user_id"]
        if "song_id" in result["metadata"]:
            document["song_id"] = result["metadata"]["song_id"]
    
    # 이미 같은 task_id로 저장된 문서가 있는지 확인
    existing = comparison_collection.find_one({"task_id": task_id})
    if existing:
        # 이미 있는 경우 업데이트
        comparison_collection.update_one(
            {"task_id": task_id},
            {"$set": document}
        )
        return str(existing["_id"])
    else:
        # 새로 저장
        inserted = comparison_collection.insert_one(document)
        return str(inserted.inserted_id)

def save_feedback(task_id: str, feedback_data: Dict[str, Any]) -> str:
    """
    생성된 피드백을 MongoDB에 저장합니다.
    
    Args:
        task_id: Celery 작업 ID
        feedback_data: 피드백 데이터
        
    Returns:
        저장된 문서의 ID
    """
    document = {
        "task_id": task_id,
        "feedback": feedback_data.get("feedback"),
        "metadata": feedback_data.get("feedback_metadata", {}),
        "created_at": feedback_data.get("created_at", None)
    }
    
    # 이미 같은 task_id로 저장된 문서가 있는지 확인
    existing = feedback_collection.find_one({"task_id": task_id})
    if existing:
        # 이미 있는 경우 업데이트
        feedback_collection.update_one(
            {"task_id": task_id},
            {"$set": document}
        )
        return str(existing["_id"])
    else:
        # 새로 저장
        inserted = feedback_collection.insert_one(document)
        return str(inserted.inserted_id)

def get_analysis_result(task_id: str) -> Dict[str, Any]:
    """
    MongoDB에서 분석 결과를 가져옵니다.
    
    Args:
        task_id: Celery 작업 ID
        
    Returns:
        분석 결과 데이터 또는 None
    """
    result = analysis_collection.find_one({"task_id": task_id})
    return result

def get_comparison_result(task_id: str) -> Dict[str, Any]:
    """
    MongoDB에서 비교 결과를 가져옵니다.
    
    Args:
        task_id: Celery 작업 ID
        
    Returns:
        비교 결과 데이터 또는 None
    """
    result = comparison_collection.find_one({"task_id": task_id})
    return result

def get_result(task_id: str) -> Dict[str, Any]:
    """
    MongoDB에서 분석 또는 비교 결과를 가져옵니다.
    
    Args:
        task_id: Celery 작업 ID
        
    Returns:
        분석 또는 비교 결과 데이터 또는 None
    """
    # 먼저 분석 결과에서 조회
    result = analysis_collection.find_one({"task_id": task_id})
    if result:
        result["result_type"] = "analysis"
        return result
    
    # 비교 결과에서 조회
    result = comparison_collection.find_one({"task_id": task_id})
    if result:
        result["result_type"] = "comparison"
    return result

def get_user_analysis_results(user_id: str, limit: int = 10) -> list:
    """
    특정 사용자의 분석 결과를 가져옵니다.
    
    Args:
        user_id: 사용자 ID
        limit: 최대 결과 수
        
    Returns:
        분석 결과 목록
    """
    cursor = analysis_collection.find(
        {"user_id": user_id}
    ).sort("created_at", -1).limit(limit)
    
    return list(cursor)

def get_user_comparison_results(user_id: str, limit: int = 10) -> list:
    """
    특정 사용자의 비교 결과를 가져옵니다.
    
    Args:
        user_id: 사용자 ID
        limit: 최대 결과 수
        
    Returns:
        비교 결과 목록
    """
    cursor = comparison_collection.find(
        {"user_id": user_id}
    ).sort("created_at", -1).limit(limit)
    
    return list(cursor)

def get_song_analysis_results(song_id: str, limit: int = 10) -> list:
    """
    특정 곡의 분석 결과를 가져옵니다.
    
    Args:
        song_id: 곡 ID
        limit: 최대 결과 수
        
    Returns:
        분석 결과 목록
    """
    cursor = analysis_collection.find(
        {"song_id": song_id}
    ).sort("created_at", -1).limit(limit)
    
    return list(cursor)

def get_song_comparison_results(song_id: str, limit: int = 10) -> list:
    """
    특정 곡의 비교 결과를 가져옵니다.
    
    Args:
        song_id: 곡 ID
        limit: 최대 결과 수
        
    Returns:
        비교 결과 목록
    """
    cursor = comparison_collection.find(
        {"song_id": song_id}
    ).sort("created_at", -1).limit(limit)
    
    return list(cursor) 