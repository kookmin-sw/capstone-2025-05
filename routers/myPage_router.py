from fastapi import APIRouter, HTTPException, UploadFile, File, Body
from fastapi.responses import StreamingResponse
from firebase_admin import auth, db, storage
from typing import List
from io import BytesIO
from manager.firebase_manager import firestore_db, storage_bucket
import uuid
import datetime
from datetime import timedelta

router = APIRouter()

@router.get("/get-user-info", tags=["My Page"])
async def get_user_info(uid: str):
    try:
        auth.get_user(uid)

        user_ref = db.reference(f"/users/{uid}")
        user_data = user_ref.get()

        if not user_data:
            print("사용자 정보 존재 X")
            raise HTTPException(status_code=404, detail="해당 사용자 정보가 존재하지 않습니다.")

        return {"user information": user_data}

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=404, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("사용자 정보 조회 실패")
        raise HTTPException(status_code=500, detail=f"사용자 정보 조회 실패: {str(e)}")

@router.put("/edit-user/nickname", tags=["My Page"])
async def edit_nickname(uid: str, nickname: str):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")
        if not user_ref.get():
            print("해당 사용자가 존재하지 않음")
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
        user_ref.update({"nickname": nickname})

        activity_ref = firestore_db.collection("my_activity").document(uid)

        for activity_type in ["post", "comment"]:
            subcollection = activity_ref.collection(activity_type)
            docs = subcollection.stream()

            for doc in docs:
                post_id = doc.id
                target_collection = firestore_db.collection(activity_type).document(post_id)
                if target_collection.get().exists:
                    target_collection.update({"작성자": nickname})

        blobs = list(storage_bucket.list_blobs(prefix=f"{uid}/record/"))

        updated_songs = set()
        for blob in blobs:
            parts = blob.name.split("/")
            if len(parts) >= 3:
                song_name = parts[2]
                updated_songs.add(song_name)

        for song_name in updated_songs:
            rank_ref = db.reference(f"/rank/{song_name}/{uid}")
            rank_data = rank_ref.get()
            if rank_data:
                rank_ref.update({"nickname": nickname})

        print("닉네임 변경 완료")
        return {"message": "닉네임 변경 완료"}

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("닉네임 변경 불확실한 이유로 실패")
        raise HTTPException(status_code=500, detail=f"닉네임 변경 실패: {str(e)}")

@router.put("/edit-user/interest-genre", tags=["My Page"])
async def edit_interest_genre(uid: str, interest_genre: List[int]):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")
        
        if not user_ref.get():
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
        
        user_ref.update({"interest_genre": interest_genre})
        print("관심 장르 변경 완료")
        return { "message": "관심 장르 변경 완료" }

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("원인불명 관심 장르 변경 실패")
        raise HTTPException(status_code=500, detail=f"관심 장르 변경 실패: {str(e)}")

@router.put("/edit-user/level", tags=["My Page"])
async def edit_level(uid: str, level: int):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")
        
        if not user_ref.get():
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
        
        user_ref.update({"level": level})
        print("실력 변경 완료")
        return { "message": "실력 변경 완료" }

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("원인불명 실력 변경 실패")
        raise HTTPException(status_code=500, detail=f"실력 변경 실패: {str(e)}")

@router.post("/change-profile-image", tags=["My Page"])
async def change_profile_image(uid: str, file: UploadFile = File(...)):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")

        storage_bucket = storage.bucket()
        blobs = storage_bucket.list_blobs(prefix=f"{uid}/profile/")

        deleted_files: List[str] = []
        for blob in blobs:
            blob.delete()
            deleted_files.append(blob.name)
        
        if deleted_files:
            print("기존 프로필 삭제 완료")
        else:
            print("삭제할 기존 프로필 사진이 없습니다.")

        file_extension = file.filename.split(".")[-1].lower()
        if file_extension not in ["jpg", "jpeg", "png"]:
            print("지원하지 않는 파일 형식 (jpg, jpeg, png만 가능)")
            raise HTTPException(status_code=400, detail="지원하지 않는 파일 형식입니다. (jpg, jpeg, png만 가능)")

        unique_filename = f"{uuid.uuid4()}.{file_extension}"
        storage_path = f"{uid}/profile/{unique_filename}"

        blob = storage_bucket.blob(storage_path)
        blob.upload_from_file(file.file, content_type=file.content_type)

        new_profile_url = blob.public_url
        user_ref.update({"profile_image": new_profile_url})

        print("프로필 사진 변경 완료")
        return {"message": "프로필 사진 변경 완료", "profile_image_url": new_profile_url}

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print(f"원인 불명 프로필 사진 변경 실패: {str(e)}")
        raise HTTPException(status_code=500, detail=f"프로필 사진 변경 실패: {str(e)}")

