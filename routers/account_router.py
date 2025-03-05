from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from firebase_admin import auth
from manager.firebase_manager import db

router = APIRouter()

class GoogleLoginRequest(BaseModel):
    id_token: str

@router.post("/google-login")
async def google_login(request: GoogleLoginRequest):
    try:
        decoded_token = auth.verify_id_token(request.id_token)
        uid = decoded_token["uid"]
        email = decoded_token.get("email", "")
        name = decoded_token.get("name", "")

        user_ref = db.collection("users").document(uid)
        user_ref.set({
            "uid": uid,
            "email": email,
            "name": name
        }, merge=True)

        return {"message": "User authenticated", "uid": uid}
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@router.get("/user/{uid}")
async def get_user(uid: str):
    try:
        user_ref = db.collection("users").document(uid)
        user_doc = user_ref.get()
        if user_doc.exists:
            return user_doc.to_dict()
        else:
            raise HTTPException(status_code=404, detail="User not found")
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))