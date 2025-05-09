from pydantic import BaseModel, Field, validator
from typing import Optional
import uuid
from sqlalchemy import Column, String, Integer, Float
from app.database import Base

# SQLAlchemy ORM 모델
class SongModel(Base):
    __tablename__ = "songs"

    song_id = Column(String, primary_key=True, index=True)
    title = Column(String(100), nullable=False)
    artist = Column(String(100), nullable=False)
    thumbnail = Column(String, nullable=False)
    sheet_music = Column(String, nullable=False)
    audio = Column(String, nullable=False)
    bpm = Column(Integer, nullable=True)
    duration = Column(Float, nullable=True)

# Pydantic 모델
class SongBase(BaseModel):
    title: str = Field(..., max_length=100, description="곡 이름")
    artist: str = Field(..., max_length=100, description="아티스트 이름")
    thumbnail: str = Field(..., description="앨범 커버 썸네일 URL")
    sheet_music: str = Field(..., description="악보 파일 URL")
    audio: str = Field(..., description="원본 음원 URL")
    bpm: Optional[int] = Field(None, ge=30, le=300, description="곡 템포")
    duration: Optional[float] = Field(None, gt=0, description="음원 길이(초)")

    @validator('bpm')
    def validate_bpm(cls, v):
        if v is not None and (v < 30 or v > 300):
            raise ValueError('BPM은 30에서 300 사이여야 합니다')
        return v

class SongCreate(SongBase):
    song_id: Optional[str] = Field(None, description="곡 고유 식별자 (제공하지 않으면 자동 생성됨)")

class SongUpdate(SongBase):
    title: Optional[str] = Field(None, max_length=100, description="곡 이름")
    artist: Optional[str] = Field(None, max_length=100, description="아티스트 이름")
    thumbnail: Optional[str] = Field(None, description="앨범 커버 썸네일 URL")
    sheet_music: Optional[str] = Field(None, description="악보 파일 URL")
    audio: Optional[str] = Field(None, description="원본 음원 URL")

class Song(SongBase):
    song_id: str = Field(..., description="곡 고유 식별자")

    class Config:
        orm_mode = True
        from_attributes = True 