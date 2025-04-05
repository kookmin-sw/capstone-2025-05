import os
from firebase_admin import auth, db
from manager.firebase_manager import firestore_db, realtime_db, storage_bucket
from pydantic import BaseModel, EmailStr
from typing import List, Dict, Any
import requests
from fastapi import HTTPException


class UserData(BaseModel):
    uid: str
    nickname: str
    interest_genre: List[int]  # 관심 장르 복수 선택 가능 (0~11)
    level: int  # 실력 (0~4)


class UserEmailSignUp(BaseModel):
    email: EmailStr
    password: str


class AccountService:
    GOOGLE_OAUTH2_URL = "https://accounts.google.com/o/oauth2/auth"
    GOOGLE_TOKEN_URL = "https://oauth2.googleapis.com/token"
    GOOGLE_USERINFO_URL = "https://www.googleapis.com/oauth2/v3/userinfo"
    
    @staticmethod
    def get_google_auth_url() -> str:
        """Google OAuth URL을 생성합니다"""
        # Use HTTPS for production environments
        base_url = os.getenv('API_BASE_URL', 'http://localhost:8000')
        if not base_url.startswith('https://') and not base_url.startswith('http://localhost'):
            base_url = 'https://' + base_url.lstrip('http://')
            
        redirect_uri = os.getenv('GOOGLE_REDIRECT_URI', f"{base_url}/account/google-auth-callback")
        google_auth_url = (
            f"{AccountService.GOOGLE_OAUTH2_URL}?response_type=code&"
            f"client_id={os.getenv('CLIENT_ID')}&"
            f"redirect_uri={redirect_uri}&"
            f"scope=openid profile email"
        )
        return google_auth_url

    @staticmethod
    async def process_google_auth(code: str) -> Dict[str, str]:
        """Google 인증 콜백을 처리합니다"""
        try:
            response = requests.post(
                AccountService.GOOGLE_TOKEN_URL,
                data={
                    "code": code,
                    "client_id": os.getenv("CLIENT_ID"),
                    "client_secret": os.getenv("CLIENT_SECRET"),
                    "redirect_uri": os.getenv("GOOGLE_REDIRECT_URI", "http://localhost:8000/account/google-auth-callback"),
                    "grant_type": "authorization_code",
                },
            )

            tokens = response.json()
            id_token = tokens.get("id_token")

            if not id_token:
                raise HTTPException(status_code=400, detail="구글 응답에서 id_token을 찾을 수 없습니다.")

            firebase_response = requests.post(
                f"https://identitytoolkit.googleapis.com/v1/accounts:signInWithIdp?key={os.getenv('API_KEY')}",
                headers={"Content-Type": "application/json"},
                json={
                    "postBody": f"id_token={id_token}&providerId=google.com",
                    "requestUri": "http://localhost",
                    "returnIdpCredential": True,
                    "returnSecureToken": True,
                }
            )
            
            firebase_response.raise_for_status()
            firebase_data = firebase_response.json()
            
            firebase_id_token = firebase_data.get("idToken")
            if not firebase_id_token:
                raise HTTPException(status_code=400, detail="firebase에서 id_token을 가져오는 것에 실패했습니다.")

            decoded_token = auth.verify_id_token(firebase_id_token)
            uid = decoded_token["uid"]

            return {
                "uid": uid
            }

        except requests.exceptions.RequestException as e:
            raise HTTPException(status_code=400, detail=f"구글 계정 오류: {str(e)}")
        except Exception as e:
            # 모든 Firebase 관련 예외는 HTTP 예외로 변환
            raise HTTPException(status_code=400, detail=f"구글 token 오류: {str(e)}")
    
    @staticmethod
    async def create_email_user(sign_up_data: UserEmailSignUp) -> Dict[str, str]:
        """이메일로 사용자를 생성합니다"""
        try:
            user = auth.create_user(
                email=sign_up_data.email,
                password=sign_up_data.password
            )
            return {"uid": user.uid}
        except Exception as e:
            # auth.EmailAlreadyExistsError는 모킹하기 어려워 일반 예외 처리
            # 실제 예외가 EmailAlreadyExistsError인 경우 적절한 상태 코드를 반환
            if "EMAIL_EXISTS" in str(e) or "이미 등록된 이메일" in str(e):
                raise HTTPException(status_code=400, detail="이미 등록된 이메일입니다.")
            else:
                raise HTTPException(status_code=500, detail=f"회원가입 실패: {str(e)}")
    
    @staticmethod
    async def set_user_information(user_info: UserData) -> Dict[str, str]:
        """사용자 정보를 설정합니다"""
        try:
            auth.get_user(user_info.uid)
        except Exception as e:
            # auth.UserNotFoundError는 모킹하기 어려워 일반 예외 처리
            if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            else:
                raise HTTPException(status_code=500, detail=f"사용자 정보 확인 실패: {str(e)}")

        user_ref = realtime_db.child(f"users/{user_info.uid}")
        user_ref.set({
            "nickname": user_info.nickname,
            "interest_genre": user_info.interest_genre,
            "level": user_info.level
        })

        return {"uid": user_info.uid}
    
    @staticmethod
    async def login_with_email(user_data: UserEmailSignUp) -> Dict[str, str]:
        """이메일로 로그인합니다"""
        try:
            response = requests.post(
                f"https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key={os.getenv('API_KEY')}",
                headers={"Content-Type": "application/json"},
                json={
                    "email": user_data.email,
                    "password": user_data.password,
                    "returnSecureToken": True
                }
            )

            response.raise_for_status()
            firebase_data = response.json()
            uid = firebase_data.get("localId")

            if not uid:
                raise HTTPException(status_code=400, detail="로그인 실패: UID를 가져올 수 없습니다.")

            return {"uid": uid}

        except requests.exceptions.RequestException as e:
            raise HTTPException(status_code=400, detail=f"로그인 실패: {str(e)}")
    
    @staticmethod
    async def delete_user_account(uid: str) -> Dict[str, str]:
        """사용자 계정을 삭제합니다"""
        try:
            # 사용자 존재 확인
            try:
                auth.get_user(uid)
            except Exception as e:
                # auth.UserNotFoundError는 모킹하기 어려워 일반 예외 처리
                if "USER_NOT_FOUND" in str(e) or "등록되지 않은 사용자" in str(e):
                    raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
                raise HTTPException(status_code=500, detail=f"사용자 확인 실패: {str(e)}")

            # Auth에서 사용자 삭제
            auth.delete_user(uid)

            # Realtime DB에서 사용자 데이터 삭제
            user_ref = realtime_db.child(f"users/{uid}")
            if user_ref.get():
                user_ref.delete()

            # Firestore에서 사용자 문서 삭제
            user_doc_ref = firestore_db.collection("users").document(uid)
            if user_doc_ref.get().exists:
                user_doc_ref.delete()

            # 사용자 점수 컬렉션 삭제
            score_collection_ref = firestore_db.collection(f"{uid}_score")
            score_docs = score_collection_ref.stream()
            for doc in score_docs:
                doc.reference.delete()

            # Storage에서 사용자 파일 삭제
            blobs = storage_bucket.list_blobs(prefix=f"{uid}/")
            for blob in blobs:
                blob.delete()

            return {"uid": uid}

        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"사용자 삭제 실패: {str(e)}")