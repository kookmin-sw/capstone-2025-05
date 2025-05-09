# Maple Media Server API 설명서

이 문서는 Maple Media Server API의 사용 방법을 설명합니다.

## 기본 URL
```
http://[서버 도메인]/songs
```

## 엔드포인트

### 1. 모든 곡 목록 조회
- **URL**: `/`
- **메소드**: `GET`
- **응답**: 모든 곡의 정보가 담긴 배열

**응답 예시**:
```json
[
  {
    "song_id": "550e8400-e29b-41d4-a716-446655440000",
    "title": "26",
    "artist": "Younha",
    "thumbnail": "/static/thumbnails/26.jpg",
    "sheet_music": "/static/sheet_music/26.gp5",
    "audio": "/static/audio/26.wav",
    "bpm": 92,
    "duration": 29.0
  },
  ...
]
```

### 2. 특정 곡 정보 조회
- **URL**: `/{song_id}`
- **메소드**: `GET`
- **URL 파라미터**: `song_id` - 곡의 고유 식별자
- **성공 응답**: 해당 곡의 상세 정보
- **에러 응답**: 404 - 노래를 찾을 수 없음

**응답 예시**:
```json
{
  "song_id": "550e8400-e29b-41d4-a716-446655440000",
  "title": "26",
  "artist": "Younha",
  "thumbnail": "/static/thumbnails/26.jpg",
  "sheet_music": "/static/sheet_music/26.gp5",
  "audio": "/static/audio/26.wav",
  "bpm": 92,
  "duration": 29.0
}
```

### 3. 새로운 곡 추가
- **URL**: `/`
- **메소드**: `POST`
- **요청 본문**: 새 곡 정보 (song_id 제외)
- **성공 응답**: 201 Created와 생성된 곡 정보

**요청 예시**:
```json
{
  "title": "새로운 곡",
  "artist": "새 아티스트",
  "thumbnail": "/static/thumbnails/new.jpg",
  "sheet_music": "/static/sheet_music/new.gp5",
  "audio": "/static/audio/new.wav",
  "bpm": 120,
  "duration": 25.5
}
```

### 4. 곡 정보 수정
- **URL**: `/{song_id}`
- **메소드**: `PUT`
- **URL 파라미터**: `song_id` - 곡의 고유 식별자
- **요청 본문**: 수정할 곡 정보 (변경할 필드만 포함 가능)
- **성공 응답**: 수정된 곡 정보
- **에러 응답**: 404 - 노래를 찾을 수 없음

**요청 예시**:
```json
{
  "title": "수정된 제목",
  "bpm": 125
}
```

### 5. 곡 삭제
- **URL**: `/{song_id}`
- **메소드**: `DELETE`
- **URL 파라미터**: `song_id` - 곡의 고유 식별자
- **성공 응답**: 204 No Content
- **에러 응답**: 404 - 노래를 찾을 수 없음

### 6. 악보 파일 다운로드
- **URL**: `/{song_id}/sheet`
- **메소드**: `GET`
- **URL 파라미터**: `song_id` - 곡의 고유 식별자
- **성공 응답**: 악보 파일 다운로드
- **에러 응답**: 404 - 노래를 찾을 수 없거나 악보 파일이 없음

### 7. 음원 파일 다운로드/스트리밍
- **URL**: `/{song_id}/audio`
- **메소드**: `GET`
- **URL 파라미터**: `song_id` - 곡의 고유 식별자
- **성공 응답**: 음원 파일 다운로드 또는 스트리밍 (`audio/mpeg` 미디어 타입)
- **에러 응답**: 404 - 노래를 찾을 수 없거나 음원 파일이 없음

### 8. 앨범 커버 이미지 다운로드
- **URL**: `/{song_id}/thumbnail`
- **메소드**: `GET`
- **URL 파라미터**: `song_id` - 곡의 고유 식별자
- **성공 응답**: 앨범 커버 이미지 다운로드 (`image/jpeg` 미디어 타입)
- **에러 응답**: 404 - 노래를 찾을 수 없거나 이미지 파일이 없음

## 데이터 모델

### Song
- `song_id` (string): 곡 고유 식별자
- `title` (string): 곡 이름
- `artist` (string): 아티스트 이름
- `thumbnail` (string): 앨범 커버 썸네일 URL
- `sheet_music` (string): 악보 파일 URL
- `audio` (string): 원본 음원 URL
- `bpm` (integer, optional): 곡 템포 (30-300 사이)
- `duration` (float, optional): 음원 길이(초) 