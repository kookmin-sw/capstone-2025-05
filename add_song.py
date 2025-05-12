import uuid
import os
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from init_db_container import SongModel, Base

# PostgreSQL 연결 설정
DB_USER = "postgres"
DB_PASSWORD = "postgres"
DB_HOST = "localhost"
DB_PORT = "5432"
DB_NAME = "maple_media"

# 데이터베이스 연결 URL
SQLALCHEMY_DATABASE_URL = f"postgresql://{DB_USER}:{DB_PASSWORD}@{DB_HOST}:{DB_PORT}/{DB_NAME}"

# 엔진 및 세션 설정
engine = create_engine(SQLALCHEMY_DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

def get_file_path(base_dir, filename, extension):
    """
    파일 경로를 생성합니다.
    
    Args:
        base_dir (str): 기본 디렉토리
        filename (str): 파일명
        extension (str): 파일 확장자
    
    Returns:
        str: 생성된 파일 경로
    """
    return f"static/{base_dir}/{filename}.{extension}"

def add_new_song(title, artist, filename, bpm=None, duration=None):
    """
    새로운 곡을 데이터베이스에 추가합니다.
    
    Args:
        title (str): 곡 제목
        artist (str): 아티스트 이름
        filename (str): 파일명 (확장자 제외)
        bpm (int, optional): BPM
        duration (float, optional): 재생 시간(초)
    """
    db = SessionLocal()
    try:
        # 파일 경로 생성
        thumbnail = get_file_path("thumbnails", filename, "jpg")
        sheet_music = get_file_path("sheet_music", filename, "gp5")
        audio = get_file_path("audio", filename, "wav")
        
        # 새로운 곡 데이터 생성
        new_song = SongModel(
            song_id=str(uuid.uuid4()),
            title=title,
            artist=artist,
            thumbnail=thumbnail,
            sheet_music=sheet_music,
            audio=audio,
            bpm=bpm,
            duration=duration
        )
        
        # 데이터베이스에 추가
        db.add(new_song)
        db.commit()
        print(f"'{title}' 곡이 성공적으로 추가되었습니다.")
        return True
    except Exception as e:
        db.rollback()
        print(f"오류 발생: {e}")
        return False
    finally:
        db.close()

def main():
    print("새로운 곡 추가")
    print("-" * 50)
    
    # 사용자로부터 곡 정보 입력 받기
    title = input("곡 제목: ").strip()
    artist = input("아티스트: ").strip()
    filename = input("파일명 (예: 26, fly, mela 등): ").strip()
    
    # BPM과 duration은 선택적으로 입력
    bpm_input = input("BPM (선택사항, Enter로 건너뛰기): ").strip()
    duration_input = input("재생 시간(초) (선택사항, Enter로 건너뛰기): ").strip()
    
    # 입력값 변환
    bpm = int(bpm_input) if bpm_input else None
    duration = float(duration_input) if duration_input else None
    
    # 필수 입력값 검증
    if not all([title, artist, filename]):
        print("필수 정보가 누락되었습니다.")
        return
    
    # 곡 추가
    add_new_song(title, artist, filename, bpm, duration)

if __name__ == "__main__":
    main() 