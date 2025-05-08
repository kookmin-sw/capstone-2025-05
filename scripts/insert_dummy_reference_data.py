#!/usr/bin/env python
"""
프론트엔드 팀을 위한 더미 레퍼런스 데이터를 MongoDB에 넣는 스크립트
기존의 result_2e6d83fe.json 파일의 데이터를 사용하여 여러 song_id에 동일한 데이터를 복제합니다.
"""
import os
import sys
import json
from pathlib import Path
from pymongo import MongoClient
from bson.objectid import ObjectId
from datetime import datetime

# 현재 스크립트 경로에서 프로젝트 루트 디렉토리 추가
current_dir = Path(__file__).parent
project_root = current_dir.parent
sys.path.append(str(project_root))

# MongoDB 연결 정보
MONGO_URI = os.environ.get("MONGO_URI", "mongodb://analysis.maple.ne.kr:27017/")
DB_NAME = os.environ.get("MONGO_DB", "maple_audio")

# 더미 곡 정보 리스트
DUMMY_SONGS = [
    {
        "title": "26",
        "artist": "Younha",
        "thumbnail": "/static/thumbnails/26.jpg",
        "sheet_music": "/static/sheet_music/26.gp5",
        "audio": "/static/audio/26.wav",
        "bpm": 92,
        "duration": 29,
        "song_id": "5da3470d-c2fa-4ae8-a035-63404a44db07"
    },
    {
        "title": "FLY",
        "artist": "YdBB",
        "thumbnail": "/static/thumbnails/fly.jpg",
        "sheet_music": "/static/sheet_music/fly.gp5",
        "audio": "/static/audio/fly.wav",
        "bpm": 104,
        "duration": 20,
        "song_id": "30273e89-8c20-4c29-bdbd-8c00b59880b6"
    },
    {
        "title": "Mela!",
        "artist": "Ryokuoushoku Shakai",
        "thumbnail": "/static/thumbnails/mela.jpg",
        "sheet_music": "/static/sheet_music/mela.gp5",
        "audio": "/static/audio/mela.wav",
        "bpm": 138,
        "duration": 28,
        "song_id": "c29418ea-a67b-4ed6-a129-23d4ab317e7d"
    },
    {
        "title": "Happy",
        "artist": "DAY6",
        "thumbnail": "/static/thumbnails/happy.jpg",
        "sheet_music": "/static/sheet_music/26.gp5",
        "audio": "/static/audio/26.wav",
        "bpm": 160,
        "duration": 35,
        "song_id": "7fa082f7-cd4e-4435-9888-3ccdc0202086"
    },
    {
        "title": "Sunfish",
        "artist": "Ado",
        "thumbnail": "/static/thumbnails/sunfish.jpg",
        "sheet_music": "/static/sheet_music/fly.gp5",
        "audio": "/static/audio/fly.wav",
        "bpm": 142,
        "duration": 25.7,
        "song_id": "dd45959d-a0d8-4637-9111-a1f9da2f47f4"
    },
    {
        "title": "Night Night",
        "artist": "YdBB",
        "thumbnail": "/static/thumbnails/night_night.jpg",
        "sheet_music": "/static/sheet_music/mela.gp5",
        "audio": "/static/audio/mela.wav",
        "bpm": 120,
        "duration": 32.5,
        "song_id": "8ac7631b-937e-434c-9da7-1592c5a31fa8"
    },
    {
        "title": "Rebel Heart",
        "artist": "IVE",
        "thumbnail": "/static/thumbnails/rebel_heart.jpg",
        "sheet_music": "/static/sheet_music/26.gp5",
        "audio": "/static/audio/26.wav",
        "bpm": 95,
        "duration": 40.2,
        "song_id": "8f6c65c6-f651-4b26-9c16-e09ca4452a4f"
    },
    {
        "title": "No Pain",
        "artist": "Silicagel",
        "thumbnail": "/static/thumbnails/no_pain.jpg",
        "sheet_music": "/static/sheet_music/fly.gp5",
        "audio": "/static/audio/fly.wav",
        "bpm": 128,
        "duration": 27.8,
        "song_id": "d8aa2c00-0fba-41ff-9826-0723734c781f"
    },
    {
        "title": "Some Like It Hot",
        "artist": "SPYAIR",
        "thumbnail": "/static/thumbnails/some_like_it_hot.jpg",
        "sheet_music": "/static/sheet_music/mela.gp5",
        "audio": "/static/audio/mela.wav",
        "bpm": 115,
        "duration": 33,
        "song_id": "97f4821e-81f7-47e9-9057-1870991e8250"
    },
    {
        "title": "Pretender",
        "artist": "Official HIGE DANdism",
        "thumbnail": "/static/thumbnails/pretender.jpg",
        "sheet_music": "/static/sheet_music/26.gp5",
        "audio": "/static/audio/26.wav",
        "bpm": 92,
        "duration": 38.5,
        "song_id": "1ede7c35-e768-43d1-879c-e53fee5aece0"
    },
    {
        "title": "Event Horizon",
        "artist": "Younha",
        "thumbnail": "/static/thumbnails/event_horizon.jpg",
        "sheet_music": "/static/sheet_music/fly.gp5",
        "audio": "/static/audio/fly.wav",
        "bpm": 150,
        "duration": 22.3,
        "song_id": "f27e1869-e804-4d61-8fd5-79a66802267d"
    },
    {
        "title": "Can't Keep Smiling",
        "artist": "YdBB",
        "thumbnail": "/static/thumbnails/can't_keep_smiling.jpg",
        "sheet_music": "/static/sheet_music/mela.gp5",
        "audio": "/static/audio/mela.wav",
        "bpm": 98,
        "duration": 30.2,
        "song_id": "2522ee96-fb5d-48c7-b5f4-c708e1122a4d"
    },
    {
        "title": "T.B.H.",
        "artist": "QWER",
        "thumbnail": "/static/thumbnails/t.b.h.jpg",
        "sheet_music": "/static/sheet_music/26.gp5",
        "audio": "/static/audio/26.wav",
        "bpm": 88,
        "duration": 25,
        "song_id": "043ee579-d8fa-4915-86d2-bda88deb78f8"
    },
    {
        "title": "Karma",
        "artist": "Dareharu",
        "thumbnail": "/static/thumbnails/karma.jpg",
        "sheet_music": "/static/sheet_music/fly.gp5",
        "audio": "/static/audio/fly.wav",
        "bpm": 130,
        "duration": 27.9,
        "song_id": "888e34d6-4824-4a35-8381-5c50ac0f3f9a"
    },
    {
        "title": "Character",
        "artist": "Ryokuoushoku Shakai",
        "thumbnail": "/static/thumbnails/character.jpg",
        "sheet_music": "/static/sheet_music/mela.gp5",
        "audio": "/static/audio/mela.wav",
        "bpm": 112,
        "duration": 31.5,
        "song_id": "0c39256c-fcf2-45d8-8369-6d47ebc260e4"
    }
]

