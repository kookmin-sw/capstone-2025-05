from fastapi import FastAPI, APIRouter, Form, File, UploadFile, HTTPException, Query
from starlette.requests import Request
from fastapi.responses import HTMLResponse
from pydantic import BaseModel
from templates import templates
from fastapi.staticfiles import StaticFiles
from templates import templates
from typing import List
from starlette.middleware.cors import CORSMiddleware
import datetime
from manager.post_model import Post, Comment
from fastapi.responses import JSONResponse
import socket
import time
from threading import Thread
import threading
from typing import Optional
from fastapi import UploadFile, File
import os
from fastapi import UploadFile
from typing import Optional
import base64
import uvicorn
from firebase_admin.firestore import SERVER_TIMESTAMP

import json

import firebase_admin
from firebase_admin import firestore, db, storage
from firebase_admin import credentials, initialize_app
import json
from google.cloud.firestore_v1 import FieldFilter
from datetime import timedelta
from dotenv import load_dotenv
from manager.firebase_manager import firestore_db, storage_bucket

posts_ref = firestore_db.collection("post")
comments_ref=firestore_db.collection("comment")
reports_ref = firestore_db.collection("reports")
scrap_ref = firestore_db.collection("scrap")
users_ref = firestore_db.collection("users")
load_dotenv()

router = APIRouter()

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
STATIC_DIR = os.path.join(BASE_DIR,'static/')
IMG_DIR = os.path.join(STATIC_DIR,'images/')
SERVER_IMG_DIR = os.path.join('http://localhost:8000/','static/','images/')
UPLOAD_IMAGE_DIR = "static/images"
UPLOAD_AUDIO_DIR = "static/audio"

os.makedirs(UPLOAD_IMAGE_DIR, exist_ok = True)
os.makedirs(UPLOAD_AUDIO_DIR, exist_ok = True)
ADMIN_UID = os.getenv("ADMIN_UID")

async def get_user_profile_image(uid: str):
    bucket = storage.bucket()
    blob = bucket.blob(f"profile_images/{uid}.jpg")
    if blob.exists():
        return blob.generate_signed_url(datetime.timedelta(seconds=300), method='GET')
    return None
@router.get("/", response_class=JSONResponse, tags=["Post"])
async def root(request: Request):
    try:
        postsdictlist = []
        alldocs = posts_ref.order_by("created_at", direction=firestore.Query.DESCENDING).stream()

        for doc in alldocs:
            datadict = doc.to_dict()
            for key, value in datadict.items():
                if isinstance(value, datetime.datetime):
                    datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
                else:
                    datadict[key] = str(value)
            # image_url과 audio_url도 포함되도록 그대로 추가
            postsdictlist.append({
                "id": datadict.get("id"),
                "제목": datadict.get("제목"),
                "내용" : datadict.get("내용"),
                "작성자": datadict.get("작성자"),
                "uid" : datadict.get("uid"),
                "조회수": datadict.get("조회수"),
                "좋아요수" : datadict.get("좋아요수"),
                "댓글수": datadict.get("댓글수"),
                "created_at": datadict.get("created_at"),
                "image_url": datadict.get("image_url", ""),
                "audio_url": datadict.get("audio_url", "")
            })

        return JSONResponse(content={ "posts": postsdictlist })

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"목록 가져오기 실패: {str(e)}")

@router.put("/posts/{post_id}/views", response_class=JSONResponse, tags=["Post"])
async def increase_views(post_id: int):
    string_number = str(post_id).zfill(8)
    post_ref = posts_ref.document(string_number)
    post = post_ref.get()

    if not post.exists:
        return {"error": "Post not found"}
    post_data = post.to_dict()

    new_views = post_data.get("조회수", 0) + 1
    post_ref.set({"조회수": new_views}, merge=True)

    post_data["조회수"] = new_views
    return post_data

