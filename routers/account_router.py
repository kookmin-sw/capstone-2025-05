from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
from manager.firebase_manager import firestore_db, storage_bucket
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

@router.get("/google-login", tags=["Sign"])
async def google_login():
    redirect_uri = os.getenv('GOOGLE_REDIRECT_URI', 'http://localhost:8000/google-auth-callback') 
    google_auth_url = (
        f"{GOOGLE_OAUTH2_URL}?response_type=code&"
        f"client_id={os.getenv('CLIENT_ID')}&"
        f"redirect_uri={redirect_uri}&" 
        f"scope=openid profile email"
    )
    return RedirectResponse(url=google_auth_url)

@router.get("/google-auth-callback", tags=["Sign"])
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

        return {
            "message" : "로그인 성공", # json 형태로 프론트로 전송, todo: 바뀔 수도
            "uid" : uid
        }

    except requests.exceptions.RequestException as e:
        print("구글 계정 오류")
        raise HTTPException(status_code=400, detail=f"구글 계정 오류: {str(e)}")
    except auth.InvalidIdTokenError:
        print("구글 토큰 오류")
        raise HTTPException(status_code=400, detail="구글 token 오류")

@router.post("/sign-up", tags=["Sign"])
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
    
@router.delete("/delete-user/{uid}", tags=["Sign"])
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
        deleted_files = []
        for blob in blobs:
            blob.delete()
            deleted_files.append(blob.name)
        
        if deleted_files:
            print(f"Storage에서 {uid} 폴더 삭제 완료 (파일 개수: {len(deleted_files)})")
        else:
            print("Storage에서 삭제할 파일이 없습니다.")

        print("사용자 삭제 완료")
        return {"message": "사용자 삭제 완료"}

    except HTTPException as e:
        raise e
    except Exception as e:
        print(f"사용자 삭제 실패: {str(e)}")
        raise HTTPException(status_code=500, detail=f"사용자 삭제 실패: {str(e)}")