def load_json_template(file_path):
    """템플릿으로 사용할 JSON 파일 로드"""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except Exception as e:
        print(f"오류: 템플릿 JSON 파일 로드 실패: {e}")
        sys.exit(1)

def insert_reference_features(client, template_data):
    """각 곡에 대한 레퍼런스 특성 데이터 삽입"""
    db = client[DB_NAME]
    reference_collection = db["reference_features"]
    songs_collection = db["songs"]
    
    # 기존 데이터 삭제 여부 확인
    should_delete = input("기존 데이터를 삭제하시겠습니까? (y/n): ").lower() == 'y'
    if should_delete:
        reference_collection.delete_many({})
        songs_collection.delete_many({})
        print("기존 데이터가 삭제되었습니다.")
    
    # 각 song_id에 대해 템플릿 데이터 복제 및 삽입
    inserted_refs = []
    inserted_songs = []
    
    for song in DUMMY_SONGS:
        song_id = song["song_id"]
        
        # 레퍼런스 데이터 준비
        ref_data = template_data.copy()
        
        # 메타데이터 업데이트
        ref_data["metadata"]["song_id"] = song_id
        ref_data["metadata"]["user_id"] = "JEnhRg0D9shVXoRtUsLJcpgTpYY2"
        ref_data["created_at"] = datetime.utcnow().isoformat()
        
        # BPM 값 업데이트
        ref_data["reference_features"]["tempo"] = song["bpm"]
        
        # MIDI 템포 업데이트
        if "midi_data" in ref_data and "tempos" in ref_data["midi_data"]:
            ref_data["midi_data"]["tempos"][0] = song["bpm"]
        
        try:
            # 레퍼런스 특성 데이터 삽입
            ref_result = reference_collection.insert_one(ref_data)
            
            # 곡 정보 데이터 삽입
            song_data = {
                "title": song["title"],
                "artist": song["artist"],
                "thumbnail": song["thumbnail"],
                "sheet_music": song["sheet_music"],
                "audio": song["audio"],
                "song_id": song_id,
                "user_id": "JEnhRg0D9shVXoRtUsLJcpgTpYY2",
                "bpm": song["bpm"],
                "duration": song["duration"],
                "reference_id": str(ref_result.inserted_id),
                "created_at": datetime.utcnow().isoformat()
            }
            song_result = songs_collection.insert_one(song_data)
            
            inserted_refs.append(str(ref_result.inserted_id))
            inserted_songs.append(str(song_result.inserted_id))
            
            print(f"'{song['title']}' (song_id: {song_id})에 대한 데이터가 성공적으로 삽입되었습니다.")
        except Exception as e:
            print(f"오류: {song['title']} 데이터 삽입 실패: {e}")
    
    print(f"\n총 {len(inserted_refs)}개의 레퍼런스 데이터와 {len(inserted_songs)}개의 곡 정보가 삽입되었습니다.")
    return inserted_refs, inserted_songs