@router.get("/posts/top-viewed", response_class=JSONResponse, tags=["Post"])
async def get_top_viewed_posts():
    postsdictlist = []
    alldocs = posts_ref.stream()

    for doc in alldocs:
        datadict = doc.to_dict()
        if '조회수' not in datadict:
            datadict['조회수'] = 0
        postsdictlist.append(datadict)
    postsdictlist.sort(key=lambda x: x.get('조회수', 0), reverse=True)

    for post in postsdictlist:
        for key, value in post.items():
            if isinstance(value, datetime.datetime):
                post[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
    return JSONResponse(content={"posts": postsdictlist})

@router.get("/posts/{post_id}", response_class=JSONResponse, tags=["Post"])
async def read_post(request: Request, post_id: int):
    context = {'post_id': post_id}
    postkeyvaluedict = []

    alldocs = posts_ref.stream()
    for doc in alldocs:
        datadict = doc.to_dict()
        if datadict.get("id") == post_id:
            for key, value in datadict.items():
                if isinstance(value, datetime.datetime):
                    datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
            postkeyvaluedict.append({
                "uid": datadict.get("uid"),
                "id": datadict.get("id"),
                "좋아요수": datadict.get("좋아요수"),
                "조회수": datadict.get("조회수"),
                "댓글수": datadict.get("댓글수"),
                "제목": datadict.get("제목"),
                "내용": datadict.get("내용"),
                "작성자": datadict.get("작성자"),
                "created_at": datadict.get("created_at"),
                "image_url": datadict.get("image_url", ""),
                "audio_url": datadict.get("audio_url", "")
            })
            break

    if not postkeyvaluedict:
        context['postexist'] = "no"
    else:
        context['postexist'] = "yes"

        comments_keyvaluelists = []
        alldocs = comments_ref.stream()
        for doc in alldocs:
            datadict = doc.to_dict()
            if datadict.get("postid") == post_id:
                for key, value in datadict.items():
                    if isinstance(value, datetime.datetime):
                        datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
                comments_keyvaluelists.append(datadict)

        context['keyvaluedict'] = postkeyvaluedict
        context['comments_keyvaluelists'] = comments_keyvaluelists

    return JSONResponse(content=context)

async def upload_file_to_firebase(file:UploadFile, path: str) -> Optional[str]:
    try:
        from firebase_admin import storage
        bucket = storage.bucket()
        blob = bucket.blob(path)
        file.file.seek(0)
        blob.upload_from_file(file.file, content_type=file.content_type)
        blob.make_public()
        return blob.public_url
    except Exception as e:
        print(f"[Firebase Storage Upload Error] {e}")
        return None

@router.post("/posts", tags=["Post"])
async def create_post(
    uid : str = Form(),
    content : str= Form(),
    author : str = Form(),
    title : str = Form(),
    password: str = Form(),
    image: Optional[UploadFile] = File(default=None), 
    audio: Optional[UploadFile] = File(default=None)):
    db = firestore.client()
    now = datetime.datetime.now()
    alldbidlist = sorted([doc.to_dict().get("id") for doc in posts_ref.stream() if doc.to_dict().get("id") is not None])

    # 삭제된 게시글 ID 추적 (예시: Firestore에서 삭제된 게시글을 is_deleted 필드로 구분)
    deleted_ids = sorted([doc.id for doc in posts_ref.where("is_deleted", "==", True).stream()])

    new_id = 1
    if deleted_ids:
        new_id = deleted_ids.pop(0)  # 삭제된 ID가 있다면 그것을 사용
    else:
        # 모든 ID 리스트에서 빈 자리를 찾아서 새로운 ID를 설정
        for existing_id in alldbidlist:
            if existing_id != new_id:
                break
            new_id += 1
        
    string_number = str(new_id).zfill(8)
    image_url = None
    if image:
        print(f"image_url: {image_url}")
        image_path = f"post/image/{uid}/{string_number}_{image.filename}"
        image_url = await upload_file_to_firebase(image, image_path)

    audio_url = None
    if audio:
        print(f"audio_url : {audio_url}")
        audio_path = f"post/audio/{uid}/{string_number}_{audio.filename}"
        audio_url = await upload_file_to_firebase(audio, audio_path)

    post_data = {
        "id": new_id,
        "uid": str(uid),
        "내용": content,
        "댓글갯수": 0,
        "created_at": now,
        "작성자": author,
        "제목": title,
        "조회수": 0,
        "좋아요수": 0,
        "비밀번호" : password,
        "image_url": image_url,
        "audio_url": audio_url
    }

    posts_ref.document(string_number).set(post_data)

    firestore.client().collection("my_activity").document(str(uid)).collection("post").document(string_number).set({
        "post_id": string_number,
        "제목": title,
        "created_at": now.isoformat()
    })

    return {"result_msg": "게시글이 성공적으로 등록되었습니다.", "post_id": string_number}
from urllib.parse import unquote, urlparse, parse_qs

async def extract_storage_path_from_url(url:str) -> str:
    try:
        parsed_url = urlparse(url)

        # 형식 1: https://firebasestorage.googleapis.com/v0/b/BUCKET/o/ENCODED_PATH?alt=media...
        if "firebasestorage.googleapis.com" in parsed_url.netloc:
            if "/o/" in parsed_url.path:
                encoded_path = parsed_url.path.split("/o/")[1]
                return unquote(encoded_path)

        # 형식 2: https://storage.googleapis.com/BUCKET/post/image/...
        if "storage.googleapis.com" in parsed_url.netloc:
            parts = parsed_url.path.split("/", 2)  # ['', bucket, actual path]
            if len(parts) >= 3:
                return unquote(parts[2])

        raise ValueError("Unsupported Firebase Storage URL format")

    except Exception as e:
        print(f"[extract_storage_path_from_url ERROR]: {e}")
        return ""
async def delete_file_from_firebase(path: str):
    from firebase_admin import storage
    bucket = storage.bucket()
    blob = bucket.blob(path)
    try:
        blob.delete()
        print(f"[Storage] Deleted: {path}")
    except Exception as e:
        print(f"[Storage] Delete failed: {path} -> {e}")
@router.put("/posts/{post_id}", tags=["Post"])
async def modify_post(
    post_id: int,
    title: Optional[str] = Form(...),
    content: Optional[str] = Form(...),
    image: Optional[UploadFile] = File(None),
    audio: Optional[UploadFile] = File(None)
):
    db = firestore.client()
    postid = str(post_id)
    string_number = postid.zfill(8)
    updated_at = datetime.datetime.utcnow()

    post_doc = posts_ref.document(string_number).get()
    if not post_doc.exists:
        raise HTTPException(status_code=404, detail="해당 게시글을 찾을 수 없습니다.")

    post_data = post_doc.to_dict()
    uid = post_data.get("uid")

    update_data = {}
    if title is not None:
        update_data["제목"] = title
    if content is not None:
        update_data["내용"] = content
    update_data["created_at"] = updated_at
    if image:
        old_image_url = post_data.get("image_url")
        if old_image_url:
            path = await extract_storage_path_from_url(old_image_url)
            await delete_file_from_firebase(path)

        image_path = f"post/image/{uid}/{string_number}_{image.filename}"
        image_url = await upload_file_to_firebase(image, image_path)
        update_data["image_url"] = image_url
    if audio:
        old_audio_url = post_data.get("audio_url")
        if old_audio_url:
            path = await extract_storage_path_from_url(old_audio_url)
            await delete_file_from_firebase(path)
        audio_path = f"post/audio/{uid}/{string_number}_{audio.filename}"
        audio_url = await upload_file_to_firebase(audio, audio_path)
        update_data["audio_url"] = audio_url
    posts_ref.document(string_number).update(update_data)

    post_doc = posts_ref.document(string_number).get()
    if post_doc.exists:
        post_data = post_doc.to_dict()
        uid = post_data.get("uid")
        db.collection("my_activity").document(uid).collection("post").document(string_number).update({
            "제목": title,
            "created_at": updated_at.isoformat()
        })

    return {'result_msg': f"{postid} updated..."}
async def delete_all_user_files_from_firebase(uid: str):
    from firebase_admin import storage

    bucket = storage.bucket()
    prefix = f"post/{uid}/"
    blobs = bucket.list_blobs(prefix=prefix)

    try:
        for blob in blobs:
            print(f"[Storage] Deleting: {blob.name}")
            blob.delete()
        print(f"[Storage] All files under 'post/{uid}/' deleted.")
    except Exception as e:
        print(f"[Storage] Failed to delete all user files under {prefix} -> {e}")

@router.delete("/posts/{post_id}", tags=["Post"])
async def delete_post(post_id: int, uid:str = Query(...)):
    global posts_ref, comments_ref
    print("delete_post >>>")
    db = firestore.client()

    string_number = str(post_id)
    dbkey = string_number.zfill(8)

    post_ref = posts_ref.document(dbkey)
    if not post_ref.get().exists:
        raise HTTPException(status_code=404, detail="게시글을 찾을 수 없습니다.")
    
    post_data = post_ref.get().to_dict()
    if post_data.get("uid") != uid:
        raise HTTPException(status_code=403, detail="본인의 게시글만 삭제할 수 있습니다.")

    image_url = post_data.get("image_url")
    audio_url = post_data.get("audio_url")
    if image_url:
        await delete_file_from_firebase(image_url)
    if audio_url:
        await delete_file_from_firebase(audio_url)
    post_ref.delete()
    if image_url or audio_url:
        await delete_all_user_files_from_firebase(uid)

    alldocs = comments_ref.stream()
    for doc in alldocs:
        docdict = doc.to_dict()
        if docdict.get("postid") == post_id:
            comments_ref.document(doc.id).delete()
    db.collection("my_activity").document(uid).collection("post").document(string_number).delete()

    for pid in range(post_id + 1, 100000):
        current_id = str(pid).zfill(8)
        next_post = posts_ref.document(current_id).get()
        if not next_post.exists:
            break
        post_data = next_post.to_dict()
        new_id = str(pid-1).zfill(8)
        post_data["id"] = pid-1
        posts_ref.document(new_id).set(post_data)
        posts_ref.document(current_id).delete()

        for doc in comments_ref.stream():
            comment = doc.to_dict()
            if comment.get("postid") == pid:
                comments_ref.document(doc.id).update({"postid": pid-1})
        user_id = post_data.get("uid")
        if user_id:
            activity_ref = db.collection("my_activity").document(user_id).collection("post")
            activity_ref.document(new_id).set(activity_ref.document(current_id).get().to_dict())
            activity_ref.document(current_id).delete()
    return {'result_msg' : f"Post {post_id} and its comments deleted."}

@router.delete("/posts/admin/{post_id}", tags=["Post"])
async def delete_post_admin(post_id: int, uid:str = Query(...)):

    if uid != ADMIN_UID:
        raise HTTPException(status_code=403, detail="삭제 권한이 없습니다.")
    db=firestore.client()
    string_number = str(post_id)
    dbkey = string_number.zfill(8)

    post_ref = posts_ref.document(dbkey)
    post_doc = post_ref.get()
    if not post_ref.get().exists:
        raise HTTPException(status_code=404, detail="게시글을 찾을 수 없습니다.")
    
    post_data = post_ref.get().to_dict()
    user_id = post_data.get("uid")
    
    image_url = post_data.get("image_url")
    audio_url = post_data.get("audio_url")
    if image_url:
        await delete_file_from_firebase(image_url)
    if audio_url:
        await delete_file_from_firebase(audio_url)
    post_ref.delete()
    if image_url or audio_url:
        await delete_all_user_files_from_firebase(user_id)

    if user_id:
        db.collection("my_activity").document(user_id).collection("post").document(dbkey).delete()

    # 댓글 삭제
    for doc in comments_ref.stream():
        comment = doc.to_dict()
        if comment.get("postid") == post_id:
            comments_ref.document(doc.id).delete()

    # ID 당기기
    for pid in range(post_id + 1, 100000):
        current_id = str(pid).zfill(8)
        next_post = posts_ref.document(current_id).get()
        if not next_post.exists:
            break

        post_data = next_post.to_dict()
        new_id = str(pid - 1).zfill(8)
        post_data["id"] = pid-1
        posts_ref.document(new_id).set(post_data)
        posts_ref.document(current_id).delete()

        # 댓글들의 postid 업데이트
        for doc in comments_ref.stream():
            comment = doc.to_dict()
            if comment.get("postid") == pid:
                comments_ref.document(doc.id).update({"postid": pid - 1})

        # my_activity 문서 이동
        uid_of_post = post_data.get("uid")
        if uid_of_post:
            activity_ref = db.collection("my_activity").document(uid_of_post).collection("post")
            activity_data = activity_ref.document(current_id).get().to_dict()
            if activity_data:
                activity_ref.document(new_id).set(activity_data)
                activity_ref.document(current_id).delete()

    return {"result_msg": f"{post_id}번 게시글이 삭제되었고 이후 게시글들이 정리되었습니다."}
@router.delete("/comments/admin/{comment_id}", tags=["Comment"])
async def delete_comment(comment_id: str, uid:str = Query(...)):
    if uid != ADMIN_UID:
        raise HTTPException(status_code=403, detail="삭제 권한이 없습니다.")

    comment_ref = comments_ref.document(comment_id)
    if not comment_ref.get().exists:
        raise HTTPException(status_code=404, detail="댓글을 찾을 수 없습니다.")
    comment_ref.delete()
    return {"result_msg" : f"{comment_id}번 댓글이 삭제됐습니다."}

@router.post("/comment", tags=["Comment"])
async def create_comment(comment: Comment):
    global posts_ref, comments_ref

    print("create_comment >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

    comment_data = comment.dict()
    now = datetime.datetime.utcnow()
    comment_data["date"] = SERVER_TIMESTAMP

    # 프로필 이미지 처리
    # 클라이언트에서 직접 받은 값을 사용
    comment_data["profile_image"] = comment_data.get("프로필이미지") or comment_data.get("profile_image")


    try:
        db = firestore.client()
        transaction = db.transaction()

        # 클로저 내부에 comment_data를 캡처
        @firestore.transactional
        def run_transaction(transaction):
            try:
                comments_snapshot = comments_ref.order_by("id", direction=firestore.Query.DESCENDING).limit(1).stream()
                last_comment_id = 0
                for doc in comments_snapshot:
                    last_comment_id = doc.to_dict().get("id", 0)
                new_comment_id = last_comment_id + 1
                comment_data["id"] = new_comment_id

                comment_doc_ref = comments_ref.document(str(new_comment_id).zfill(8))
                transaction.set(comment_doc_ref, comment_data)

                post_ref = posts_ref.document(str(comment_data["postid"]).zfill(8))
                transaction.update(post_ref, {"댓글갯수": firestore.Increment(1)})

                return new_comment_id
            except Exception as inner_e:
                print("[TRANSACTION ERROR]", inner_e)
                raise

        new_comment_id = run_transaction(transaction)
    except Exception as e:
        import traceback
        print("[ERROR] create_comment 실패")
        traceback.print_exc()
        print("comment_data", comment_data)
        raise HTTPException(status_code=500, detail="댓글 작성 중 오류 발생")
    # 활동 기록 처리
    if comment_data.get("uid"):
        try:
            uid = comment_data["uid"]
            post_id_str = str(comment_data["postid"]).zfill(8)
            firestore.client().collection("my_activity").document(uid).collection("comment").document(
                str(new_comment_id).zfill(8)
            ).set({
                "post_id": post_id_str,
                "date": now.isoformat()
            })
        except Exception as e:
            print(f"Activity logging error: {e}")

    return {'result_msg': 'put Registered...'}


@router.put("/comment/{comment_id}", tags=["Comment"])
async def modify_post(comment: Comment, comment_id : int):

    print("modify_post >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

    commentid = str(comment_id)
    string_number=commentid.zfill(8)

    toupdate_commentcontent=comment.내용
    updated_at = datetime.datetime.utcnow()

    comments_ref.document(string_number).update({
    
        "내용": toupdate_commentcontent,
        "date": updated_at
    })
    existing_comment = comments_ref.document(string_number).get()
    if existing_comment.exists:
        comment_data = existing_comment.to_dict()
        uid = comment_data.get("uid")
        if uid:
            firestore.client().collection("my_activity").document(uid).collection("comment").document(string_number).update({
                "내용": toupdate_commentcontent,
                "date": updated_at.isoformat()
            })

    return {'result_msg': f"{commentid} updated..."}

@router.delete("/comment/{comment_id}", tags=["Comment"])
async def delete_comment(comment_id: int):
    global posts_ref, comments_ref
    print("delete_post >>>")

    string_number = str(comment_id).zfill(8)

    comment_ref = comments_ref.document(string_number)
    comment_doc = comment_ref.get()

    if not comment_doc.exists:
        return {'result_msg' : f"ID {comment_id}의 댓글을 찾을 수 없습니다."}
    comment_data = comment_doc.to_dict()
    post_id = comment_data.get('postid')
    uid = comment_data.get('uid')
    postkey = str(post_id).zfill(8)
    if post_id is None:
        return {'result_msg' : "이 댓글에 연결된 게시글이 없습니다."}
    comment_ref.delete()
    print(f"댓글 {comment_id} 삭제 완료")

    post_ref = posts_ref.document(str(postkey))
    post_doc = post_ref.get()

    if post_doc.exists:
        post_data = post_doc.to_dict()
        current_comment_count = post_data.get('댓글갯수', 0)

        print(f"현재 댓글 갯수: {current_comment_count}")

        if isinstance(current_comment_count, int) and current_comment_count > 0:
            new_comment_count = current_comment_count - 1
            post_ref.update({"댓글갯수": new_comment_count})
            print(f"게시글 {post_id}의 댓글 갯수 업데이트 완료 : {new_comment_count}")

            updated_doc = post_ref.get()
            updated_count = updated_doc.to_dict().get("댓글갯수")
            print(f"업데이트된 댓글갯수: {updated_count}")
    if uid:
        firestore.client().collection("my_activity").document(uid).collection("comment").document(string_number).delete()
        
    return {'result_msg' : f"댓글 {comment_id} 삭제 성공"}


@router.post("/posts/{post_id}/like", tags=["Post"])
async def like_post(post_id: int, uid: str, request: Request):
    db = firestore.client()

    if not uid:
        raise HTTPException(status_code=400, detail="User UID is required")

    string_number = str(post_id).zfill(8)
    
    like_doc_ref = db.collection("my_activity").document(uid).collection("like").document(string_number)
    post_doc_ref = db.collection("post").document(string_number)

    if like_doc_ref.get().exists:
        raise HTTPException(status_code=400, detail="이미 좋아요한 게시글입니다.")

    post_doc = post_doc_ref.get()
    if not post_doc.exists:
        raise HTTPException(status_code=404, detail="게시글이 존재하지 않습니다.")

    post_doc_ref.update({"좋아요수": firestore.Increment(1)})

    like_doc_ref.set({
        "user_id" : uid,
        "post_id": string_number,
        "like_date": datetime.datetime.utcnow()
    })

    return {"result_msg": "좋아요가 추가되었습니다.", "post_id": post_id}


@router.delete("/posts/{post_id}/like", tags=["Post"])
async def unlike_post(post_id: int, uid: str):

    db = firestore.client()

    if not uid:
        raise HTTPException(status_code=400, detail="User UID is required")
    string_number = str(post_id).zfill(8)

    post_ref = db.collection("post").document(string_number)
    post = post_ref.get()
    if not post.exists:
        raise HTTPException(status_code=404, detail="Post not found")

    like_doc = db.collection("my_activity").document(uid).collection("like").document(string_number)
    if not like_doc.get().exists:
        raise HTTPException(status_code=400, detail="좋아요하지 않은 게시글입니다.")

    current_likes = post.to_dict().get("좋아요수", 0)
    if current_likes > 0:
        post_ref.update({"좋아요수": firestore.Increment(-1)})

    like_doc.delete()

    return {"message": "좋아요가 취소되었습니다.", "post_id": post_id}


@router.post("/posts/{post_id}/scrap", tags=["Post"])
async def scrap_post(post_id: str, uid: str):
    db = firestore.client()

    if not uid:
        raise HTTPException(status_code=400, detail="User UID is required")
    string_number = post_id.zfill(8)
    post_ref = db.collection("post").document(string_number)
    post = post_ref.get()
    if not post.exists:
        raise HTTPException(status_code=404, detail="Post not found")

    scrap_doc = db.collection("my_activity").document(uid).collection("scrap").document(post_id)
    if scrap_doc.get().exists:
        raise HTTPException(status_code=400, detail="이미 스크랩한 게시글입니다.")
        
    post_ref.update({"조회수": firestore.Increment(1)})

    scrap_doc.set({
        "user_id": uid,
        "post_id": post_id,
        "scrap_date": datetime.datetime.utcnow()
    })

    return {"message": "스크랩이 추가되었습니다.", "post_id": post_id}


@router.delete("/posts/{post_id}/scrap", tags=["Post"])
async def remove_scrap(post_id: str, uid: str):
    db = firestore.client()

    if not uid:
        raise HTTPException(status_code=400, detail="User UID is required")

    scrap_doc = db.collection("my_activity").document(uid).collection("scrap").document(post_id)
    if not scrap_doc.get().exists:
        raise HTTPException(status_code=404, detail="스크랩하지 않은 게시글입니다.")

    post_ref = db.collection("post").document(post_id)
    post = post_ref.get()
    if post.exists:
        post_ref.update({"조회수": firestore.Increment(-1)})

    scrap_doc.delete()
    return {"message": "스크랩이 취소되었습니다.", "post_id": post_id}


@router.post("/report/post/{post_id}", tags=["Post"])
async def report_post(post_id : int, reason: str):
    global reports_ref
    print("report_post >>")

    report_data = {
        "type": "post",
        "post_id" : post_id,
        "reason": reason,
        "timestamp": datetime.datetime.now()
    }

    reports_ref.add(report_data)
    return {"result_msg": f"Post {post_id} reported for reason: {reason}"}

@router.post("/report/comment/{comment_id}", tags=["Comment"])
async def report_comment(comment_id: int, reason: str):
    global reports_ref
    print("report_comment >>>")
    report_data = {
        "type": "comment",
        "comment_id": comment_id,
        "reason": reason,
        "timestamp": datetime.datetime.now()
    }
    reports_ref.add(report_data)
    return {"result_msg": f"Comment {comment_id} reported for reason: {reason}"}


@router.get("/search", tags=["Post"])
def search_post(
    query: str = Query(..., description="검색어 입력")
):
    try:
        query_lower = query.lower()

        docs = list(posts_ref.stream())

        results = []
        for doc in docs:
            data = doc.to_dict()
            title = data.get("제목", "")
            content = data.get("내용", "")

            if query_lower in title.lower() or query_lower in content.lower():
                results.append({
                    "id": doc.id,
                    "title": title,
                    "content": content,
                    "created_at": data.get("date", ""),
                })

        return {"results": results}

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"검색 실패: {str(e)}")

@router.get("/autocomplete", tags=["Post"])
def autocomplete(query: str = Query(..., description="자동완성 검색어 입력")):
    try:
        query_lower = query.lower()

        docs = list(posts_ref.stream())
        print(f"총 문서수: {len(docs)}")
        print(f"Query: {query_lower}")
        results = []
        for doc in docs:
            data = doc.to_dict()
            title = data.get("제목", "")
            if query_lower in title.lower():  # 부분 일치
                results.append({"title": title})

        return results
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"검색 실패: {str(e)}")

@router.get("/comment/{post_id}", response_class=JSONResponse, tags=["Comment"])
async def read_comment(request: Request, post_id: int):
    
    context = {'post_id': post_id}

    comments_keyvaluelists=[]
    alldocs=comments_ref.stream()
    for doc in alldocs:
        datadict=doc.to_dict()
        if datadict.get("postid")==post_id:
            for key, value in datadict.items():
                if isinstance(value, datetime.datetime):
                    datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
            comments_keyvaluelists.append(datadict)
    if not comments_keyvaluelists:
        context['comments_exist'] = "no"
    else:
        context['comments_exist'] = "yes"
        context['comments_keyvaluelists'] = comments_keyvaluelists
    return JSONResponse(content=context)
