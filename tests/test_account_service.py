import pytest
from unittest.mock import patch, MagicMock
from fastapi import HTTPException
from manager.account_service import AccountService, UserData, UserEmailSignUp
import firebase_admin.auth


@pytest.fixture
def mock_firebase_auth():
    with patch("manager.account_service.auth") as mock_auth:
        yield mock_auth


@pytest.fixture
def mock_realtime_db():
    with patch("manager.account_service.realtime_db") as mock_db:
        yield mock_db


@pytest.fixture
def mock_firestore_db():
    with patch("manager.account_service.firestore_db") as mock_db:
        yield mock_db


@pytest.fixture
def mock_storage_bucket():
    with patch("manager.account_service.storage_bucket") as mock_bucket:
        yield mock_bucket


@pytest.fixture
def mock_requests():
    with patch("manager.account_service.requests") as mock_req:
        yield mock_req


class TestAccountService:
    @pytest.mark.asyncio
    async def test_create_email_user_success(self, mock_firebase_auth):
        # Arrange
        test_user = UserEmailSignUp(email="test@example.com", password="password123")
        mock_user = MagicMock()
        mock_user.uid = "test_uid_123"
        mock_firebase_auth.create_user.return_value = mock_user

        # Act
        result = await AccountService.create_email_user(test_user)

        # Assert
        mock_firebase_auth.create_user.assert_called_once_with(
            email=test_user.email, password=test_user.password
        )
        assert result == {"uid": "test_uid_123"}

    @pytest.mark.asyncio
    async def test_create_email_user_email_exists(self, mock_firebase_auth):
        # Arrange
        test_user = UserEmailSignUp(email="existing@example.com", password="password123")
        
        # EmailAlreadyExistsError는 실제 에러를 모킹하기 어려우므로 일반 HTTP 예외로 대체
        mock_firebase_auth.create_user.side_effect = HTTPException(
            status_code=400, detail="이미 등록된 이메일입니다."
        )

        # Act & Assert
        with pytest.raises(HTTPException) as exc_info:
            await AccountService.create_email_user(test_user)
        
        assert exc_info.value.status_code == 400
        assert "이미 등록된 이메일입니다" in exc_info.value.detail

    @pytest.mark.asyncio
    async def test_set_user_information_success(self, mock_firebase_auth, mock_realtime_db):
        # Arrange
        user_data = UserData(
            uid="test_uid_123", 
            nickname="테스트유저", 
            interest_genre=[1, 3, 5], 
            level=2
        )
        mock_firebase_auth.get_user.return_value = MagicMock()
        mock_ref = MagicMock()
        mock_realtime_db.child.return_value = mock_ref

        # Act
        result = await AccountService.set_user_information(user_data)

        # Assert
        mock_firebase_auth.get_user.assert_called_once_with(user_data.uid)
        mock_realtime_db.child.assert_called_once_with(f"users/{user_data.uid}")
        mock_ref.set.assert_called_once_with({
            "nickname": user_data.nickname,
            "interest_genre": user_data.interest_genre,
            "level": user_data.level
        })
        assert result == {"uid": user_data.uid}

    @pytest.mark.asyncio
    async def test_login_with_email_success(self, mock_requests):
        # Arrange
        login_data = UserEmailSignUp(
            email="test@example.com", 
            password="password123"
        )
        mock_response = MagicMock()
        mock_response.json.return_value = {"localId": "test_uid_123"}
        mock_requests.post.return_value = mock_response

        # Act
        result = await AccountService.login_with_email(login_data)

        # Assert
        assert mock_requests.post.called
        assert result == {"uid": "test_uid_123"}

    @pytest.mark.asyncio
    async def test_delete_user_account_success(
        self, mock_firebase_auth, mock_realtime_db, mock_firestore_db, mock_storage_bucket
    ):
        # Arrange
        test_uid = "test_uid_123"
        
        # Mock get_user
        mock_firebase_auth.get_user.return_value = MagicMock()
        
        # Mock realtime DB
        mock_ref = MagicMock()
        mock_ref.get.return_value = {"data": "exists"}
        mock_realtime_db.child.return_value = mock_ref
        
        # Mock Firestore
        mock_doc_ref = MagicMock()
        mock_doc_ref.get.return_value.exists = True
        mock_firestore_db.collection.return_value.document.return_value = mock_doc_ref
        
        # Mock score collection
        mock_score_docs = [MagicMock(), MagicMock()]
        mock_firestore_db.collection.return_value.stream.return_value = mock_score_docs
        
        # Mock storage blobs
        mock_blobs = [MagicMock(), MagicMock()]
        mock_storage_bucket.list_blobs.return_value = mock_blobs

        # Act
        result = await AccountService.delete_user_account(test_uid)

        # Assert
        mock_firebase_auth.get_user.assert_called_once_with(test_uid)
        mock_firebase_auth.delete_user.assert_called_once_with(test_uid)
        mock_realtime_db.child.assert_called_once_with(f"users/{test_uid}")
        mock_ref.delete.assert_called_once()
        mock_firestore_db.collection.assert_any_call("users")
        mock_firestore_db.collection.return_value.document.assert_called_once_with(test_uid)
        mock_doc_ref.delete.assert_called_once()
        mock_storage_bucket.list_blobs.assert_called_once_with(prefix=f"{test_uid}/")
        assert len(mock_blobs) == 2
        for blob in mock_blobs:
            blob.delete.assert_called_once()
        assert result == {"uid": test_uid}