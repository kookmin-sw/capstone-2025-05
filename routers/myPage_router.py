from fastapi import APIRouter, HTTPException
from firebase_admin import auth, db, storage
from typing import List
from fastapi import APIRouter, HTTPException, UploadFile, File
from manager.firebase_manager import firestore_db, storage_bucket
from urllib.parse import urlparse
import uuid

router = APIRouter()

@router.put("/edit-user/nickname", tags=["My Page"])
async def edit_nickname(uid: str, nickname: str):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")
        
        if not user_ref.get():
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
        
        user_ref.update({"nickname": nickname})
        print("닉네임 변경 완료")
        return { "message": "닉네임 변경 완료" }

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("원인불명 닉네임 변경 실패")
        raise HTTPException(status_code=500, detail=f"닉네임 변경 실패: {str(e)}")


@router.put("/edit-user/interest-genre", tags=["My Page"])
async def edit_interest_genre(uid: str, interest_genre: List[int]):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")
        
        if not user_ref.get():
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
        
        user_ref.update({"interest_genre": interest_genre})
        print("관심 장르 변경 완료")
        return { "message": "관심 장르 변경 완료" }

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("원인불명 관심 장르 변경 실패")
        raise HTTPException(status_code=500, detail=f"관심 장르 변경 실패: {str(e)}")

@router.put("/edit-user/level", tags=["My Page"])
async def edit_level(uid: str, level: int):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")
        
        if not user_ref.get():
            raise HTTPException(status_code=400, detail="해당 사용자가 존재하지 않습니다.")
        
        user_ref.update({"level": level})
        print("실력 변경 완료")
        return { "message": "실력 변경 완료" }

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print("원인불명 실력 변경 실패")
        raise HTTPException(status_code=500, detail=f"실력 변경 실패: {str(e)}")

@router.post("/change-profile-image", tags=["My Page"])
async def change_profile_image(uid: str, file: UploadFile = File(...)):
    try:
        auth.get_user(uid)
        user_ref = db.reference(f"/users/{uid}")

        storage_bucket = storage.bucket()
        blobs = storage_bucket.list_blobs(prefix=f"{uid}/profile/")

        deleted_files: List[str] = []
        for blob in blobs:
            blob.delete()
            deleted_files.append(blob.name)
        
        if deleted_files:
            print("기존 프로필 삭제 완료")
        else:
            print("삭제할 기존 프로필 사진이 없습니다.")

        file_extension = file.filename.split(".")[-1].lower()
        if file_extension not in ["jpg", "jpeg", "png"]:
            print("지원하지 않는 파일 형식 (jpg, jpeg, png만 가능)")
            raise HTTPException(status_code=400, detail="지원하지 않는 파일 형식입니다. (jpg, jpeg, png만 가능)")

        unique_filename = f"{uuid.uuid4()}.{file_extension}"
        storage_path = f"{uid}/profile/{unique_filename}"

        blob = storage_bucket.blob(storage_path)
        blob.upload_from_file(file.file, content_type=file.content_type)

        new_profile_url = blob.public_url
        user_ref.update({"profile_image": new_profile_url})

        print("프로필 사진 변경 완료")
        return {"message": "프로필 사진 변경 완료", "profile_image_url": new_profile_url}

    except auth.UserNotFoundError:
        print("등록되지 않은 사용자")
        raise HTTPException(status_code=400, detail="등록되지 않은 사용자입니다.")
    except Exception as e:
        print(f"원인 불명 프로필 사진 변경 실패: {str(e)}")
        raise HTTPException(status_code=500, detail=f"프로필 사진 변경 실패: {str(e)}")