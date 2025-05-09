from fastapi import APIRouter, HTTPException, Response, status
from fastapi.responses import FileResponse
from typing import List, Optional
import uuid
import os
from pathlib import Path

from app.models.song import Song, SongCreate, SongUpdate

router = APIRouter(
    tags=["songs"],
    responses={404: {"description": "Not found"}},
)

# 메모리에 저장할 가상의 DB
songs_db = []

# 테스트용 더미 데이터 생성
def create_test_data():
    test_songs = [
        {
            "song_id": str(uuid.uuid4()),
            "title": "26",
            "artist": "Younha",
            "thumbnail": "static/thumbnails/26.jpg",
            "sheet_music": "static/sheet_music/26.gp5",
            "audio": "static/audio/26.wav",
            "bpm": 92,
            "duration": 29.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "FLY",
            "artist": "YdBB",
            "thumbnail": "static/thumbnails/fly.jpg",
            "sheet_music": "static/sheet_music/fly.gp5",
            "audio": "static/audio/fly.wav",
            "bpm": 104,
            "duration": 20.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Mela!",
            "artist": "Ryokuoushoku Shakai",
            "thumbnail": "static/thumbnails/mela.jpg",
            "sheet_music": "static/sheet_music/mela.gp5",
            "audio": "static/audio/mela.wav",
            "bpm": 138,
            "duration": 28.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Happy",
            "artist": "DAY6",
            "thumbnail": "static/thumbnails/happy.jpg",
            "sheet_music": "static/sheet_music/26.gp5",
            "audio": "static/audio/26.wav",
            "bpm": 160,
            "duration": 35.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Sunfish",
            "artist": "Younha",
            "thumbnail": "static/thumbnails/sunfish.jpg",
            "sheet_music": "static/sheet_music/fly.gp5",
            "audio": "static/audio/fly.wav",
            "bpm": 142,
            "duration": 25.7
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Night Night",
            "artist": "YdBB",
            "thumbnail": "static/thumbnails/night_night.jpg",
            "sheet_music": "static/sheet_music/mela.gp5",
            "audio": "static/audio/mela.wav",
            "bpm": 120,
            "duration": 32.5
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Rebel Heart",
            "artist": "IVE",
            "thumbnail": "static/thumbnails/rebel_heart.jpg",
            "sheet_music": "static/sheet_music/26.gp5",
            "audio": "static/audio/26.wav",
            "bpm": 95,
            "duration": 40.2
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "No Pain",
            "artist": "Silicagel",
            "thumbnail": "static/thumbnails/no_pain.jpg",
            "sheet_music": "static/sheet_music/fly.gp5",
            "audio": "static/audio/fly.wav",
            "bpm": 128,
            "duration": 27.8
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Some Like It Hot",
            "artist": "SPYAIR",
            "thumbnail": "static/thumbnails/some_like_it_hot.jpg",
            "sheet_music": "static/sheet_music/mela.gp5",
            "audio": "static/audio/mela.wav",
            "bpm": 115,
            "duration": 33.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Pretender",
            "artist": "Official HIGE DANdism",
            "thumbnail": "static/thumbnails/pretender.jpg",
            "sheet_music": "static/sheet_music/26.gp5",
            "audio": "static/audio/26.wav",
            "bpm": 92,
            "duration": 38.5
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Event Horizon",
            "artist": "Younha",
            "thumbnail": "static/thumbnails/event_horizon.jpg",
            "sheet_music": "static/sheet_music/fly.gp5",
            "audio": "static/audio/fly.wav",
            "bpm": 150,
            "duration": 22.3
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Can't Keep Smiling",
            "artist": "YdBB",
            "thumbnail": "static/thumbnails/can't_keep_smiling.jpg",
            "sheet_music": "static/sheet_music/mela.gp5",
            "audio": "static/audio/mela.wav",
            "bpm": 98,
            "duration": 30.2
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "T.B.H.",
            "artist": "QWER",
            "thumbnail": "static/thumbnails/t.b.h.jpg",
            "sheet_music": "static/sheet_music/26.gp5",
            "audio": "static/audio/26.wav",
            "bpm": 88,
            "duration": 25.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Karma",
            "artist": "Dareharu",
            "thumbnail": "static/thumbnails/karma.jpg",
            "sheet_music": "static/sheet_music/fly.gp5",
            "audio": "static/audio/fly.wav",
            "bpm": 130,
            "duration": 27.9
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Character",
            "artist": "Ryokuoushoku Shakai",
            "thumbnail": "static/thumbnails/character.jpg",
            "sheet_music": "static/sheet_music/mela.gp5",
            "audio": "static/audio/mela.wav",
            "bpm": 112,
            "duration": 31.5
        }
    ]
    
    for song in test_songs:
        songs_db.append(Song(**song))

