from fastapi import APIRouter, HTTPException, UploadFile, File
from typing import List

from manager.mypage_service import MyPageService

router = APIRouter()

@router.put("/edit-user/nickname")
async def edit_nickname(uid: str, nickname: str):
    """사용자 닉네임 변경"""
    return await MyPageService.edit_nickname(uid, nickname)

@router.put("/edit-user/interest-genre")
async def edit_interest_genre(uid: str, interest_genre: List[int]):
    """사용자 관심 장르 변경"""
    return await MyPageService.edit_interest_genre(uid, interest_genre)

@router.put("/edit-user/level")
async def edit_level(uid: str, level: int):
    """사용자 실력 레벨 변경"""
    return await MyPageService.edit_level(uid, level)

@router.post("/change-profile-image")
async def change_profile_image(uid: str, file: UploadFile = File(...)):
    """프로필 이미지 변경"""
    return await MyPageService.change_profile_image(uid, file)

@router.get("/records/all")
async def get_all_records(uid: str):
    """모든 연습 기록 조회"""
    return await MyPageService.get_all_records(uid)

@router.get("/records/specific")
async def get_specific_record(uid: str, song_name: str, upload_count: int):
    """특정 연습 기록 조회"""
    return await MyPageService.get_specific_record(uid, song_name, upload_count)

@router.get("/records/audio")
async def get_record_audio(uid: str, song_name: str, upload_count: int):
    """연습 녹음 오디오 파일 조회"""
    return await MyPageService.get_record_audio(uid, song_name, upload_count)

@router.get("/my-posts")
async def get_my_posts(uid: str):
    """내가 작성한 게시글 목록 조회"""
    return await MyPageService.get_my_posts(uid)

@router.get("/my-scraps")
async def get_my_scraps(uid: str):
    """내가 스크랩한 게시글 목록 조회"""
    return await MyPageService.get_my_scraps(uid)

@router.get("/my-likes")
async def get_my_likes(uid: str):
    """내가 좋아요한 게시글 목록 조회"""
    return await MyPageService.get_my_likes(uid)