import uuid
from sqlalchemy.orm import Session
from app.database import SessionLocal, engine, Base
from app.models.song import SongModel

# 테스트용 더미 데이터 생성
def create_test_data(db: Session):
    # DB에 데이터가 있는지 확인
    if db.query(SongModel).count() > 0:
        print("이미 데이터가 존재합니다.")
        return False
        
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
    
    print(f"{len(test_songs)}개의 테스트 데이터를 생성합니다...")
    
    for song_data in test_songs:
        db_song = SongModel(**song_data)
        db.add(db_song)
    
    db.commit()
    print("테스트 데이터가 성공적으로 생성되었습니다.")
    return True

def init_db():
    # 데이터베이스 테이블 생성
    Base.metadata.create_all(bind=engine)
    print("데이터베이스 테이블이 생성되었습니다.")
    
    # 세션 생성 및 테스트 데이터 삽입
    db = SessionLocal()
    try:
        result = create_test_data(db)
        if result:
            print("데이터베이스 초기화가 완료되었습니다.")
        else:
            print("이미 초기화된 데이터베이스입니다.")
    finally:
        db.close()

if __name__ == "__main__":
    init_db() 