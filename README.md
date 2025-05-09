# Maple Media Server

FastAPI를 사용한 음악 스트리밍 및 데이터 관리 API 서버입니다. 이 서버는 음악 데이터를 제공하고, 악보 및 오디오 파일을 스트리밍하는 엔드포인트를 제공합니다.

## 프로젝트 소개

Maple Media Server는 음악 정보, 악보, 오디오 파일을 관리하고 제공하는 API 서버입니다. 이 프로젝트는 국민대학교 캡스톤 디자인 2025-05 팀의 일환으로 개발되었습니다.

### 주요 기능

- 음악 목록 조회 및 관리
- 악보 파일 제공
- 오디오 파일 스트리밍
- 앨범 커버 이미지 제공

## 설치 방법

### 직접 실행

```bash
# 가상환경 생성 (선택사항)
python -m venv venv
source venv/bin/activate  # Linux/Mac
# 또는
venv\Scripts\activate  # Windows

# 의존성 패키지 설치
pip install -r requirements.txt
```

자세한 Docker 사용법은 [DOCKER.md](DOCKER.md) 문서를 참조하세요.

## 실행 방법

### 직접 실행

```bash
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

### Docker 로그 확인

```bash
docker-compose logs -f
```

## API 엔드포인트

| 메서드 | 경로 | 설명 |
|--------|------|------|
| GET | `/songs` | 모든 곡 목록 조회 |
| GET | `/songs/{song_id}` | 특정 곡 상세 정보 조회 |
| POST | `/songs` | 새로운 곡 데이터 추가 |
| PUT | `/songs/{song_id}` | 곡 정보 수정 |
| DELETE | `/songs/{song_id}` | 곡 삭제 |
| GET | `/songs/{song_id}/sheet` | 악보 파일 다운로드 |
| GET | `/songs/{song_id}/audio` | 음원 파일 다운로드/스트리밍 |
| GET | `/songs/{song_id}/thumbnail` | 앨범 커버 이미지 다운로드 |

자세한 API 사용법은 [API_DOCUMENTATION.md](API_DOCUMENTATION.md) 문서를 참조하세요.

## API 문서

서버 실행 후 다음 URL에서 API 문서 확인 가능:
- Swagger UI: `http://localhost:8000/docs`
- ReDoc: `http://localhost:8000/redoc`

## 디렉토리 구조

```
maple-media-server/
├── app/                     # 애플리케이션 코드
│   ├── models/              # 데이터 모델
│   │   └── song.py          # 곡 모델 정의
│   ├── routers/             # API 라우터
│   │   └── songs.py         # 곡 관련 API 엔드포인트
│   └── static/              # 정적 파일
│       ├── audio/           # 음원 파일
│       ├── sheet_music/     # 악보 파일
│       └── thumbnails/      # 앨범 커버 이미지
├── main.py                  # 애플리케이션 진입점
├── requirements.txt         # 의존성 패키지 목록
├── Dockerfile               # Docker 이미지 빌드 설정
└── docker-compose.yml       # Docker Compose 설정
```