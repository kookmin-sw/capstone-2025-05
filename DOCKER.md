# Docker를 사용한 음악 Mock API 서버 실행 가이드

이 문서는 Docker를 사용하여 음악 Mock API 서버를 실행하는 방법을 설명합니다.

## 사전 요구사항

- Docker 설치
- Docker Compose 설치

## 실행 방법

### 1. Docker Compose를 사용한 실행 (권장)

프로젝트 루트 디렉토리에서 다음 명령어를 실행합니다:

```bash
docker-compose up -d
```

이 명령어는 컨테이너를 백그라운드에서 실행합니다. 로그를 확인하려면:

```bash
docker-compose logs -f
```

### 2. Docker만 사용한 수동 실행

프로젝트 루트 디렉토리에서 다음 명령어를 실행합니다:

```bash
# 이미지 빌드
docker build -t maple-mock-api .

# 컨테이너 실행
docker run -d --name maple-mock-api -p 8000:8000 -v $(pwd)/app/static:/app/app/static maple-mock-api
```

## 컨테이너 관리

### 컨테이너 중지

```bash
docker-compose down
```

또는 Docker만 사용하는 경우:

```bash
docker stop maple-mock-api
docker rm maple-mock-api
```

### 로그 확인

```bash
docker-compose logs -f
```

또는 Docker만 사용하는 경우:

```bash
docker logs -f maple-mock-api
```

### 컨테이너 내부 접속

```bash
docker-compose exec maple-mock-api bash
```

또는 Docker만 사용하는 경우:

```bash
docker exec -it maple-mock-api bash
```

## 정적 파일 관리

Docker 컨테이너를 실행할 때 호스트의 `app/static` 디렉토리를 컨테이너의 `/app/app/static` 디렉토리에 마운트합니다. 
따라서 호스트에서 `app/static` 디렉토리에 파일을 추가하거나 수정하면 컨테이너에서도 즉시 반영됩니다.

## API 접근

컨테이너가 실행된 후, 다음 URL을 통해 API에 접근할 수 있습니다:

- API 기본 URL: http://localhost:8000
- API 문서: http://localhost:8000/docs
- API 대체 문서: http://localhost:8000/redoc 