from fastapi import APIRouter, HTTPException, Response, status, Depends
from fastapi.responses import FileResponse
from typing import List, Optional
import uuid
import os
from pathlib import Path
from sqlalchemy.orm import Session

from app.models.song import Song, SongCreate, SongUpdate, SongModel
from app.database import get_db

router = APIRouter(
    tags=["songs"],
    responses={404: {"description": "Not found"}},
)

@router.get("", response_model=List[Song])
def get_songs(db: Session = Depends(get_db)):
    """
    모든 곡 목록을 조회합니다.
    """
    songs = db.query(SongModel).all()
    return songs

@router.get("/{song_id}", response_model=Song)
def get_song(song_id: str, db: Session = Depends(get_db)):
    """
    특정 ID의 곡 정보를 조회합니다.
    """
    song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
    if song is None:
        raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")
    return song

@router.post("", response_model=Song, status_code=status.HTTP_201_CREATED)
def create_song(song: SongCreate, db: Session = Depends(get_db)):
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
        db_song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
        if db_song:
            # 기존 곡 업데이트
            for field, value in song_dict.items():
                setattr(db_song, field, value)
            db.commit()
            db.refresh(db_song)
            return db_song
    
    # 새 song_id 생성 (제공되지 않았거나, 제공된 ID로 기존 곡을 찾지 못한 경우)
    if not song_id:
        song_id = str(uuid.uuid4())
        
    # 새로운 곡 추가
    song_dict["song_id"] = song_id
    db_song = SongModel(**song_dict)
    db.add(db_song)
    db.commit()
    db.refresh(db_song)
    return db_song

@router.put("/{song_id}", response_model=Song)
def update_song(song_id: str, song: SongUpdate, db: Session = Depends(get_db)):
    """
    특정 ID의 곡 정보를 수정합니다.
    """
    db_song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
    if db_song is None:
        raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")
    
    update_data = song.dict(exclude_unset=True)
    
    # 필드 갱신
    for field, value in update_data.items():
        if value is not None:
            setattr(db_song, field, value)
    
    db.commit()
    db.refresh(db_song)
    return db_song

@router.delete("/{song_id}", status_code=status.HTTP_204_NO_CONTENT)
def delete_song(song_id: str, db: Session = Depends(get_db)):
    """
    특정 ID의 곡을 삭제합니다.
    """
    db_song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
    if db_song is None:
        raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")
    
    db.delete(db_song)
    db.commit()
    return Response(status_code=status.HTTP_204_NO_CONTENT)

@router.get("/{song_id}/sheet")
def get_sheet_music(song_id: str, db: Session = Depends(get_db)):
    """
    특정 ID의 곡 악보 파일을 다운로드합니다.
    """
    db_song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
    if db_song is None:
        raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")
    
    # 경로 수정: 상대 경로 사용
    file_path = db_song.sheet_music
    file_path = os.path.join("app", file_path)
    
    if os.path.exists(file_path):
        return FileResponse(file_path, filename=Path(file_path).name)
    else:
        raise HTTPException(status_code=404, detail=f"악보 파일을 찾을 수 없습니다: {file_path}")

@router.get("/{song_id}/audio")
def get_audio(song_id: str, db: Session = Depends(get_db)):
    """
    특정 ID의 곡 음원 파일을 다운로드/스트리밍합니다.
    """
    db_song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
    if db_song is None:
        raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")
    
    # 경로 수정: 상대 경로 사용
    file_path = db_song.audio
    file_path = os.path.join("app", file_path)
    
    if os.path.exists(file_path):
        return FileResponse(
            file_path, 
            filename=Path(file_path).name,
            media_type="audio/mpeg"
        )
    else:
        raise HTTPException(status_code=404, detail=f"음원 파일을 찾을 수 없습니다: {file_path}")

@router.get("/{song_id}/thumbnail")
def get_thumbnail(song_id: str, db: Session = Depends(get_db)):
    """
    특정 ID의 곡 앨범 커버 이미지를 다운로드합니다.
    """
    db_song = db.query(SongModel).filter(SongModel.song_id == song_id).first()
    if db_song is None:
        raise HTTPException(status_code=404, detail="노래를 찾을 수 없습니다")
    
    # 경로 수정: 상대 경로 사용
    file_path = db_song.thumbnail
    file_path = os.path.join("app", file_path)
    
    if os.path.exists(file_path):
        return FileResponse(
            file_path, 
            filename=Path(file_path).name,
            media_type="image/jpeg"
        )
    else:
        raise HTTPException(status_code=404, detail=f"앨범 커버 이미지를 찾을 수 없습니다: {file_path}") 