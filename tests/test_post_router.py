import pytest
import datetime
from unittest.mock import patch, MagicMock, AsyncMock
from fastapi.testclient import TestClient
from fastapi import FastAPI, Form
from routers.post_router import router as post_router
from model import Post, Comment
from models.response_models import StandardResponse

# 테스트용 FastAPI 앱 생성
app = FastAPI()
app.include_router(post_router, prefix="/post")
client = TestClient(app)

# 테스트 데이터 정의
@pytest.fixture
def sample_post():
    return Post(
        uid="test_user_uid",
        게시판종류이름="공지사항",
        내용="테스트 내용입니다.",
        댓글갯수=0,
        비밀번호="1234",
        작성일시=datetime.datetime.now(),
        작성자="테스트유저",
        제목="테스트 제목",
        조회수=0,
        좋아요수=0
    )

@pytest.fixture
def sample_comment():
    return Comment(
        uid="test_comment_uid",
        postid=1,
        내용="테스트 댓글입니다.",
        비밀번호="1234",
        작성일시=datetime.datetime.now(),
        작성자="댓글작성자"
    )

# PostRouter 테스트 클래스
class TestPostRouter:
    # 게시글 목록 조회 테스트
    @patch('routers.post_router.PostService.get_all_posts')
    def test_get_all_posts(self, mock_get_all_posts):
        # mock 설정
        mock_get_all_posts.return_value = [
            {"id": 1, "제목": "첫번째 글", "작성일시": "2025-03-01T12:00:00"},
            {"id": 2, "제목": "두번째 글", "작성일시": "2025-03-02T12:00:00"}
        ]
        
        # API 호출
        response = client.get("/post/posts")
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert len(data["data"]) == 2
        assert data["data"][0]["id"] == 1
        assert data["data"][1]["id"] == 2
        
    # 게시글 상세 조회 테스트
    @patch('routers.post_router.PostService.get_post_by_id')
    def test_read_post(self, mock_get_post):
        # mock 설정
        post_id = 1
        mock_get_post.return_value = {
            "post": {
                "id": post_id,
                "제목": "테스트 제목",
                "내용": "테스트 내용",
                "작성일시": "2025-03-01T12:00:00"
            },
            "comments": [
                {
                    "id": 101,
                    "postid": post_id,
                    "내용": "테스트 댓글",
                    "작성자": "댓글작성자",
                    "작성일시": "2025-03-01T12:30:00"
                }
            ]
        }
        
        # API 호출
        response = client.get(f"/post/posts/{post_id}")
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["post"]["id"] == post_id
        assert len(data["data"]["comments"]) == 1
        
    # 게시글 생성 테스트
    @patch('routers.post_router.PostService.create_post')
    def test_create_post(self, mock_create_post, sample_post):
        # mock 설정
        mock_create_post.return_value = {
            "id": 10,
            "message": "게시글이 성공적으로 생성되었습니다."
        }
        
        # API 호출
        post_data = sample_post.dict()
        # datetime 객체는 문자열로 변환
        post_data["작성일시"] = post_data["작성일시"].isoformat()
        
        response = client.post("/post/posts", json=post_data)
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["id"] == 10
        assert "message" in data["data"]
        
    # 게시글 수정 테스트
    @patch('routers.post_router.PostService.update_post')
    def test_update_post(self, mock_update_post, sample_post):
        # mock 설정
        post_id = 1
        mock_update_post.return_value = {
            "id": post_id,
            "message": "게시글이 성공적으로 수정되었습니다."
        }
        
        # API 호출
        post_data = sample_post.dict()
        # datetime 객체는 문자열로 변환
        post_data["작성일시"] = post_data["작성일시"].isoformat()
        
        response = client.put(f"/post/posts/{post_id}", json=post_data)
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["id"] == post_id
        assert "message" in data["data"]
        
    # 게시글 삭제 테스트
    @patch('routers.post_router.PostService.delete_post')
    def test_delete_post(self, mock_delete_post):
        # mock 설정
        post_id = 1
        mock_delete_post.return_value = {
            "id": post_id,
            "deleted_comments": 2,
            "message": "게시글과 관련 댓글 2개가 삭제되었습니다."
        }
        
        # API 호출
        response = client.delete(f"/post/posts/{post_id}")
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["id"] == post_id
        assert data["data"]["deleted_comments"] == 2
        
    # 댓글 생성 테스트
    @patch('routers.post_router.PostService.create_comment')
    def test_create_comment(self, mock_create_comment, sample_comment):
        # mock 설정
        mock_create_comment.return_value = {
            "id": 5,
            "postid": sample_comment.postid,
            "message": "댓글이 성공적으로 등록되었습니다."
        }
        
        # API 호출
        comment_data = sample_comment.dict()
        # datetime 객체는 문자열로 변환
        comment_data["작성일시"] = comment_data["작성일시"].isoformat()
        
        response = client.post("/post/comments", json=comment_data)
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["id"] == 5
        assert data["data"]["postid"] == sample_comment.postid
        
    # 조회수 증가 테스트
    @patch('routers.post_router.PostService.increase_view_count')
    def test_increase_views(self, mock_increase_views):
        # mock 설정
        post_id = 1
        mock_increase_views.return_value = {
            "id": post_id,
            "제목": "테스트 제목",
            "조회수": 6
        }
        
        # API 호출
        response = client.put(f"/post/posts/{post_id}/views")
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["id"] == post_id
        assert data["data"]["조회수"] == 6
        
    # 인기 게시글 조회 테스트
    @patch('routers.post_router.PostService.get_top_viewed_post')
    def test_get_top_viewed_post(self, mock_get_top_post):
        # mock 설정
        mock_get_top_post.return_value = [{
            "id": 2,
            "제목": "인기 게시글",
            "조회수": 100,
            "작성일시": "2025-03-01T12:00:00"
        }]
        
        # API 호출
        response = client.get("/post/posts/top-viewed")
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert len(data["data"]) == 1
        assert data["data"][0]["조회수"] == 100
        
    # 게시글 좋아요 테스트
    @patch('routers.post_router.PostService.like_post')
    def test_like_post(self, mock_like_post):
        # mock 설정
        post_id = 1
        mock_like_post.return_value = {
            "id": post_id,
            "likes": 6,
            "message": "게시글에 좋아요가 추가되었습니다."
        }
        
        # API 호출
        response = client.put(f"/post/posts/{post_id}/like")
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["id"] == post_id
        assert data["data"]["likes"] == 6
        
    # 게시글 신고 테스트 - Form 데이터 처리 방식으로 수정
    @patch('routers.post_router.PostService.report_post')
    def test_report_post(self, mock_report_post):
        # mock 설정
        post_id = 1
        reason = "부적절한 내용"
        mock_report_post.return_value = {
            "id": "report_id_123",
            "post_id": post_id,
            "message": "게시글이 신고되었습니다."
        }
        
        # API 호출 - Form 데이터 사용
        response = client.post(
            f"/post/posts/{post_id}/report", 
            data={"reason": reason}  # application/x-www-form-urlencoded 형식으로 전송
        )
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["post_id"] == post_id
        assert "message" in data["data"]
        
    # 댓글 신고 테스트 - Form 데이터 처리 방식으로 수정
    @patch('routers.post_router.PostService.report_comment')
    def test_report_comment(self, mock_report_comment):
        # mock 설정
        comment_id = 101
        reason = "부적절한 내용"
        mock_report_comment.return_value = {
            "id": "report_id_456",
            "comment_id": comment_id,
            "message": "댓글이 신고되었습니다."
        }
        
        # API 호출 - Form 데이터 사용
        response = client.post(
            f"/post/comments/{comment_id}/report",
            data={"reason": reason}  # application/x-www-form-urlencoded 형식으로 전송
        )
        
        # 응답 검증
        assert response.status_code == 200
        
        data = response.json()
        assert data["success"] == True
        assert data["data"]["comment_id"] == comment_id
        assert "message" in data["data"]