import pytest
from fastapi.testclient import TestClient
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, Session
from sqlalchemy.pool import StaticPool
import os

from app.database import Base, get_db
from app.models.song import SongModel
from main import app

# 테스트 모드 설정
os.environ["TESTING"] = "True"

# 테스트용 인메모리 SQLite 데이터베이스 설정
TEST_SQLALCHEMY_DATABASE_URL = "sqlite:///:memory:"
engine = create_engine(
    TEST_SQLALCHEMY_DATABASE_URL,
    connect_args={"check_same_thread": False},
    poolclass=StaticPool,
)
TestingSessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

# 테스트용 의존성 함수 재정의
def override_get_db():
    db = TestingSessionLocal()
    try:
        yield db
    finally:
        db.close()

# 의존성 오버라이드 설정
app.dependency_overrides[get_db] = override_get_db

@pytest.fixture
def client():
    """테스트 클라이언트 생성"""
    # 기존 테이블 삭제 후 새로 생성 (있다면)
    Base.metadata.drop_all(bind=engine)
    Base.metadata.create_all(bind=engine)
    
    # 테스트 데이터 추가
    db = TestingSessionLocal()
    create_test_data(db)
    db.close()
    
    with TestClient(app) as test_client:
        yield test_client
    
    # 테스트 후 데이터베이스 정리
    Base.metadata.drop_all(bind=engine)

def create_test_data(db: Session):
    """테스트용 데이터 생성"""
    # 기존 데이터가 있는지 확인하고 있으면 삭제
    db.query(SongModel).delete()
    
    test_songs = [
        {
            "song_id": "test-song-1",
            "title": "테스트 곡 1",
            "artist": "테스트 아티스트",
            "thumbnail": "static/thumbnails/test1.jpg",
            "sheet_music": "static/sheet_music/test1.gp5",
            "audio": "static/audio/test1.wav",
            "bpm": 120,
            "duration": 30.0
        },
        {
            "song_id": "test-song-2",
            "title": "테스트 곡 2",
            "artist": "테스트 아티스트 2",
            "thumbnail": "static/thumbnails/test2.jpg",
            "sheet_music": "static/sheet_music/test2.gp5",
            "audio": "static/audio/test2.wav",
            "bpm": 100,
            "duration": 25.0
        }
    ]
    
    for song_data in test_songs:
        db_song = SongModel(**song_data)
        db.add(db_song)
    
    db.commit() 