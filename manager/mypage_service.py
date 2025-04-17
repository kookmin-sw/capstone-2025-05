import os
from typing import Dict, List, Optional, Any
import uuid
from io import BytesIO
from datetime import datetime
from firebase_admin import auth
from fastapi import HTTPException, UploadFile
from fastapi.responses import StreamingResponse
from pydantic import BaseModel

from manager.firebase_manager import firestore_db, realtime_db, storage_bucket

class MyPageService:
    """마이페이지 관련 기능을 처리하는 서비스 클래스"""
    
    @staticmethod
    async def edit_nickname(uid: str, nickname: str) -> Dict[str, str]:
        """사용자 닉네임 변경"""
        try:
            # 사용자 존재 확인
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            
            # 사용자 데이터 확인
            user_data = user_ref.get()
            if not user_data:
                raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
            
            # 닉네임 업데이트
            user_ref.update({"nickname": nickname})
            return {"message": "닉네임 변경 완료"}
            
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"닉네임 변경 실패: {str(e)}")
    
    @staticmethod
    async def edit_interest_genre(uid: str, interest_genre: List[int]) -> Dict[str, str]:
        """사용자 관심 장르 변경"""
        try:
            # 사용자 존재 확인
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            
            # 사용자 데이터 확인
            user_data = user_ref.get()
            if not user_data:
                raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
            
            # 관심 장르 업데이트
            user_ref.update({"interest_genre": interest_genre})
            return {"message": "관심 장르 변경 완료"}
            
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"관심 장르 변경 실패: {str(e)}")
    
    @staticmethod
    async def edit_level(uid: str, level: int) -> Dict[str, str]:
        """사용자 실력 레벨 변경"""
        try:
            # 사용자 존재 확인
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            
            # 사용자 데이터 확인
            user_data = user_ref.get()
            if not user_data:
                raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
            
            # 레벨 업데이트
            user_ref.update({"level": level})
            return {"message": "실력 변경 완료"}
            
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"실력 변경 실패: {str(e)}")
    
    @staticmethod
    async def change_profile_image(uid: str, file: UploadFile) -> Dict[str, str]:
        """프로필 이미지 변경"""
        try:
            # 사용자 존재 확인
            auth.get_user(uid)
            user_ref = realtime_db.child(f"users/{uid}")
            
            # 기존 프로필 이미지 삭제
            blobs = storage_bucket.list_blobs(prefix=f"{uid}/profile/")
            for blob in blobs:
                blob.delete()
            
            # 파일 형식 확인
            file_extension = file.filename.split(".")[-1].lower()
            if file_extension not in ["jpg", "jpeg", "png"]:
                raise HTTPException(status_code=400, detail="지원하지 않는 파일 형식입니다. (jpg, jpeg, png만 가능)")
            
            # 새 프로필 이미지 업로드
            unique_filename = f"{uuid.uuid4()}.{file_extension}"
            storage_path = f"{uid}/profile/{unique_filename}"
            
            blob = storage_bucket.blob(storage_path)
            blob.upload_from_file(file.file, content_type=file.content_type)
            
            # 프로필 이미지 URL 업데이트
            new_profile_url = blob.public_url
            user_ref.update({"profile_image": new_profile_url})
            
            return {
                "message": "프로필 사진 변경 완료", 
                "profile_image_url": new_profile_url
            }
            
        except Exception as e:
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            raise HTTPException(status_code=500, detail=f"프로필 사진 변경 실패: {str(e)}")
    
    @staticmethod
    def _get_score_data(uid: str, song_name: str, upload_count: int) -> Optional[Dict[str, Any]]:
        """특정 연습 기록의 점수 데이터 조회"""
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
        """날짜 데이터 추출 및 형식화"""
        if hasattr(date, "strftime"):
            return date.strftime("%Y-%m-%d")
        elif isinstance(date, str) and 'T' in date:
            return date.split('T')[0]
        return str(date)
    
    @staticmethod
    async def get_all_records(uid: str) -> Dict[str, Dict[str, List[Dict[str, Any]]]]:
        """모든 연습 기록 조회"""
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
        """특정 연습 기록 조회"""
        try:
            if upload_count == 0:  # 모든 연습 내역 조회
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
        """연습 녹음 오디오 파일 조회"""
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
        """특정 활동(게시글, 스크랩, 좋아요)에 따른 게시글 목록 조회"""
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
                        "title": post_data.get("제목", "") if post_data else ""
                    })
            return posts
            
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"{activity_type} 데이터 조회 실패: {str(e)}")
    
    @staticmethod
    async def get_my_posts(uid: str) -> Dict[str, List[Dict[str, str]]]:
        """내가 작성한 게시글 목록 조회"""
        posts = MyPageService._get_posts_by_activity(uid, "post")
        return {"my_posts": posts}
    
    @staticmethod
    async def get_my_scraps(uid: str) -> Dict[str, List[Dict[str, str]]]:
        """내가 스크랩한 게시글 목록 조회"""
        scraps = MyPageService._get_posts_by_activity(uid, "scrap")
        return {"my_scraps": scraps}
    
    @staticmethod
    async def get_my_likes(uid: str) -> Dict[str, List[Dict[str, str]]]:
        """내가 좋아요한 게시글 목록 조회"""
        likes = MyPageService._get_posts_by_activity(uid, "like")
        return {"my_likes": likes}