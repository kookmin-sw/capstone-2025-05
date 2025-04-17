from fastapi import APIRouter, HTTPException, UploadFile, File
from fastapi.responses import StreamingResponse
from firebase_admin import auth, db, storage
from typing import List, Dict, Any, Optional
from io import BytesIO
from manager.firebase_manager import firestore_db, storage_bucket
import uuid
from models.response_models import StandardResponse
from manager.mypage_service import MyPageService

router = APIRouter()

@router.put("/edit-user/nickname", tags=["My Page"])
async def edit_nickname(uid: str, nickname: str):
    try:
        result = await MyPageService.edit_nickname(uid, nickname)
        return StandardResponse(
            success=True,
            message="닉네임 변경 완료",
            data=result
        )
    except HTTPException as e:
        print("닉네임 변경 불확실한 이유로 실패")
        raise e

@router.put("/edit-user/interest-genre", tags=["My Page"])
async def edit_interest_genre(uid: str, interest_genre: List[int]):
    try:
        result = await MyPageService.edit_interest_genre(uid, interest_genre)
        return StandardResponse(
            success=True,
            message="관심 장르 변경 완료",
            data=result
        )
    except HTTPException as e:
        print("원인불명 관심 장르 변경 실패")
        raise e

@router.put("/edit-user/level", tags=["My Page"])
async def edit_level(uid: str, level: int):
    try:
        result = await MyPageService.edit_level(uid, level)
        return StandardResponse(
            success=True,
            message="실력 변경 완료",
            data=result
        )
    except HTTPException as e:
        print("원인불명 실력 변경 실패")
        raise e

@router.post("/change-profile-image", tags=["My Page"])
async def change_profile_image(uid: str, file: UploadFile = File(...)):
    try:
        result = await MyPageService.change_profile_image(uid, file)
        return StandardResponse(
            success=True,
            message="프로필 사진 변경 완료",
            data={"profile_image_url": result["profile_image_url"]}
        )
    except HTTPException as e:
        print(f"원인 불명 프로필 사진 변경 실패: {str(e)}")
        raise e

@router.get("/records/all", tags=["My Page"])
async def get_all_records(uid: str):
    try:
        records = await MyPageService.get_all_records(uid)
        return StandardResponse(
            success=True,
            message="연습 기록 조회 성공",
            data=records
        )
    except HTTPException as e:
        print(f"연습 기록 조회 실패: {str(e)}")
        raise e

@router.get("/records/specific", tags=["My Page"])
async def get_specific_record(uid: str, song_name: str, upload_count: int):
    """특정 연습 기록 조회"""
    try:
        record = await MyPageService.get_specific_record(uid, song_name, upload_count)
        return StandardResponse(
            success=True,
            message="특정 연습 기록 조회 성공",
            data=record
        )
    except HTTPException as e:
        print(f"특정 연습 기록 조회 실패: {str(e)}")
        raise e

@router.get("/records/audio", tags=["My Page"])
async def get_record_audio(uid: str, song_name: str, upload_count: int):
    try:
        return await MyPageService.get_record_audio(uid, song_name, upload_count)
    except HTTPException as e:
        print("숫자 입력 잘못해서 음원 가져오기 실패")
        raise e

@router.get("/my-posts", tags=["My Page"])
async def get_my_posts(uid: str):
    try:
        posts = await MyPageService.get_my_posts(uid)
        return StandardResponse(
            success=True,
            message="내 게시글 조회 성공",
            data=posts
        )
    except HTTPException as e:
        print(f"내 게시글 조회 실패: {str(e)}")
        raise e

@router.get("/my-scraps", tags=["My Page"])
async def get_my_scraps(uid: str):
    try:
        scraps = await MyPageService.get_my_scraps(uid)
        return StandardResponse(
            success=True,
            message="내 스크랩 조회 성공",
            data=scraps
        )
    except HTTPException as e:
        print(f"내 스크랩 조회 실패: {str(e)}")
        raise e

@router.get("/my-likes", tags=["My Page"])
async def get_my_likes(uid: str):
    try:
        likes = await MyPageService.get_my_likes(uid)
        return StandardResponse(
            success=True,
            message="내 좋아요 조회 성공",
            data=likes
        )
    except HTTPException as e:
        print(f"내 좋아요 조회 실패: {str(e)}")
        raise e

@router.get("/my-specific-song-rank", tags=["My Page"])
async def get_my_rank(uid: str, song_name: str):
    try:
        rank_info = await MyPageService.get_my_rank(uid, song_name)
        return StandardResponse(
            success=True,
            message="랭킹 정보 조회 성공",
            data=rank_info
        )
    except HTTPException as e:
        print(f"랭킹 정보 조회 실패: {str(e)}")
        raise e