def insert_comparison_results(client, template_data):
    """각 곡에 대한 비교 결과 더미 데이터 삽입"""
    db = client[DB_NAME]
    comparison_collection = db["comparison_results"]
    
    # 비교 결과 컬렉션 초기화 여부 확인
    should_delete = input("기존 비교 결과 데이터를 삭제하시겠습니까? (y/n): ").lower() == 'y'
    if should_delete:
        comparison_collection.delete_many({})
        print("기존 비교 결과 데이터가 삭제되었습니다.")
    
    # 각 song_id에 대해 비교 결과 데이터 생성 (각 3개씩)
    inserted_comparisons = []
    
    for song in DUMMY_SONGS:
        song_id = song["song_id"]
        
        # 3개의 더미 비교 결과 생성
        for i in range(3):
            # 비교 결과 데이터 준비
            comp_data = template_data.copy()
            
            # 메타데이터 업데이트
            comp_data["metadata"]["song_id"] = song_id
            comp_data["metadata"]["user_id"] = "JEnhRg0D9shVXoRtUsLJcpgTpYY2"
            comp_data["metadata"]["task_id"] = f"dummy-task-{i+1}-{song_id[:8]}"
            comp_data["created_at"] = datetime.utcnow().isoformat()
            
            # 점수 약간 수정 (더미 데이터 다양성을 위해)
            variation = (i - 1) * 5  # -5, 0, 5의 변화
            for score_key in comp_data["scores"]:
                if isinstance(comp_data["scores"][score_key], (int, float)):
                    new_score = comp_data["scores"][score_key] + variation
                    # 0-100 범위 유지
                    comp_data["scores"][score_key] = max(0, min(100, new_score))
            
            try:
                # 비교 결과 데이터 삽입
                result = comparison_collection.insert_one(comp_data)
                inserted_comparisons.append(str(result.inserted_id))
                
                print(f"'{song['title']}' (song_id: {song_id})에 대한 비교 결과 #{i+1}이 삽입되었습니다.")
            except Exception as e:
                print(f"오류: {song['title']} 비교 결과 #{i+1} 삽입 실패: {e}")
    
    print(f"\n총 {len(inserted_comparisons)}개의 비교 결과 데이터가 삽입되었습니다.")
    return inserted_comparisons

def main():
    # 템플릿 JSON 파일 로드
    template_path = project_root / "result_2e6d83fe.json"
    if not template_path.exists():
        print(f"오류: 템플릿 파일이 존재하지 않습니다: {template_path}")
        return 1
    
    template_data = load_json_template(template_path)
    
    # MongoDB 연결
    try:
        client = MongoClient(MONGO_URI)
        # 연결 확인
        client.admin.command('ping')
        print("MongoDB 연결 성공!")
    except Exception as e:
        print(f"오류: MongoDB 연결 실패: {e}")
        return 1
    
    try:
        # 레퍼런스 특성 및 곡 정보 삽입
        ref_ids, song_ids = insert_reference_features(client, template_data)
        
        # 비교 결과 삽입
        should_insert_comparisons = input("\n비교 결과 더미 데이터도 삽입하시겠습니까? (y/n): ").lower() == 'y'
        if should_insert_comparisons:
            comp_ids = insert_comparison_results(client, template_data)
        
        print("\n데이터 삽입이 완료되었습니다!")
        
    except Exception as e:
        print(f"오류: 데이터 삽입 중 문제 발생: {e}")
        return 1
    finally:
        client.close()
    
    return 0

if __name__ == "__main__":
    sys.exit(main()) 