# 애플리케이션 시작시 더미 데이터 로드
create_test_data()

@router.get("", response_model=List[Song])
def get_songs():
    """
    모든 곡 목록을 조회합니다.
    """
    return songs_db

@router.get("/{song_id}", response_model=Song)
def get_song(song_id: str):
    """
    특정 ID의 곡 정보를 조회합니다.
    """
    for song in songs_db:
        if song.song_id == song_id:
            return song
    raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")

@router.post("", response_model=Song, status_code=status.HTTP_201_CREATED)
def create_song(song: SongCreate):
    """
    새로운 곡을 추가합니다.
    이미 동일한 song_id가 존재하는 경우 해당 곡을 업데이트합니다.
    song_id를 제공하지 않으면 자동으로 생성됩니다.
    """
    song_dict = song.dict(exclude_unset=True)
    
    # song_id 처리
    song_id = song_dict.pop("song_id", None)
    
    if song_id:
        # song_id가 제공된 경우, 기존 곡 검색
        for idx, existing_song in enumerate(songs_db):
            if existing_song.song_id == song_id:
                # 기존 곡 업데이트
                updated_song = existing_song.dict()
                
                # 업데이트할 필드 갱신
                for field, value in song_dict.items():
                    updated_song[field] = value
                
                # 객체 갱신
                songs_db[idx] = Song(**updated_song)
                return songs_db[idx]
    
    # 새 song_id 생성 (제공되지 않았거나, 제공된 ID로 기존 곡을 찾지 못한 경우)
    if not song_id:
        song_id = str(uuid.uuid4())
        
    # 새로운 곡 추가
    song_dict["song_id"] = song_id
    db_song = Song(**song_dict)
    songs_db.append(db_song)
    return db_song

@router.put("/{song_id}", response_model=Song)
def update_song(song_id: str, song: SongUpdate):
    """
    특정 ID의 곡 정보를 수정합니다.
    """
    for idx, db_song in enumerate(songs_db):
        if db_song.song_id == song_id:
            update_data = song.dict(exclude_unset=True)
            
            # 기존 데이터를 딕셔너리로 변환
            updated_song = db_song.dict()
            
            # 업데이트할 필드만 갱신
            for field, value in update_data.items():
                if value is not None:
                    updated_song[field] = value
            
            # 객체 갱신
            songs_db[idx] = Song(**updated_song)
            return songs_db[idx]
    
    raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")

@router.delete("/{song_id}", status_code=status.HTTP_204_NO_CONTENT)
def delete_song(song_id: str):
    """
    특정 ID의 곡을 삭제합니다.
    """
    for idx, song in enumerate(songs_db):
        if song.song_id == song_id:
            songs_db.pop(idx)
            return Response(status_code=status.HTTP_204_NO_CONTENT)
    
    raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")

@router.get("/{song_id}/sheet")
def get_sheet_music(song_id: str):
    """
    특정 ID의 곡 악보 파일을 다운로드합니다.
    """
    for song in songs_db:
        if song.song_id == song_id:
            # 경로 수정: 상대 경로 사용
            file_path = song.sheet_music
            file_path = os.path.join("app", file_path)
            
            if os.path.exists(file_path):
                return FileResponse(file_path, filename=Path(file_path).name)
            else:
                raise HTTPException(status_code=404, detail=f"악보 파일을 찾을 수 없습니다: {file_path}")
    
    raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")

@router.get("/{song_id}/audio")
def get_audio(song_id: str):
    """
    특정 ID의 곡 음원 파일을 다운로드/스트리밍합니다.
    """
    for song in songs_db:
        if song.song_id == song_id:
            # 경로 수정: 상대 경로 사용
            file_path = song.audio
            file_path = os.path.join("app", file_path)
            
            if os.path.exists(file_path):
                return FileResponse(
                    file_path, 
                    filename=Path(file_path).name,
                    media_type="audio/mpeg"
                )
            else:
                raise HTTPException(status_code=404, detail=f"음원 파일을 찾을 수 없습니다: {file_path}")
    
    raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")

@router.get("/{song_id}/thumbnail")
def get_thumbnail(song_id: str):
    """
    특정 ID의 곡 앨범 커버 이미지를 다운로드합니다.
    """
    for song in songs_db:
        if song.song_id == song_id:
            # 경로 수정: 상대 경로 사용
            file_path = song.thumbnail
            file_path = os.path.join("app", file_path)
            
            if os.path.exists(file_path):
                return FileResponse(
                    file_path, 
                    filename=Path(file_path).name,
                    media_type="image/jpeg"
                )
            else:
                raise HTTPException(status_code=404, detail=f"앨범 커버 이미지를 찾을 수 없습니다: {file_path}")
    
    raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다") 