def get_score_data(uid: str, song_name: str, upload_count: int):
    try:
        doc_ref = firestore_db.collection(f"{uid}_score").document(song_name).collection(str(upload_count)).document("score")
        score_data = doc_ref.get()

        if not score_data.exists:
            print("연습 기록이 존재하지 않음")
            return None

        return score_data.to_dict()
    except Exception as e:
        print(f"연습 기록 조회 실패: {str(e)}")
        return None
    
def extract_date(date):
    if hasattr(date, "strftime"):
        return date.strftime("%Y-%m-%d")
    elif isinstance(date, str) and 'T' in date:
        return date.split('T')[0]
    return date

@router.get("/records/all", tags=["My Page"])
async def get_all_records(uid: str):
    try:
        storage_bucket = storage.bucket()
        blobs = list(storage_bucket.list_blobs(prefix=f"{uid}/record/"))

        records = {}

        for blob in blobs:
            path_parts = blob.name.split("/")

            if len(path_parts) < 5 or not blob.name.endswith(('.mp3', '.wav')):
                continue

            song_name = path_parts[2]
            upload_count = int(path_parts[3])

            score_data = get_score_data(uid, song_name, upload_count)

            if score_data:
                if song_name not in records:
                    records[song_name] = []

                if not any(record["upload_count"] == upload_count and record["audio_url"] == blob.public_url 
                           for record in records[song_name]):
                    date = score_data.get("date")

                    hasattr(date, "strftime")
                    date = date.strftime("%Y-%m-%d") 

                    records[song_name].append({
                        "upload_count": upload_count,
                        "tempo": score_data.get("tempo"),
                        "beat": score_data.get("beat"),
                        "interval": score_data.get("interval"),
                        "date": date
                    })

        return {"records": records}

    except Exception as e:
        print(f"연습 기록 조회 실패: {str(e)}")
        raise HTTPException(status_code=500, detail="연습 기록 조회에 실패했습니다.")

@router.get("/records/specific", tags=["My Page"])
async def get_specific_record(uid: str, song_name: str, upload_count: int):
    """특정 연습 기록 조회"""
    try:
        if upload_count == 0:  # 모든 연습 내역 조회
            blob_path = f"{uid}/record/{song_name}/"
            blobs = list(storage_bucket.list_blobs(prefix=blob_path))
            upload_counts = set(blob.name.split("/")[3] for blob in blobs if blob.name.endswith(('.mp3', '.wav')))

            records = []
            for count in upload_counts:
                score_data = get_score_data(uid, song_name, int(count))
                if score_data:
                    records.append({
                        "upload_count": int(count),
                        "tempo": score_data.get("tempo"),
                        "beat": score_data.get("beat"),
                        "interval": score_data.get("interval"),
                        "date": extract_date(score_data.get("date"))
                    })

            if not records:
                raise HTTPException(status_code=404, detail="해당 연습 기록이 없습니다.")

            return records

        else:
            score_data = get_score_data(uid, song_name, upload_count)
            if not score_data:
                raise HTTPException(status_code=404, detail="해당 연습 기록이 없습니다.")

            record_data = {
                "upload_count": upload_count,
                "tempo": score_data.get("tempo"),
                "beat": score_data.get("beat"),
                "interval": score_data.get("interval"),
                "date": extract_date(score_data.get("date"))
            }

            return record_data

    except Exception as e:
        print(f"특정 연습 기록 조회 실패: {str(e)}")
        raise HTTPException(status_code=500, detail="특정 연습 기록 조회에 실패했습니다.")


