rom fastapi import APIRouter, UploadFile, File, Form, HTTPException
from fastapi.responses import StreamingResponse
from typing import Optional
from uuid import uuid4
import os

from manager.firebase_manager import firestore_db, storage_bucket

song_router = APIRouter(prefix="/songs", tags=["songs"])

async def upload_file_to_storage(file: UploadFile, folder:str) -> str:
    filename=f"{folder}/{uuid4()}_{file.filename}"
    blob = storage_bucket.blob(filename)
    blob.upload_from_file(file.file, content_type=file.content_type)
    return blob.public_url
async def validate_file_size(file: UploadFile, max_size_mb : int):
    file.file.seek(0, os.SEEK_END)
    size = file.file.tell()
    file.file.seek(0)
    if size > max_size_mb * 1024 * 1024:
        raise HTTPException(status_code=400, detail=f"File size excees {max_size_mb}MB.")

@song_router.get("/")
async def get_all_songs():
    songs = firestore_db.collection("songs").stream()
    return [
        {
            "song_id": song.id,
            "title": song.to_dict().get("title"),
            "artist": song.to_dict().get("artist"),
            "thumbnail": song.to_dict().get("thumbnail")
        }
        for song in songs
    ]

# 특정 곡 상세 조회
@song_router.get("/{song_id}")
async def get_song(song_id: str):
    song_ref = firestore_db.collection("songs").document(song_id)
    song = song_ref.get()
    if not song.exists:
        raise HTTPException(status_code=404, detail="Song not found")
    return song.to_dict()

# 새 곡 추가
@song_router.post("/")
async def create_song(
    title: str = Form(...),
    artist: str = Form(...),
    thumbnail: UploadFile = File(...),
    sheet_music: UploadFile = File(...),
    audio: UploadFile = File(...),
    bpm: Optional[int] = Form(None),
    duration: Optional[float] = Form(None),
):
    # 파일 포맷 & 사이즈 검증
    if thumbnail.content_type not in ["image/jpeg", "image/png"]:
        raise HTTPException(status_code=400, detail="Thumbnail must be jpg or png.")
    if sheet_music.filename.split('.')[-1] != "gp5":
        raise HTTPException(status_code=400, detail="Sheet music must be .gp5 file.")
    if audio.content_type != "audio/wav":
        raise HTTPException(status_code=400, detail="Audio must be .wav format.")

    await validate_file_size(thumbnail, 0.5)  # 500KB
    await validate_file_size(sheet_music, 10) # 10MB
    await validate_file_size(audio, 50)       # 50MB

    # 파일 업로드
    thumbnail_url = await upload_file_to_storage(thumbnail, "thumbnails")
    sheet_music_url = await upload_file_to_storage(sheet_music, "sheet_music")
    audio_url = await upload_file_to_storage(audio, "audio")

    # Firestore에 저장
    song_id = str(uuid4())
    firestore_db.collection("songs").document(song_id).set({
        "title": title,
        "artist": artist,
        "thumbnail": thumbnail_url,
        "sheet_music": sheet_music_url,
        "audio": audio_url,
        "bpm": bpm,
        "duration": duration
    })

    return {"message": "Song created successfully", "song_id": song_id}

# 곡 수정
@song_router.put("/{song_id}")
async def update_song(
    song_id: str,
    title: Optional[str] = Form(None),
    artist: Optional[str] = Form(None),
    bpm: Optional[int] = Form(None),
    duration: Optional[float] = Form(None),
):
    song_ref = firestore_db.collection("songs").document(song_id)
    if not song_ref.get().exists:
        raise HTTPException(status_code=404, detail="Song not found")

    update_data = {}
    if title is not None:
        update_data["title"] = title
    if artist is not None:
        update_data["artist"] = artist
    if bpm is not None:
        update_data["bpm"] = bpm
    if duration is not None:
        update_data["duration"] = duration

    song_ref.update(update_data)

    return {"message": "Song updated successfully"}

# 곡 삭제
@song_router.delete("/{song_id}")
async def delete_song(song_id: str):
    song_ref = firestore_db.collection("songs").document(song_id)
    if not song_ref.get().exists:
        raise HTTPException(status_code=404, detail="Song not found")

    song_ref.delete()
    return {"message": "Song deleted successfully"}
