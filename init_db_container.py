import uuid
from sqlalchemy import create_engine, inspect
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, Session

# PostgreSQL 연결 설정
DB_USER = "postgres"
DB_PASSWORD = "postgres"
DB_HOST = "localhost"  # 호스트 머신에서 컨테이너로 포트포워딩된 주소
DB_PORT = "5432"
DB_NAME = "maple_media"

# 데이터베이스 연결 URL
SQLALCHEMY_DATABASE_URL = f"postgresql://{DB_USER}:{DB_PASSWORD}@{DB_HOST}:{DB_PORT}/{DB_NAME}"

# 엔진 및 세션 설정
engine = create_engine(SQLALCHEMY_DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

# Song 모델 정의 (임시로 여기에 정의)
from sqlalchemy import Column, String, Integer, Float

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

# 테스트 데이터 생성 함수
def create_test_data(db: Session):
    # 테이블이 비어있는지 확인
    row_count = db.query(SongModel).count()
    print(f"현재 songs 테이블의 행 수: {row_count}")
    
    # 데이터가 있더라도 강제로 전부 삭제하고 다시 생성
    if row_count > 0:
        print("기존 데이터를 모두 삭제합니다...")
        db.query(SongModel).delete()
        db.commit()
    
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
            "sheet_music": "static/sheet_music/happy.gp5",
            "audio": "static/audio/happy.wav",
            "bpm": 160,
            "duration": 35.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "태양물고기",
            "artist": "Younha",
            "thumbnail": "static/thumbnails/sunfish.jpg",
            "sheet_music": "static/sheet_music/sunfish.gp5",
            "audio": "static/audio/sunfish.wav",
            "bpm": 142,
            "duration": 25.7
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "오늘은 잠에 들 거예요",
            "artist": "YdBB",
            "thumbnail": "static/thumbnails/night-night.jpg",
            "sheet_music": "static/sheet_music/night-night.gp5",
            "audio": "static/audio/night-night.wav",
            "bpm": 120,
            "duration": 32.5
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Rebel Heart",
            "artist": "IVE",
            "thumbnail": "static/thumbnails/rebel-heart.jpg",
            "sheet_music": "static/sheet_music/rebel-heart.gp5",
            "audio": "static/audio/rebel-heart.wav",
            "bpm": 95,
            "duration": 40.2
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "No Pain",
            "artist": "Silicagel",
            "thumbnail": "static/thumbnails/no-pain.jpg",
            "sheet_music": "static/sheet_music/no-pain.gp5",
            "audio": "static/audio/no-pain.wav",
            "bpm": 128,
            "duration": 27.8
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Some Like It Hot",
            "artist": "SPYAIR",
            "thumbnail": "static/thumbnails/some-like-it-hot.jpg",
            "sheet_music": "static/sheet_music/some-like-it-hot.gp5",
            "audio": "static/audio/some-like-it-hot.wav",
            "bpm": 115,
            "duration": 33.0
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Pretender",
            "artist": "Official HIGE DANdism",
            "thumbnail": "static/thumbnails/pretender.jpg",
            "sheet_music": "static/sheet_music/pretender.gp5",
            "audio": "static/audio/pretender.wav",
            "bpm": 92,
            "duration": 38.5
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "사건의 지평선",
            "artist": "Younha",
            "thumbnail": "static/thumbnails/event-horizon.jpg",
            "sheet_music": "static/sheet_music/event-horizon.gp5",
            "audio": "static/audio/event-horizon.wav",
            "bpm": 150,
            "duration": 22.3
        },
        {
            "song_id": str(uuid.uuid4()),
            "title": "Canon Rock",
            "artist": "funtwo",
            "thumbnail": "static/thumbnails/canon-rock.jpg",
            "sheet_music": "static/sheet_music/canon-rock.gp5",
            "audio": "static/audio/canon-rock.wav",
            "bpm": 200,
            "duration": 20.0
        }
    ]
    
    print(f"{len(test_songs)}개의 테스트 데이터를 생성합니다...")
    
    for song_data in test_songs:
        db_song = SongModel(**song_data)
        db.add(db_song)
    
    db.commit()
    
    # 삽입 결과 확인
    final_count = db.query(SongModel).count()
    print(f"삽입 후 songs 테이블의 행 수: {final_count}")
    
    if final_count > 0:
        print("테스트 데이터가 성공적으로 생성되었습니다.")
        return True
    else:
        print("데이터 삽입에 실패했습니다!")
        return False

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
            print("데이터베이스 초기화에 실패했습니다.")
    except Exception as e:
        print(f"오류 발생: {e}")
    finally:
        db.close()

if __name__ == "__main__":
    print("Docker 컨테이너의 PostgreSQL에 직접 연결합니다...")
    init_db()
    print("작업이 완료되었습니다.") 