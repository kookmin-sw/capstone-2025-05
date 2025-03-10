from fastapi import APIRouter, HTTPException
from fastapi.responses import RedirectResponse
import os
import requests
from firebase_admin import auth, db
from pydantic import BaseModel
from typing import List

router = APIRouter()

GOOGLE_OAUTH2_URL = "https://accounts.google.com/o/oauth2/auth"
GOOGLE_TOKEN_URL = "https://oauth2.googleapis.com/token"
GOOGLE_USERINFO_URL = "https://www.googleapis.com/oauth2/v3/userinfo"

class UserData(BaseModel):
    uid: str
    nickname: str
    interest_genre: List[int]  # 관심 장르 복수 선택 가능 (0~6)
    level: int  # 실력 (0~3)

@router.post("/sign-up")
async def sign_up(user_data: UserData):
    try:
        try:
            auth.get_user(user_data.uid)
        except auth.UserNotFoundError:
            raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
            print("등록되지 않은 사용자")

        user_ref = db.reference(f"/users/{user_data.uid}")
        existing_user = user_ref.get()

        if existing_user:
            print("이미 등록된 사용자입니다.")

        user_ref.set({
            "nickname": user_data.nickname,
            "interest_genre": user_data.interest_genre,
            "level": user_data.level
        })
        print("데이터 저장 성공") # 서버 로그 확인용
    
    except HTTPException as e:
        raise e
    except Exception as e:
        raise HTTPException(status_code=500, detail=e)
        print("무슨오류지?")