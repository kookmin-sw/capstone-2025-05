import os
from typing import Dict, List, Optional, Any
import uuid
from io import BytesIO
from firebase_admin import auth
from fastapi import HTTPException, UploadFile
from fastapi.responses import StreamingResponse

from manager.firebase_manager import firestore_db, realtime_db, storage_bucket

class MyPageService:
    @staticmethod
    async def edit_nickname(uid: str, nickname: str) -> Dict[str, str]:
        try:
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            user_data = user_ref.get()
            if not user_data:
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
                rank_ref = realtime_db.child(f"rank/{song_name}/{uid}")
                rank_data = rank_ref.get()
                if rank_data:
                    rank_ref.update({"nickname": nickname})

            return {"message": "닉네임 변경 완료"}
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"닉네임 변경 실패: {str(e)}")

    @staticmethod
    async def edit_interest_genre(uid: str, interest_genre: List[int]) -> Dict[str, str]:
        try:
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            user_data = user_ref.get()
            if not user_data:
                raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
            user_ref.update({"interest_genre": interest_genre})
            return {"message": "관심 장르 변경 완료"}
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"관심 장르 변경 실패: {str(e)}")

    @staticmethod
    async def edit_level(uid: str, level: int) -> Dict[str, str]:
        try:
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            user_data = user_ref.get()
            if not user_data:
                raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
            user_ref.update({"level": level})
            return {"message": "실력 변경 완료"}
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"실력 변경 실패: {str(e)}")

    @staticmethod
    async def change_profile_image(uid: str, file: UploadFile) -> Dict[str, str]:
        try:
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            blobs = storage_bucket.list_blobs(prefix=f"{uid}/profile/")
            for blob in blobs:
                blob.delete()

            file_extension = file.filename.split(".")[-1].lower()
            if file_extension not in ["jpg", "jpeg", "png"]:
                raise HTTPException(status_code=400, detail="지원하지 않는 파일 형식입니다. (jpg, jpeg, png만 가능)")

            unique_filename = f"{uuid.uuid4()}.{file_extension}"
            storage_path = f"{uid}/profile/{unique_filename}"
            blob = storage_bucket.blob(storage_path)
            blob.upload_from_file(file.file, content_type=file.content_type)
            new_profile_url = blob.public_url
            user_ref.update({"profile_image": new_profile_url})

            return {"message": "프로필 사진 변경 완료", "profile_image_url": new_profile_url}
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"프로필 사진 변경 실패: {str(e)}")

    @staticmethod
    def _get_score_data(uid: str, song_name: str, upload_count: int) -> Optional[Dict[str, Any]]:
        try:
            doc_ref = firestore_db.collection(f"{uid}_score").document(song_name).collection(str(upload_count)).document("score")
            score_data = doc_ref.get()
            if not score_data.exists:
                return None
            return score_data.to_dict()
        except Exception:
            return None

    @staticmethod
    def _extract_date(date: Any) -> str:
        if hasattr(date, "strftime"):
            return date.strftime("%Y-%m-%d")
        elif isinstance(date, str) and 'T' in date:
            return date.split('T')[0]
        return str(date)

    @staticmethod
    async def get_all_records(uid: str) -> Dict[str, Dict[str, List[Dict[str, Any]]]]:
        try:
            blobs = list(storage_bucket.list_blobs(prefix=f"{uid}/record/"))
            records = {}
            for blob in blobs:
                path_parts = blob.name.split("/")
                if len(path_parts) < 5 or not blob.name.endswith(('.mp3', '.wav')):
                    continue
                song_name = path_parts[2]
                upload_count = int(path_parts[3])
                score_data = MyPageService._get_score_data(uid, song_name, upload_count)
                if score_data:
                    if song_name not in records:
                        records[song_name] = []
                    if not any(record["upload_count"] == upload_count for record in records[song_name]):
                        date = score_data.get("date")
                        formatted_date = MyPageService._extract_date(date)
                        records[song_name].append({
                            "upload_count": upload_count,
                            "tempo": score_data.get("tempo"),
                            "beat": score_data.get("beat"),
                            "interval": score_data.get("interval"),
                            "date": formatted_date,
                            "audio_url": blob.public_url
                        })
            return {"records": records}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"연습 기록 조회 실패: {str(e)}")

    @staticmethod
    async def get_specific_record(uid: str, song_name: str, upload_count: int) -> Dict[str, Any]:
        try:
            if upload_count == 0:
                blob_path = f"{uid}/record/{song_name}/"
                blobs = list(storage_bucket.list_blobs(prefix=blob_path))
                upload_counts = set(int(blob.name.split("/")[3]) for blob in blobs if blob.name.endswith(('.mp3', '.wav')))
                records = []
                for count in upload_counts:
                    score_data = MyPageService._get_score_data(uid, song_name, count)
                    if score_data:
                        records.append({
                            "upload_count": count,
                            "tempo": score_data.get("tempo"),
                            "beat": score_data.get("beat"),
                            "interval": score_data.get("interval"),
                            "date": MyPageService._extract_date(score_data.get("date"))
                        })
                if not records:
                    raise HTTPException(status_code=404, detail="해당 연습 기록이 없습니다.")
                return {"records": records}
            else:
                score_data = MyPageService._get_score_data(uid, song_name, upload_count)
                if not score_data:
                    raise HTTPException(status_code=404, detail="해당 연습 기록이 없습니다.")
                record_data = {
                    "upload_count": upload_count,
                    "tempo": score_data.get("tempo"),
                    "beat": score_data.get("beat"),
                    "interval": score_data.get("interval"),
                    "date": MyPageService._extract_date(score_data.get("date"))
                }
                return record_data
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"특정 연습 기록 조회 실패: {str(e)}")

    @staticmethod
    async def get_record_audio(uid: str, song_name: str, upload_count: int) -> StreamingResponse:
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
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"업로드한 음원 가져오기 실패: {str(e)}")

    @staticmethod
    def _get_posts_by_activity(uid: str, activity_type: str) -> List[Dict[str, str]]:
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
                    post_data = post_doc.to_dict()
                    posts.append({
                        "post_id": post_id,
                        "title": post_data.get("title", "")
                    })
            return posts
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"{activity_type} 데이터 조회 실패: {str(e)}")

    @staticmethod
    async def get_my_posts(uid: str) -> Dict[str, List[Dict[str, str]]]:
        posts = MyPageService._get_posts_by_activity(uid, "post")
        return {"my_posts": posts}

    @staticmethod
    async def get_my_scraps(uid: str) -> Dict[str, List[Dict[str, str]]]:
        scraps = MyPageService._get_posts_by_activity(uid, "scrap")
        return {"my_scraps": scraps}

    @staticmethod
    async def get_my_likes(uid: str) -> Dict[str, List[Dict[str, str]]]:
        likes = MyPageService._get_posts_by_activity(uid, "like")
        return {"my_likes": likes}

    @staticmethod
    async def get_my_rank(uid: str, song_name: str) -> Dict[str, Any]:
        try:
            doc_ref = firestore_db.collection(f"{uid}_score").document(song_name)
            doc = doc_ref.get()
            if not doc.exists:
                raise HTTPException(status_code=404, detail="해당 곡의 랭킹 정보가 없습니다.")
            data = doc.to_dict()
            rank = data.get("rank")
            if rank is None:
                raise HTTPException(status_code=404, detail="랭킹 정보가 존재하지 않습니다.")
            return {"rank": rank}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"랭킹 정보 조회 실패: {str(e)}")