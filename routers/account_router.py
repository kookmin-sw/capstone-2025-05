from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
from firebase_admin import auth, db
from firebase_admin import firestore
from pydantic import BaseModel
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

@router.get("/google-login")
async def google_login():
    redirect_uri = os.getenv('GOOGLE_REDIRECT_URI', 'http://localhost:8000/google-auth-callback') 
    google_auth_url = (
        f"{GOOGLE_OAUTH2_URL}?response_type=code&"
        f"client_id={os.getenv('CLIENT_ID')}&"
        f"redirect_uri={redirect_uri}&" 
        f"scope=openid profile email"
    )
    return RedirectResponse(url=google_auth_url)

@router.get("/google-auth-callback")
async def google_auth_callback(code: str):
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
            print("구글 token 오류")
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
            print("firebase token 오류")
            raise HTTPException(status_code=400, detail="firebase에서 id_token을 가져오는 것에 실패했습니다.")

        decoded_token = auth.verify_id_token(firebase_id_token)
        uid = decoded_token["uid"]

        firestore_client = firestore.client()
        user_doc_ref = firestore_client.collection("users").document(uid)
        user_data = user_doc_ref.get()

        if user_data.exists:
            user_info = user_data.to_dict()
            print("기존 사용자 로그인 성공")
            return { "message": "기존 사용자 로그인 성공!" }
        else:
            return { "message": "로그인 성공!" }

    except requests.exceptions.RequestException as e:
        raise HTTPException(status_code=400, detail=f"구글 계정 오류: {str(e)}")
    except auth.InvalidIdTokenError:
        raise HTTPException(status_code=400, detail="구글 token 오류")

@router.post("/sign-up")
async def sign_up(user_data: UserData):
    try:
        try:
            auth.get_user(user_data.uid)
        except auth.UserNotFoundError:
            print("등록되지 않은 사용자")
            raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")

        user_ref = db.reference(f"/users/{user_data.uid}")
        existing_user = user_ref.get()

        if existing_user:
            print("이미 등록된 사용자")
            raise HTTPException(status_code=400, detail="이미 등록된 사용자입니다.")

        user_ref.set({
            "nickname": user_data.nickname,
            "interest_genre": user_data.interest_genre,
            "level": user_data.level
        })
        print("회원가입 완료")
        return {"message": "회원가입 완료"}
    
    except HTTPException as e:
        raise e
    except Exception as e:
        print("회원가입 실패")
        raise HTTPException(status_code=500, detail=f"회원가입 실패: {str(e)}")

@router.put("/edit-user")
async def edit_user(user_data: UserData):
    try:
        try:
            auth.get_user(user_data.uid)
        except auth.UserNotFoundError:
            print("등록되지 않은 사용자")
            raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
        
        user_ref = db.reference(f"/users/{user_data.uid}")
        existing_user = user_ref.get()

        if not existing_user:
            print("해당 유저가 존재하지 않음")
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")

        user_ref.update({
            "nickname": user_data.nickname,
            "interest_genre": user_data.interest_genre,
            "level": user_data.level
        })
        print("유저 정보 수정 완료")
    
    except HTTPException as e:
        raise e
    except Exception as e:
        print("무슨오류지")
        raise HTTPException(status_code=500, detail=e)
    
@router.delete("/delete-user/{uid}")
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

        firestore_client = firestore.client()
        user_doc_ref = firestore_client.collection("users").document(uid)
        if user_doc_ref.get().exists:
            user_doc_ref.delete()
        
        print("사용자 삭제 완료")
        return {"message": "사용자 삭제 완료"}

    except HTTPException as e:
        raise e
    except Exception as e:
        print("사용자 삭제 실패")
        raise HTTPException(status_code=500, detail=f"사용자 삭제 실패: {str(e)}")