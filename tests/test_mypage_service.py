import pytest
import io
from unittest.mock import AsyncMock, MagicMock, patch, mock_open
from fastapi import HTTPException, UploadFile
from fastapi.responses import StreamingResponse

from manager.mypage_service import MyPageService


class TestMyPageService:
    """마이페이지 서비스 테스트"""

    @pytest.mark.asyncio
    @patch("manager.mypage_service.realtime_db")
    @patch("manager.mypage_service.auth")
    async def test_edit_nickname(self, mock_auth, mock_realtime_db):
        # 모의 객체 설정
        mock_auth.get_user.return_value = MagicMock()
        mock_user_ref = MagicMock()
        mock_realtime_db.child.return_value = mock_user_ref
        mock_user_ref.get.return_value = {"nickname": "old_nickname"}
        mock_user_ref.update.return_value = None
        
        # 테스트
        result = await MyPageService.edit_nickname("test_uid", "new_nickname")
        
        # 검증
        mock_auth.get_user.assert_called_once_with("test_uid")
        mock_realtime_db.child.assert_called_once_with("users/test_uid")
        mock_user_ref.update.assert_called_once_with({"nickname": "new_nickname"})
        assert result == {"message": "닉네임 변경 완료"}

    @pytest.mark.asyncio
    @patch("manager.mypage_service.realtime_db")
    @patch("manager.mypage_service.auth")
    async def test_edit_nickname_exception(self, mock_auth, mock_realtime_db):
        # 모의 객체 설정 - 예외 발생
        mock_auth.get_user.side_effect = Exception("USER_NOT_FOUND")
        
        # 테스트 및 예외 검증
        with pytest.raises(HTTPException) as excinfo:
            await MyPageService.edit_nickname("test_uid", "new_nickname")
        
        assert excinfo.value.status_code == 400
        assert "등록되지 않은 사용자" in str(excinfo.value.detail)

    @pytest.mark.asyncio
    @patch("manager.mypage_service.realtime_db")
    @patch("manager.mypage_service.auth")
    async def test_edit_interest_genre(self, mock_auth, mock_realtime_db):
        # 모의 객체 설정
        mock_auth.get_user.return_value = MagicMock()
        mock_user_ref = MagicMock()
        mock_realtime_db.child.return_value = mock_user_ref
        mock_user_ref.get.return_value = {"interest_genre": [4, 5]}
        mock_user_ref.update.return_value = None
        
        # 테스트
        result = await MyPageService.edit_interest_genre("test_uid", [1, 2, 3])
        
        # 검증
        mock_auth.get_user.assert_called_once_with("test_uid")
        mock_realtime_db.child.assert_called_once_with("users/test_uid")
        mock_user_ref.update.assert_called_once_with({"interest_genre": [1, 2, 3]})
        assert result == {"message": "관심 장르 변경 완료"}

    @pytest.mark.asyncio
    @patch("manager.mypage_service.realtime_db")
    @patch("manager.mypage_service.auth")
    async def test_edit_level(self, mock_auth, mock_realtime_db):
        # 모의 객체 설정
        mock_auth.get_user.return_value = MagicMock()
        mock_user_ref = MagicMock()
        mock_realtime_db.child.return_value = mock_user_ref
        mock_user_ref.get.return_value = {"level": 1}
        mock_user_ref.update.return_value = None
        
        # 테스트
        result = await MyPageService.edit_level("test_uid", 3)
        
        # 검증
        mock_auth.get_user.assert_called_once_with("test_uid")
        mock_realtime_db.child.assert_called_once_with("users/test_uid")
        mock_user_ref.update.assert_called_once_with({"level": 3})
        assert result == {"message": "실력 변경 완료"}

    @pytest.mark.asyncio
    @patch("manager.mypage_service.storage_bucket")
    @patch("manager.mypage_service.realtime_db")
    @patch("manager.mypage_service.auth")
    @patch("manager.mypage_service.uuid")
    async def test_change_profile_image(self, mock_uuid, mock_auth, mock_realtime_db, mock_storage_bucket):
        # 모의 객체 설정
        mock_auth.get_user.return_value = MagicMock()
        mock_user_ref = MagicMock()
        mock_realtime_db.child.return_value = mock_user_ref
        mock_user_ref.get.return_value = {"profile_image": "old_url"}
        
        # UUID 모의 설정
        mock_uuid.uuid4.return_value = "test-uuid"
        
        # 모의 blob 객체 설정
        mock_blob = MagicMock()
        mock_storage_bucket.blob.return_value = mock_blob
        mock_blob.public_url = "https://example.com/profile.jpg"
        
        # 모의 리스트 블롭 설정
        mock_storage_bucket.list_blobs.return_value = [MagicMock()]
        
        # 모의 파일 객체 설정
        mock_file = MagicMock(spec=UploadFile)
        mock_file.filename = "profile.jpg"
        mock_file.file = io.BytesIO(b"test file content")
        mock_file.content_type = "image/jpeg"
        
        # 테스트
        result = await MyPageService.change_profile_image("test_uid", mock_file)
        
        # 검증
        mock_auth.get_user.assert_called_once_with("test_uid")
        mock_storage_bucket.list_blobs.assert_called_once_with(prefix="test_uid/profile/")
        mock_storage_bucket.blob.assert_called_once_with("test_uid/profile/test-uuid.jpg")
        mock_blob.upload_from_file.assert_called_once()
        mock_user_ref.update.assert_called_once_with({"profile_image": "https://example.com/profile.jpg"})
        assert result["message"] == "프로필 사진 변경 완료"
        assert result["profile_image_url"] == "https://example.com/profile.jpg"

    @pytest.mark.asyncio
    @patch("manager.mypage_service.storage_bucket")
    @patch("manager.mypage_service.MyPageService._get_score_data")
    async def test_get_all_records(self, mock_get_score_data, mock_storage_bucket):
        # 모의 blob 설정
        mock_blob1 = MagicMock()
        mock_blob1.name = "test_uid/record/song1/1/recording.mp3"
        mock_blob1.public_url = "https://example.com/song1_1.mp3"
        
        mock_blob2 = MagicMock()
        mock_blob2.name = "test_uid/record/song2/1/recording.mp3"
        mock_blob2.public_url = "https://example.com/song2_1.mp3"
        
        mock_storage_bucket.list_blobs.return_value = [mock_blob1, mock_blob2]
        
        # 점수 데이터 모의 설정
        def mock_get_score_side_effect(uid, song_name, upload_count):
            return {
                "tempo": 120 if song_name == "song1" else 130,
                "beat": 0.85 if song_name == "song1" else 0.9,
                "interval": 0.75 if song_name == "song1" else 0.8,
                "date": "2025-04-05" if song_name == "song1" else "2025-04-06"
            }
        
        mock_get_score_data.side_effect = mock_get_score_side_effect
        
        # 테스트
        result = await MyPageService.get_all_records("test_uid")
        
        # 검증
        mock_storage_bucket.list_blobs.assert_called_once_with(prefix="test_uid/record/")
        assert "records" in result
        assert "song1" in result["records"]
        assert "song2" in result["records"]
        assert len(result["records"]["song1"]) == 1
        assert len(result["records"]["song2"]) == 1
        assert result["records"]["song1"][0]["tempo"] == 120
        assert result["records"]["song2"][0]["tempo"] == 130
        assert result["records"]["song1"][0]["audio_url"] == "https://example.com/song1_1.mp3"

    @pytest.mark.asyncio
    @patch("manager.mypage_service.MyPageService._get_score_data")
    async def test_get_specific_record(self, mock_get_score_data):
        # 모의 객체 설정
        mock_get_score_data.return_value = {
            "tempo": 120,
            "beat": 0.85,
            "interval": 0.75,
            "date": "2025-04-05"
        }
        
        # 테스트
        result = await MyPageService.get_specific_record("test_uid", "song1", 1)
        
        # 검증
        mock_get_score_data.assert_called_once_with("test_uid", "song1", 1)
        assert result["tempo"] == 120
        assert result["beat"] == 0.85
        assert result["interval"] == 0.75
        assert result["date"] == "2025-04-05"
        assert result["upload_count"] == 1

    @pytest.mark.asyncio
    @patch("manager.mypage_service.MyPageService._get_score_data")
    async def test_get_specific_record_not_found(self, mock_get_score_data):
        # 모의 객체 설정 - 데이터가 없는 경우
        mock_get_score_data.return_value = None
        
        # 테스트 및 예외 검증
        with pytest.raises(HTTPException) as excinfo:
            await MyPageService.get_specific_record("test_uid", "song1", 1)
        
        assert excinfo.value.status_code == 404
        assert "해당 연습 기록이 없습니다" in str(excinfo.value.detail)

    @pytest.mark.asyncio
    @patch("manager.mypage_service.storage_bucket")
    async def test_get_record_audio(self, mock_storage_bucket):
        # 모의 블롭 설정
        mock_blob = MagicMock()
        mock_blob.name = "test_uid/record/song1/1/recording.mp3"
        mock_blob.download_as_bytes.return_value = b"fake audio data"
        
        # 리스트 블롭 설정
        mock_storage_bucket.list_blobs.return_value = [mock_blob]
        
        # 테스트
        result = await MyPageService.get_record_audio("test_uid", "song1", 1)
        
        # 검증
        mock_storage_bucket.list_blobs.assert_called_once_with(prefix="test_uid/record/song1/1/")
        assert isinstance(result, StreamingResponse)
        assert result.media_type == "audio/mpeg"

    @pytest.mark.asyncio
    @patch("manager.mypage_service.storage_bucket")
    async def test_get_record_audio_not_found(self, mock_storage_bucket):
        # 모의 객체 설정 - 파일이 없는 경우
        mock_storage_bucket.list_blobs.return_value = []
        
        # 테스트 및 예외 검증
        with pytest.raises(HTTPException) as excinfo:
            await MyPageService.get_record_audio("test_uid", "song1", 1)
        
        assert excinfo.value.status_code == 404
        assert "해당 연습 파일이 없습니다" in str(excinfo.value.detail)

    @pytest.mark.asyncio
    @patch("manager.mypage_service.MyPageService._get_posts_by_activity")
    async def test_get_my_posts(self, mock_get_posts):
        # 모의 포스트 데이터 설정
        mock_posts = [
            {"post_id": "post1", "title": "첫 번째 게시글"},
            {"post_id": "post2", "title": "두 번째 게시글"}
        ]
        mock_get_posts.return_value = mock_posts
        
        # 테스트
        result = await MyPageService.get_my_posts("test_uid")
        
        # 검증
        mock_get_posts.assert_called_once_with("test_uid", "post")
        assert "my_posts" in result
        assert len(result["my_posts"]) == 2
        assert result["my_posts"][0]["post_id"] == "post1"
        assert result["my_posts"][1]["post_id"] == "post2"

    @pytest.mark.asyncio
    @patch("manager.mypage_service.MyPageService._get_posts_by_activity")
    async def test_get_my_scraps(self, mock_get_posts):
        # 모의 스크랩 데이터 설정
        mock_scraps = [
            {"post_id": "post1", "title": "스크랩한 게시글 1"},
            {"post_id": "post2", "title": "스크랩한 게시글 2"}
        ]
        mock_get_posts.return_value = mock_scraps
        
        # 테스트
        result = await MyPageService.get_my_scraps("test_uid")
        
        # 검증
        mock_get_posts.assert_called_once_with("test_uid", "scrap")
        assert "my_scraps" in result
        assert len(result["my_scraps"]) == 2
        assert result["my_scraps"][0]["post_id"] == "post1"
        assert result["my_scraps"][1]["post_id"] == "post2"

    @pytest.mark.asyncio
    @patch("manager.mypage_service.MyPageService._get_posts_by_activity")
    async def test_get_my_likes(self, mock_get_posts):
        # 모의 좋아요 데이터 설정
        mock_likes = [
            {"post_id": "post1", "title": "좋아요한 게시글 1"},
            {"post_id": "post2", "title": "좋아요한 게시글 2"}
        ]
        mock_get_posts.return_value = mock_likes
        
        # 테스트
        result = await MyPageService.get_my_likes("test_uid")
        
        # 검증
        mock_get_posts.assert_called_once_with("test_uid", "like")
        assert "my_likes" in result
        assert len(result["my_likes"]) == 2
        assert result["my_likes"][0]["post_id"] == "post1"
        assert result["my_likes"][1]["post_id"] == "post2"

    @pytest.mark.asyncio
    @patch("manager.mypage_service.firestore_db")
    async def test_get_posts_by_activity(self, mock_firestore_db):
        # 모의 객체 설정
        mock_activity_collection = MagicMock()
        mock_doc_collection = MagicMock()
        mock_firestore_db.collection.side_effect = lambda collection_name: mock_activity_collection if collection_name == "my_activity" else mock_doc_collection
        
        # 모의 활동 문서 설정
        mock_activity_doc = MagicMock()
        mock_activity_ref = MagicMock()
        mock_activity_collection.document.return_value = mock_activity_doc
        mock_activity_doc.collection.return_value = mock_activity_ref
        
        mock_doc1 = MagicMock()
        mock_doc1.id = "post1"
        mock_doc2 = MagicMock()
        mock_doc2.id = "post2"
        mock_activity_ref.stream.return_value = [mock_doc1, mock_doc2]
        
        # 모의 포스트 문서 설정
        mock_post_doc1 = MagicMock()
        mock_post_doc1.exists = True
        mock_post_doc1.to_dict.return_value = {"제목": "게시글 1"}
        
        mock_post_doc2 = MagicMock()
        mock_post_doc2.exists = True
        mock_post_doc2.to_dict.return_value = {"제목": "게시글 2"}
        
        mock_doc_collection.document.side_effect = lambda doc_id: MagicMock(get=lambda: mock_post_doc1 if doc_id == "post1" else mock_post_doc2)
        
        # 테스트
        result = MyPageService._get_posts_by_activity("test_uid", "post")
        
        # 검증
        assert len(result) == 2
        assert result[0]["post_id"] == "post1"
        assert result[0]["title"] == "게시글 1"
        assert result[1]["post_id"] == "post2"
        assert result[1]["title"] == "게시글 2"