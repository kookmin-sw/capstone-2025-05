from fastapi import APIRouter, HTTPException, Depends, Form, File, UploadFile
from starlette.requests import Request
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
from typing import List, Optional
import datetime
import os
from model import Post, Comment
from models.response_models import StandardResponse
from manager.post_service import PostService

router = APIRouter()

# 게시글 경로 설정
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
STATIC_DIR = os.path.join(BASE_DIR, 'static/')
IMG_DIR = os.path.join(STATIC_DIR, 'images/')
SERVER_IMG_DIR = 'http://localhost:8000/static/images/'
UPLOAD_IMAGE_DIR = "static/images"
UPLOAD_AUDIO_DIR = "static/audio"

os.makedirs(UPLOAD_IMAGE_DIR, exist_ok=True)
os.makedirs(UPLOAD_AUDIO_DIR, exist_ok=True)

# 전체 목록 가져오기
@router.get("/posts", response_model=StandardResponse, tags=["Post"])
async def get_all_posts():
    """모든 게시글 목록을 반환합니다."""
    try:
        posts = await PostService.get_all_posts()
        return StandardResponse(
            success=True,
            message="게시글 목록을 성공적으로 조회했습니다",
            data=posts
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 조회수 증가 기능
@router.put("/posts/{post_id}/views", response_model=StandardResponse, tags=["Post"])
async def increase_views(post_id: int):
    """게시글 조회수를 증가시킵니다."""
    try:
        post_data = await PostService.increase_view_count(post_id)
        return StandardResponse(
            success=True,
            message="조회수가 증가되었습니다",
            data=post_data
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 조회수 가장 높은 게시글 불러오기
@router.get("/posts/top-viewed", response_model=StandardResponse, tags=["Post"])
async def get_top_viewed_post():
    """조회수가 가장 높은 게시글을 반환합니다."""
    try:
        top_post = await PostService.get_top_viewed_post()
        return StandardResponse(
            success=True,
            message="인기 게시글을 성공적으로 조회했습니다",
            data=top_post
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 한 글 상세보기 페이지 (댓글 리스트도 표시)
@router.get("/posts/{post_id}", response_model=StandardResponse, tags=["Post"])
async def read_post(post_id: int):
    """ID로 게시글과 관련 댓글을 조회합니다."""
    try:
        post_data = await PostService.get_post_by_id(post_id)
        return StandardResponse(
            success=True,
            message="게시글을 성공적으로 조회했습니다",
            data=post_data
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 게시글 추가
@router.post("/posts", response_model=StandardResponse, tags=["Post"])
async def create_post(post: Post):
    """새 게시글을 생성합니다."""
    try:
        result = await PostService.create_post(post)
        return StandardResponse(
            success=True,
            message="게시글이 성공적으로 등록되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 게시글 수정
@router.put("/posts/{post_id}", response_model=StandardResponse, tags=["Post"])
async def update_post(post: Post, post_id: int):
    """게시글을 수정합니다."""
    try:
        result = await PostService.update_post(post_id, post)
        return StandardResponse(
            success=True,
            message="게시글이 성공적으로 수정되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 게시글 삭제
@router.delete("/posts/{post_id}", response_model=StandardResponse, tags=["Post"])
async def delete_post(post_id: int):
    """게시글과 관련 댓글을 삭제합니다."""
    try:
        result = await PostService.delete_post(post_id)
        return StandardResponse(
            success=True,
            message=f"게시글 및 관련 댓글이 삭제되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 댓글 추가
@router.post("/comments", response_model=StandardResponse, tags=["Comment"])
async def create_comment(comment: Comment):
    """게시글에 새 댓글을 생성합니다."""
    try:
        result = await PostService.create_comment(comment)
        return StandardResponse(
            success=True,
            message="댓글이 성공적으로 등록되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 댓글 수정
@router.put("/comments/{comment_id}", response_model=StandardResponse, tags=["Comment"])
async def update_comment(comment: Comment, comment_id: int):
    """댓글을 수정합니다."""
    try:
        result = await PostService.update_comment(comment_id, comment)
        return StandardResponse(
            success=True,
            message="댓글이 성공적으로 수정되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 댓글 삭제
@router.delete("/comments/{comment_id}", response_model=StandardResponse, tags=["Comment"])
async def delete_comment(comment_id: int):
    """댓글을 삭제합니다."""
    try:
        result = await PostService.delete_comment(comment_id)
        return StandardResponse(
            success=True,
            message="댓글이 성공적으로 삭제되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 게시글 좋아요 기능
@router.put("/posts/{post_id}/like", response_model=StandardResponse, tags=["Post"])
async def like_post(post_id: int):
    """게시글에 좋아요를 추가합니다."""
    try:
        result = await PostService.like_post(post_id)
        return StandardResponse(
            success=True,
            message="게시글에 좋아요가 추가되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 게시글 좋아요 취소
@router.put("/posts/{post_id}/unlike", response_model=StandardResponse, tags=["Post"])
async def unlike_post(post_id: int):
    """게시글의 좋아요를 취소합니다."""
    try:
        result = await PostService.unlike_post(post_id)
        return StandardResponse(
            success=True,
            message="게시글의 좋아요가 취소되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 게시글 신고 기능
@router.post("/posts/{post_id}/report", response_model=StandardResponse, tags=["Post"])
async def report_post(post_id: int, reason: str = Form(...)):
    """게시글을 신고합니다."""
    try:
        result = await PostService.report_post(post_id, reason)
        return StandardResponse(
            success=True,
            message="게시글이 신고되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")

# 댓글 신고 기능
@router.post("/comments/{comment_id}/report", response_model=StandardResponse, tags=["Comment"])
async def report_comment(comment_id: int, reason: str = Form(...)):
    """댓글을 신고합니다."""
    try:
        result = await PostService.report_comment(comment_id, reason)
        return StandardResponse(
            success=True,
            message="댓글이 신고되었습니다",
            data=result
        )
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"서버 오류: {str(e)}")