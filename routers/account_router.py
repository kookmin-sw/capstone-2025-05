from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
from manager.firebase_manager import firestore_db, storage_bucket
from firebase_admin import auth, db
from pydantic import BaseModel, EmailStr
from typing import List
import requests
import os

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

@router.get("/google-login", tags=["Account"])
async def google_login():
    redirect_uri = os.getenv('GOOGLE_REDIRECT_URI', 'https://maple.ne.kr/api/google-auth-callback') 
    google_auth_url = (
        f"{GOOGLE_OAUTH2_URL}?response_type=code&"
        f"client_id={os.getenv('CLIENT_ID')}&"
        f"redirect_uri={redirect_uri}&" 
        f"scope=openid profile email"
    )
    return RedirectResponse(url=google_auth_url)

@router.get("/google-auth-callback", tags=["Account"])
async def google_auth_callback(code: str):
    try:
        redirect_uri = os.getenv("GOOGLE_REDIRECT_URI", "https://maple.ne.kr/api/google-auth-callback")

        response = requests.post(
            GOOGLE_TOKEN_URL,
            data={
                "code": code,
                "client_id": os.getenv("CLIENT_ID"),
                "client_secret": os.getenv("CLIENT_SECRET"),
                "redirect_uri": redirect_uri,
                "grant_type": "authorization_code",
            },
        )

        tokens = response.json()

        id_token = tokens.get("id_token")

        if not id_token:
            raise HTTPException(status_code=400, detail={"error": "구글 응답에서 id_token 없음", "tokens": tokens})

        firebase_response = requests.post(
            f"https://identitytoolkit.googleapis.com/v1/accounts:signInWithIdp?key={os.getenv('API_KEY')}",
            headers={"Content-Type": "application/json"},
            json={
                "postBody": f"id_token={id_token}&providerId=google.com",
                "requestUri": redirect_uri, 
                "returnIdpCredential": True,
                "returnSecureToken": True,
            }
        )
        
        firebase_response.raise_for_status()
        firebase_data = firebase_response.json()
        
        firebase_id_token = firebase_data.get("idToken")

        if not firebase_id_token:
            raise HTTPException(status_code=400, detail={"error": "firebase 응답에서 id_token 없음", "firebase_data": firebase_data})

        decoded_token = auth.verify_id_token(firebase_id_token)
        uid = decoded_token["uid"]

        return {
            "message": "로그인 성공",
            "uid": uid
        }

    except requests.exceptions.RequestException as e:
        raise HTTPException(status_code=400, detail=f"구글 계정 요청 오류: {str(e)}")
    except auth.InvalidIdTokenError:
        raise HTTPException(status_code=400, detail="firebase id_token 검증 오류")
    except Exception as e:
        import traceback
        tb = traceback.format_exc()
        raise HTTPException(status_code=500, detail={"error": "알 수 없는 서버 오류", "traceback": tb})

@router.post("/email-sign-up", tags=["Account"])
async def email_sign_up(sign_up_data: UserEmailSignUp):
    try:
        user = auth.create_user(
            email=sign_up_data.email,
            password=sign_up_data.password
        )
        return {"message": "회원가입 성공", "uid": user.uid}
    except auth.EmailAlreadyExistsError:
        print("이미 등록된 이메일")
        raise HTTPException(status_code=400, detail="이미 등록된 이메일입니다.")
    except Exception as e:
        print("원인불명 회원가입 실패")
        raise HTTPException(status_code=500, detail=f"회원가입 실패: {str(e)}")

@router.post("/set-user-info", tags=["Account"])
async def set_user_info(user_info: UserData):
    try:
        auth.get_user(user_info.uid)
    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")

    user_ref = db.reference(f"/users/{user_info.uid}")
    user_ref.set({
        "nickname": user_info.nickname,
        "interest_genre": user_info.interest_genre,
        "level": user_info.level
    })

    return {"message": "유저 정보 입력 완료"}

@router.post("/email-login", tags=["Account"])
async def email_login(user_data: UserEmailSignUp):
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

        return {"message": "로그인 성공", "uid": uid}

    except requests.exceptions.RequestException as e:
        print("비밀번호 틀렸을 가능성 높음")
        raise HTTPException(status_code=400, detail=f"로그인 실패: {str(e)}")

@router.delete("/delete-user/{uid}", tags=["Account"])
async def delete_user(uid: str):
    try:
        try:
            auth.get_user(uid)
        except auth.UserNotFoundError:
            print("등록되지 않은 사용자")
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
        print(f"Firestore에서 {uid}_score 컬렉션 삭제 완료")

        blobs = storage_bucket.list_blobs(prefix=f"{uid}/")
        for blob in blobs:
            blob.delete()
        print(f"Storage에서 {uid} 폴더 삭제 완료")

        print("사용자 삭제 완료")
        return {"message": "사용자 삭제 완료"}

    except HTTPException as e:
        raise e
    except Exception as e:
        print(f"사용자 삭제 실패: {str(e)}")
        raise HTTPException(status_code=500, detail=f"사용자 삭제 실패: {str(e)}")
