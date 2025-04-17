from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
from manager.firebase_manager import firestore_db, storage_bucket
from firebase_admin import auth, db
from pydantic import BaseModel, EmailStr
from typing import List
import requests
import os
from models.response_models import StandardResponse

router = APIRouter()

GOOGLE_OAUTH2_URL = "https://accounts.google.com/o/oauth2/auth"
GOOGLE_TOKEN_URL = "https://oauth2.googleapis.com/token"
GOOGLE_USERINFO_URL = "https://www.googleapis.com/oauth2/v3/userinfo"

class UserData(BaseModel):
    uid: str
    nickname: str
    interest_genre: List[int]  # 관심 장르 복수 선택 가능 (0~11)
    level: int  # 실력 (0~4)

class UserEmailSignUp(BaseModel):
    email: EmailStr
    password: str

# 테스트용 서비스 클래스 추가
class AccountService:
    @staticmethod
    def get_google_auth_url():
        redirect_uri = os.getenv('GOOGLE_REDIRECT_URI', 'http://localhost:8000/google-auth-callback') 
        google_auth_url = (
            f"{GOOGLE_OAUTH2_URL}?response_type=code&"
            f"client_id={os.getenv('CLIENT_ID')}&"
            f"redirect_uri={redirect_uri}&" 
            f"scope=openid profile email"
        )
        return google_auth_url
    
    @staticmethod
    async def process_google_auth(code: str):
        try:
            response = requests.post(
                GOOGLE_TOKEN_URL,
                data={
                    "code": code,
                    "client_id": os.getenv("CLIENT_ID"),
                    "client_secret": os.getenv("CLIENT_SECRET"),
                    "redirect_uri": os.getenv("GOOGLE_REDIRECT_URI", "http://localhost:8000/google-auth-callback"),
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
                "message": "로그인 성공",
                "uid": uid
            }

        except requests.exceptions.RequestException as e:
            raise HTTPException(status_code=400, detail=f"구글 계정 오류: {str(e)}")
        except auth.InvalidIdTokenError:
            raise HTTPException(status_code=400, detail="구글 token 오류")
    
    @staticmethod
    async def create_email_user(email: str, password: str):
        try:
            user = auth.create_user(
                email=email,
                password=password
            )
            return {"message": "회원가입 성공", "uid": user.uid}
        except auth.EmailAlreadyExistsError:
            raise HTTPException(status_code=400, detail="이미 등록된 이메일입니다.")
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"회원가입 실패: {str(e)}")
    
    @staticmethod
    async def set_user_information(uid: str, nickname: str, interest_genre: List[int], level: int):
        try:
            auth.get_user(uid)
        except auth.UserNotFoundError:
            raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")

        user_ref = db.reference(f"/users/{uid}")
        user_ref.set({
            "nickname": nickname,
            "interest_genre": interest_genre,
            "level": level
        })

        return {"message": "유저 정보 입력 완료", "uid": uid}
    
    @staticmethod
    async def login_with_email(email: str, password: str):
        try:
            response = requests.post(
                f"https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key={os.getenv('API_KEY')}",
                headers={"Content-Type": "application/json"},
                json={
                    "email": email,
                    "password": password,
                    "returnSecureToken": True
                }
            )

            response.raise_for_status()
            firebase_data = response.json()
            uid = firebase_data.get("localId")

            if not uid:
                raise HTTPException(status_code=400, detail="로그인 실패: UID를 가져올 수 없습니다.")

            return {"message": "로그인 성공", "uid": uid}

        except requests.exceptions.RequestException as e:
            raise HTTPException(status_code=400, detail=f"로그인 실패: {str(e)}")
    
    @staticmethod
    async def delete_user_account(uid: str):
        try:
            try:
                auth.get_user(uid)
            except auth.UserNotFoundError:
                raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")

            auth.delete_user(uid)

            user_ref = db.reference(f"/users/{uid}")
            if user_ref.get():
                user_ref.delete()

            user_doc_ref = firestore_db.collection("users").document(uid)
            if user_doc_ref.get().exists:
                user_doc_ref.delete()

            score_collection_ref = firestore_db.collection(f"{uid}_score")
            score_docs = score_collection_ref.stream()
            for doc in score_docs:
                doc.reference.delete()

            blobs = storage_bucket.list_blobs(prefix=f"{uid}/")
            for blob in blobs:
                blob.delete()

            return {"message": "사용자 삭제 완료", "uid": uid}

        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"사용자 삭제 실패: {str(e)}")

# 이후에 라우터 함수를 AccountService를 사용하도록 수정
@router.get("/google-login", tags=["Account"])
async def google_login():
    google_auth_url = AccountService.get_google_auth_url()
    return RedirectResponse(url=google_auth_url)

@router.get("/google-auth-callback", tags=["Account"])
async def google_auth_callback(code: str):
    try:
        result = await AccountService.process_google_auth(code)
        return StandardResponse(
            success=True,
            message="로그인 성공",
            data={"uid": result["uid"]}
        )
    except HTTPException as e:
        print("구글 계정 오류" if e.status_code == 400 else "구글 토큰 오류")
        raise e

@router.post("/email-sign-up", tags=["Account"])
async def email_sign_up(sign_up_data: UserEmailSignUp):
    try:
        result = await AccountService.create_email_user(sign_up_data.email, sign_up_data.password)
        return StandardResponse(
            success=True,
            message="회원가입 성공",
            data={"uid": result["uid"]}
        )
    except HTTPException as e:
        if e.status_code == 400:
            print("이미 등록된 이메일")
        else:
            print("원인불명 회원가입 실패")
        raise e

@router.post("/set-user-info", tags=["Account"])
async def set_user_info(user_info: UserData):
    try:
        result = await AccountService.set_user_information(
            user_info.uid, 
            user_info.nickname, 
            user_info.interest_genre, 
            user_info.level
        )
        return StandardResponse(
            success=True,
            message="유저 정보 입력 완료",
            data={"uid": result["uid"]}
        )
    except HTTPException as e:
        print("등록되지 않은 사용자" if e.status_code == 400 else "유저 정보 입력 실패")
        raise e

@router.post("/email-login", tags=["Account"])
async def email_login(user_data: UserEmailSignUp):
    try:
        result = await AccountService.login_with_email(user_data.email, user_data.password)
        return StandardResponse(
            success=True,
            message="로그인 성공",
            data={"uid": result["uid"]}
        )
    except HTTPException as e:
        print("비밀번호 틀렸을 가능성 높음" if e.status_code == 400 else "로그인 실패")
        raise e

@router.delete("/delete-user/{uid}", tags=["Account"])
async def delete_user(uid: str):
    try:
        result = await AccountService.delete_user_account(uid)
        return StandardResponse(
            success=True,
            message="사용자 삭제 완료",
            data={"uid": result["uid"]}
        )
    except HTTPException as e:
        if e.status_code == 400:
            print("등록되지 않은 사용자")
        else:
            print(f"사용자 삭제 실패: {str(e)}")
        raise e