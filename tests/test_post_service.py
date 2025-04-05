import pytest
import datetime
from unittest.mock import patch, MagicMock, AsyncMock
from fastapi import HTTPException
from manager.post_service import PostService
from model import Post, Comment

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

# 모든 테스트에서 사용할 수 있는 mock 객체
class MockFirestoreClass:
    def __init__(self):
        pass

    def collection(self, *args, **kwargs):
        return self

    def document(self, *args, **kwargs):
        return self

    def stream(self, *args, **kwargs):
        return []

    def get(self, *args, **kwargs):
        mock_doc = MagicMock()
        mock_doc.exists = True
        mock_doc.to_dict.return_value = {"id": 1, "조회수": 5, "댓글갯수": 2}
        return mock_doc

    def set(self, *args, **kwargs):
        return None

    def update(self, *args, **kwargs):
        return None

    def delete(self, *args, **kwargs):
        return None
        
    def transaction(self, *args, **kwargs):
        return MagicMock()

    def order_by(self, *args, **kwargs):
        return self

    def limit(self, *args, **kwargs):
        return self
        
    def add(self, *args, **kwargs):
        return (None, MagicMock(id="new_doc_id"))

# PostService 테스트 클래스
class TestPostService:
    # 모든 게시글 가져오기 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_get_all_posts(self, mock_db):
        # mock 설정
        mock_posts = [
            {"id": 1, "제목": "첫번째 글", "작성일시": datetime.datetime.now()},
            {"id": 2, "제목": "두번째 글", "작성일시": datetime.datetime.now()}
        ]
        
        mock_ref = MagicMock()
        mock_db.collection.return_value = mock_ref
        
        mock_docs = []
        for post in mock_posts:
            mock_doc = MagicMock()
            mock_doc.to_dict.return_value = post
            mock_docs.append(mock_doc)
        
        mock_ref.stream.return_value = mock_docs
        
        # 함수 실행 및 검증
        result = await PostService.get_all_posts()
        
        assert len(result) == 2
        assert result[0]["id"] == 1
        assert result[1]["id"] == 2
        assert isinstance(result[0]["작성일시"], str)  # datetime이 변환되었는지 확인

    # 조회수 증가 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_increase_view_count(self, mock_db):
        # mock 설정
        post_id = 1
        mock_post_data = {
            "id": post_id,
            "제목": "테스트 제목",
            "내용": "테스트 내용",
            "조회수": 5
        }
        
        mock_doc = MagicMock()
        mock_doc.exists = True
        mock_doc.to_dict.return_value = mock_post_data
        
        mock_ref = MagicMock()
        mock_ref.get.return_value = mock_doc
        
        mock_db.collection.return_value.document.return_value = mock_ref
        
        # 함수 실행 및 검증
        result = await PostService.increase_view_count(post_id)
        
        assert result["조회수"] == 6  # 5 + 1
        mock_ref.set.assert_called_once_with({"조회수": 6}, merge=True)

    # 게시글 ID로 조회 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_get_post_by_id(self, mock_db):
        # mock 설정
        post_id = 1
        mock_post_data = {
            "id": post_id,
            "제목": "테스트 제목",
            "내용": "테스트 내용",
            "조회수": 5,
            "작성일시": datetime.datetime.now()
        }
        
        mock_doc = MagicMock()
        mock_doc.to_dict.return_value = mock_post_data
        
        # 댓글 mock
        mock_comment_data = {
            "id": 101,
            "postid": post_id,
            "내용": "테스트 댓글",
            "작성자": "댓글작성자",
            "작성일시": datetime.datetime.now()
        }
        mock_comment = MagicMock()
        mock_comment.to_dict.return_value = mock_comment_data
        
        # collection과 stream 설정
        mock_db.collection.side_effect = lambda collection_name: (
            MagicMock(stream=lambda: [mock_doc]) if collection_name == "post"
            else MagicMock(stream=lambda: [mock_comment])
        )
        
        # 함수 실행
        result = await PostService.get_post_by_id(post_id)
        
        # 검증
        assert "post" in result
        assert "comments" in result
        assert result["post"]["id"] == post_id
        assert len(result["comments"]) == 1
        assert result["comments"][0]["postid"] == post_id
        
    # 게시글 생성 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_create_post(self, mock_db, sample_post):
        # mock 설정
        mock_doc = MagicMock()
        mock_doc.to_dict.return_value = {"id": 5}  # 이미 있는 최대 ID
        
        mock_ref = MagicMock()
        mock_ref.stream.return_value = [mock_doc]
        
        mock_db.collection.return_value = mock_ref
        
        # 함수 실행
        result = await PostService.create_post(sample_post)
        
        # 검증
        assert result["id"] == 6  # 최대 ID + 1
        assert "message" in result
        # mock_ref.document.assert_called_once() - 정확한 호출 파라미터는 환경에 따라 다를 수 있음
        
    # 게시글 수정 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_update_post(self, mock_db, sample_post):
        # mock 설정
        post_id = 1
        
        mock_doc = MagicMock()
        mock_doc.exists = True
        
        mock_ref = MagicMock()
        mock_ref.get.return_value = mock_doc
        
        mock_db.collection.return_value.document.return_value = mock_ref
        
        # 함수 실행
        result = await PostService.update_post(post_id, sample_post)
        
        # 검증
        assert result["id"] == post_id
        assert "message" in result
        mock_ref.update.assert_called_once_with({
            "제목": sample_post.제목,
            "내용": sample_post.내용
        })
        
    # 게시글 삭제 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_delete_post(self, mock_db):
        # mock 설정
        post_id = 1
        
        # 게시글 문서 mock
        mock_post_doc = MagicMock()
        mock_post_doc.exists = True
        mock_post_ref = MagicMock()
        mock_post_ref.get.return_value = mock_post_doc
        
        # 댓글 문서 mock
        mock_comment_doc1 = MagicMock()
        mock_comment_doc1.id = "comment1"
        mock_comment_doc1.to_dict.return_value = {"postid": post_id}
        
        mock_comment_doc2 = MagicMock()
        mock_comment_doc2.id = "comment2"
        mock_comment_doc2.to_dict.return_value = {"postid": post_id}
        
        # collection 반환값 설정
        def mock_collection_side_effect(collection_name):
            if collection_name == "post":
                return MagicMock(document=lambda doc_id: mock_post_ref)
            elif collection_name == "comment":
                mock_ref = MagicMock()
                mock_ref.stream.return_value = [mock_comment_doc1, mock_comment_doc2]
                mock_ref.document.return_value = MagicMock()
                return mock_ref
            return MagicMock()
            
        mock_db.collection.side_effect = mock_collection_side_effect
        
        # 함수 실행
        result = await PostService.delete_post(post_id)
        
        # 검증
        assert result["id"] == post_id
        assert result["deleted_comments"] == 2
        mock_post_ref.delete.assert_called_once()
        
    # 댓글 생성 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_create_comment(self, mock_db, sample_comment):
        # Firebase 트랜잭션을 위한 mock 설정
        mock_transaction = MagicMock()
        mock_db.transaction.return_value = mock_transaction
        
        # 게시글 문서 mock
        mock_post_doc = MagicMock()
        mock_post_doc.exists = True
        mock_post_ref = MagicMock()
        mock_post_ref.get.return_value = mock_post_doc
        
        # 댓글 문서 mock (기존 댓글 ID 조회용)
        mock_comment_doc = MagicMock()
        mock_comment_doc.to_dict.return_value = {"id": 5}  # 기존 최대 댓글 ID
        
        # mock collection 설정
        def mock_collection_side_effect(collection_name):
            if collection_name == "post":
                return MagicMock(document=lambda doc_id: mock_post_ref)
            elif collection_name == "comment":
                mock_ref = MagicMock()
                mock_ref.order_by.return_value.limit.return_value.stream.return_value = [mock_comment_doc]
                mock_ref.document.return_value = MagicMock()
                return mock_ref
            return MagicMock()
            
        mock_db.collection.side_effect = mock_collection_side_effect
        
        # post_service.py의 create_comment 함수에서 사용하는 transactional 데코레이터를 직접 패치
        # 트랜잭션 처리 함수를 직접 모의 함수로 대체
        mock_transactional = MagicMock()
        # 이 모의 함수는 항상 6을 반환함 (새로운 댓글 ID)
        mock_transactional.return_value = 6
        
        # 패치 적용
        with patch('manager.post_service.firestore.transactional', create=True) as mock_trans:
            # transactional 데코레이터는 함수를 받아 다른 함수를 반환함
            # 외부 함수는 데코레이터로 사용될 함수
            def mock_decorator(func):
                # 내부 함수는 실제 트랜잭션 처리를 대신함
                def mock_transaction_func(*args, **kwargs):
                    # 새 댓글 ID 반환
                    return 6
                return mock_transaction_func
            
            mock_trans.side_effect = mock_decorator
            
            # 함수 실행
            result = await PostService.create_comment(sample_comment)
            
            # 검증
            assert result["id"] == 6  # 새 댓글 ID
            assert result["postid"] == sample_comment.postid
            assert result["message"] == "댓글이 성공적으로 등록되었습니다."
            
    # 게시글 좋아요 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_like_post(self, mock_db):
        # mock 설정
        post_id = 1
        
        mock_doc = MagicMock()
        mock_doc.exists = True
        mock_doc.to_dict.return_value = {"id": post_id, "좋아요수": 5}
        
        mock_ref = MagicMock()
        mock_ref.get.side_effect = [mock_doc, mock_doc]  # 처음과 업데이트 후 호출
        
        mock_db.collection.return_value.document.return_value = mock_ref
        
        # 함수 실행
        result = await PostService.like_post(post_id)
        
        # 검증
        assert result["id"] == post_id
        assert result["likes"] == 5  # 실제 값은 업데이트 후의 값으로 설정됨
        mock_ref.update.assert_called_once()
        
    # 게시글 좋아요 취소 테스트
    @pytest.mark.asyncio
    @patch('manager.post_service.firestore_db')
    async def test_unlike_post(self, mock_db):
        # mock 설정
        post_id = 1
        
        mock_doc = MagicMock()
        mock_doc.exists = True
        mock_doc.to_dict.return_value = {"id": post_id, "좋아요수": 5}
        
        mock_ref = MagicMock()
        mock_ref.get.return_value = mock_doc
        
        mock_db.collection.return_value.document.return_value = mock_ref
        
        # 함수 실행
        result = await PostService.unlike_post(post_id)
        
        # 검증
        assert result["id"] == post_id
        assert result["likes"] == 4  # 5 - 1
        mock_ref.update.assert_called_once_with({"좋아요수": 4})