@router.get("/records/audio", tags=["My Page"])
async def get_record_audio(uid: str, song_name: str, upload_count: int):
    try:
        blob_path = f"{uid}/record/{song_name}/{upload_count}/"
        blobs = list(storage_bucket.list_blobs(prefix=blob_path))

        audio_files = [blob for blob in blobs if blob.name.endswith(('.mp3', '.wav'))]

        if not audio_files:
            raise HTTPException(status_code=404, detail="해당 연습 파일이 없습니다.")

        target_blob = audio_files[0]
        audio_stream = BytesIO(target_blob.download_as_bytes())
        audio_filename = target_blob.name.split("/")[-1]

        headers = {
            "Content-Disposition": f"attachment; filename={audio_filename}"
        }

        return StreamingResponse(audio_stream, headers=headers, media_type="audio/mpeg")

    except Exception as e:
        print("숫자 입력 잘못해서 음원 가져오기 실패")
        raise HTTPException(status_code=500, detail="업로드한 음원 가져오기 실패")
    
def get_posts_by_activity(uid: str, activity_type: str):
    try:
        activity_ref = firestore_db.collection("my_activity").document(uid).collection(activity_type)
        activity_docs = activity_ref.stream()
        
        post_ids = [doc.id for doc in activity_docs]
        if not post_ids:
            return []
        
        posts = []
        for post_id in post_ids:
            post_doc = firestore_db.collection("post").document(post_id).get()
            if post_doc.exists:
                posts.append({
                    "post_id": post_id,
                    "title": post_doc.to_dict().get("title", "")
                })
        return posts
    
    except Exception as e:
        print(f"{activity_type} 데이터 조회 실패: {str(e)}")
        raise HTTPException(status_code=500, detail=f"{activity_type} 데이터 조회 실패")

@router.get("/my-posts", tags=["My Page"])
async def get_my_posts(uid: str):
    return {"my_posts": get_posts_by_activity(uid, "post")}

@router.get("/my-scraps", tags=["My Page"])
async def get_my_scraps(uid: str):
    return {"my_scraps": get_posts_by_activity(uid, "scrap")}

@router.get("/my-likes", tags=["My Page"])
async def get_my_likes(uid: str):
    return {"my_likes": get_posts_by_activity(uid, "like")}

@router.get("/my-specific-song-rank", tags=["My Page"])
async def get_my_rank(uid: str, song_name: str):
    try:
        doc_ref = firestore_db.collection(f"{uid}_score").document(song_name)
        doc = doc_ref.get()

        if not doc.exists:
            print("해당 곡의 랭킹 정보가 없음")
            raise HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")

        data = doc.to_dict()
        rank = data.get("rank")

        if rank is None:
            print("연습 기록을 안 올려서 랭킹 정보가 없음")
            raise HTTPException(status_code=404, detail="랭킹 정보가 존재하지 않습니다.")

        return {"rank": rank}

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"랭킹 정보 조회 실패: {str(e)}")
    
@router.get("/recent-4-record", tags=["My Page"])
async def get_recent_album_covers(uid: str):
    try:
        user_collection = firestore_db.collection(f"{uid}_score")
        all_docs = user_collection.stream()

        uploads = []
        for doc in all_docs:
            song_name = doc.id
            count_subcol = user_collection.document(song_name).collections()
            for subcol in count_subcol:
                for doc2 in subcol.stream():
                    if doc2.id == "score":
                        data = doc2.to_dict()
                        date = extract_date(data.get("date"))
                        uploads.append({
                            "song_name": song_name,
                            "date": date
                        })

        sorted_uploads = sorted(uploads, key=lambda x: x["date"], reverse=True)[:4]

        album_covers = []
        for upload in sorted_uploads:
            song_name = upload["song_name"]
            blob = storage.bucket().blob(f"album_covers/{song_name}.jpg")
            url = blob.generate_signed_url(datetime.timedelta(minutes=60)) if blob.exists() else None
            album_covers.append({
                "song_name": song_name,
                "cover_url": url
            })

        return {"recent_uploads": album_covers}

    except Exception as e:
        print("앨범 커버 조회 실패")
        raise HTTPException(status_code=500, detail=f"앨범 커버 조회 실패: {str(e)}")