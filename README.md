# 음악 Mock API 서버

FastAPI를 사용한 음악 관련 Mock API 서버입니다. 이 서버는 음악 데이터를 제공하고 관리하는 API 엔드포인트를 제공합니다.

## 설치 방법

### 직접 실행
```bash
pip install -r requirements.txt
```

### Docker 실행 (권장)
```bash
# Docker Compose 사용
docker-compose up -d

# 또는 Docker만 사용
docker build -t maple-mock-api .
docker run -d -p 8000:8000 --name maple-mock-api maple-mock-api
```

자세한 Docker 사용법은 [DOCKER.md](DOCKER.md) 문서를 참조하세요.

## 실행 방법

### 직접 실행
```bash
uvicorn main:app --reload
```

### Docker 로그 확인
```bash
docker-compose logs -f
```

## API 엔드포인트

- `GET /songs`: 모든 곡 목록 조회
- `GET /songs/{song_id}`: 특정 곡 상세 정보 조회
- `POST /songs`: 새로운 곡 데이터 추가
- `PUT /songs/{song_id}`: 곡 정보 수정
- `DELETE /songs/{song_id}`: 곡 삭제
- `GET /songs/{song_id}/sheet`: 악보 파일 다운로드
- `GET /songs/{song_id}/audio`: 음원 파일 다운로드/스트리밍

## API 문서

서버 실행 후 다음 URL에서 API 문서 확인 가능:
- Swagger UI: `http://localhost:8000/docs`
- ReDoc: `http://localhost:8000/redoc` 