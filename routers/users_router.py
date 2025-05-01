from fastapi import APIRouter, HTTPException
from firebase_admin import auth, db

router = APIRouter()

@router.get("/users")
async def get_all_users():
    try:
        users = []
        page = auth.list_users()
        while page:
            for user in page.users:
                uid = user.uid
                email = user.email
                nickname_ref = db.reference(f"users/{uid}/nickname")
                nickname = nickname_ref.get()
                users.append({
                    "uid": uid,
                    "email": email,
                    "nickname": nickname
                })
            page = page.get_next_page()
        return users
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@router.get("/users-one")
async def get_user_by_uid(uid: str):
    try:
        user = auth.get_user(uid)
        nickname_ref = db.reference(f"users/{uid}/nickname")
        nickname = nickname_ref.get()
        return {
            "uid": user.uid,
            "email": user.email,
            "nickname": nickname
        }
    except auth.UserNotFoundError:
        raise HTTPException(status_code=404, detail="User not found")
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))