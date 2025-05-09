# 음악 Mock API 서버 설치 및 실행 방법

## 1. 필수 패키지 설치

```bash
pip install -r requirements.txt
```

## 2. 서버 실행

```bash
uvicorn main:app --reload
```

또는 다음 명령어로도 실행 가능합니다:

```bash
python main.py
```

서버는 기본적으로 http://localhost:8000 에서 실행됩니다.

## 3. API 문서 확인

API 서버가 실행된 후에 다음 URL에서 자동 생성된 API 문서를 확인할 수 있습니다:

- Swagger UI: http://localhost:8000/docs
- ReDoc: http://localhost:8000/redoc

## 4. 정적 파일 관리

실제 서비스에서 사용할 음악 파일, 악보 파일, 썸네일 이미지를 다음 디렉토리에 저장하시면 됩니다:

- 썸네일 이미지: `app/static/thumbnails/`
- 악보 파일: `app/static/sheet_music/`
- 음원 파일: `app/static/audio/`

## 5. JUCE C++ 애플리케이션과의 통신

JUCE C++ 애플리케이션에서는 HTTP 클라이언트를 사용하여 다음과 같이 API와 통신할 수 있습니다:

### 예시 - cURL 명령어

#### 모든 곡 목록 조회
```bash
curl -X GET http://localhost:8000/songs
```

#### 특정 곡 조회
```bash
curl -X GET http://localhost:8000/songs/{song_id}
```

#### 새 곡 추가
```bash
curl -X POST http://localhost:8000/songs \
  -H "Content-Type: application/json" \
  -d '{
    "title": "새로운 곡",
    "artist": "아티스트 이름",
    "thumbnail": "/static/thumbnails/new_song.jpg",
    "sheet_music": "/static/sheet_music/new_song.pdf",
    "audio": "/static/audio/new_song.mp3",
    "bpm": 120,
    "duration": 180.5
  }'
```

#### 곡 정보 수정
```bash
curl -X PUT http://localhost:8000/songs/{song_id} \
  -H "Content-Type: application/json" \
  -d '{
    "title": "수정된 제목"
  }'
```

#### 곡 삭제
```bash
curl -X DELETE http://localhost:8000/songs/{song_id}
``` 