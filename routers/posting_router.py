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

import json

import firebase_admin
from firebase_admin import firestore, db, storage
from firebase_admin import credentials
import json
from google.cloud.firestore_v1 import FieldFilter
from dotenv import load_dotenv
from manager.firebase_manager import firestore_db

storage_bucket = storage.bucket()
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

async def get_user_profile_image(uid: str) -> str:
    bucket = storage.bucket()
    prefix = f"{uid}/profile/"

    blob = list(bucket.list_blobs(prefix=prefix))
    if blobs:
        return blobs[0].generate_signed_url(version="v4", expiration=datetime.timedelta(minutes=15), method="GET")

        return ""
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
                "id": datadict.get("uid"),
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
        if datadict.get("uid") == post_id:
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
        global storage_bucket
        blob = storage_bucket.blob(path)
        file.file.seek(0)
        blob.upload_from_file(file.file, content_type=file.content_type)
        blob.make_public()
        print(f"Uploading to: {path}")
        print(f"Content-Type: {file.content_type}")
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
    alldbidlist = [doc.to_dict().get("id") for doc in posts_ref.stream()]
    alldbidlist = [id for id in alldbidlist if id is not None]
    lastdbid = max(alldbidlist) if alldbidlist else 0
    string_number = str(lastdbid + 1).zfill(8)

    image_url = None
    if image:
        image_path = f"post/image/{string_number}_{image.filename}"
        image_url = await upload_file_to_firebase(image, image_path)

    audio_url = None
    if audio:
        audio_path = f"post/audio/{string_number}_{audio.filename}"
        audio_url = await upload_file_to_firebase(audio, audio_path)

    post_data = {
        "id": lastdbid + 1,
        "uid": int(string_number),
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

    update_data = {}
    if title is not None:
        update_data["제목"] = title
    if content is not None:
        update_data["내용"] = content
    update_data["created_at"] = updated_at
    if image:
        image_path = f"post/image/{string_number}_{image.filename}"
        image_url = await upload_file_to_firebase(image, image_path)
        update_data["image_url"] = image_url
    if audio:
        audio_path = f"post/audio/{string_number}_{audio.filename}"
        audio_url = await upload_file_to_firebase(audio, audio_path)
        update_data["audio_url"] = audio_url
    posts_ref.document(string_number).update(update_data)

    post_doc = posts_ref.document(string_number).get()
    if post_doc.exists:
        db.collection("my_activity").document(uid).collection("post").document(string_number).update({
            "제목": title,
            "created_at": updated_at.isoformat()
        })

    return {'result_msg': f"{postid} updated..."}

@router.delete("/posts/{post_id}", tags=["Post"])
async def delete_post(post_id: int):
    global posts_ref, comments_ref
    print("delete_post >>>")
    db = firestore.client()

    string_number = str(post_id)
    dbkey = string_number.zfill(8)

    posts_ref.document(dbkey).delete()

    alldocs = comments_ref.stream()
    for doc in alldocs:
        docdict = doc.to_dict()
        if docdict["postid"] == post_id:
            comments_ref.document(doc.id).delete()
    db.collection("my_activity").document(uid).collection("post").document(string_number).delete()
    return {'result_msg' : f"Post {post_id} and its comments deleted."}

@router.delete("/posts/admin/{post_id}", tags=["Post"])
async def delete_post(post_id: int, uid:str = Query(...)):
    string_number = str(post_id).zfill(8)

    if uid != ADMIN_UID:
        raise HTTPException(status_code=403, detail="삭제 권한이 없습니다.")

    post_ref = posts_ref.document(string_number)
    if not post_ref.get().exists:
        raise HTTPException(status_code=404, detail="게시글을 찾을 수 없습니다.")
    
    post_ref.delete()

    firestore.client().collection("my_activity").document(uid).collection("post").document(string_number).delete()

    return {"result_msg": f"{post_id}번 게시글이 삭제됐습니다."}
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
    
    print("create_comment >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
    def transaction_create_comment(transaction, db, comment_data):
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

    comment_data= comment.dict()
    now = datetime.datetime.utcnow()
    comment_data["date"] = now
    if comment_data.get("uid"):
        profile_image = await get_user_profile_image(comment_data["uid"])
        if profile_image:
            comment_data["profile_image"] = profile_image
    with firestore_db.transaction() as transaction:
        new_comment_id = transaction_create_comment(transaction, firestore_db, comment_data)
    if comment_data.get("uid"):
        uid = comment_data["uid"]
        post_id_str = str(comment_data["postid"]).zfill(8)
    firestore.client().collection("my_activity").document(uid).collection("comment").document(str(new_comment_id).zfill(8)).set({
        "post_id": post_id_str,
        "date": now.isoformat()
    })

    return { 'result_msg': f'{"put"} Registered...' }

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
async def unlike_post(post_id: int, request: Request):
    user = request.headers.get("uid")
    if not user:
        raise HTTPException(status_code=400, detail="User UID is required")

    string_number = str(post_id).zfill(8)
    like_doc_ref = db.collection("my_activity").document(user).collection("like").document(string_number)
    post_doc_ref = posts_ref.document(string_number)

    if not like_doc.get().exists:
        raise HTTPException(status_code=404, detail="좋아요하지 않은 게시글입니다.")

    post_doc = post_doc_ref.get()
    if not post_doc.exists:
        raise HTTPException(status_code=404, detail="게시글이 존재하지 않습니다.")

    post_doc_ref.update({"좋아요수": firestore.Increment(-1)})
    like_doc_ref.delete()
    return {"result_msg": "좋아요가 취소되었습니다.", "post_id": post_id}

@router.post("/posts/{post_id}/scrap", tags=["Post"])
async def scrap_post(post_id: str, request: Request):
    user = request.headers.get("uid")
    if not user:
        raise HTTPException(status_code=400, detail="User UID is required")

    post_ref = posts_ref.document(post_id)
    post = post_ref.get()
    if not post.exists:
        raise HTTPException(status_code=404, detail="Post not found")

    scrap_doc = db.collection("my_activity").document(user).collection("scrap").document(string_number)
    if scrap_doc.get().exists:
        raise HTTPException(status_code=400, detail="이미 스크랩한 게시글입니다.")
    scrap_doc_ref.set({
        "user_id": user,
        "post_id": post_id,
        "scrap_date": datetime.datetime.utcnow()
    })


    return {"message": "스크랩이 추가되었습니다.", "post_id": post_id}

@router.delete("/posts/{post_id}/scrap", tags=["Post"])
async def remove_scrap(post_id: str, request: Request):
    user = request.headers.get("uid")
    if not user:
        raise HTTPException(status_code=400, detail="User UID is required")

    scrap_doc = db.collection("my_activity").document(user).collection("scrap").document(string_number)
    if not scrap_doc.get().exists:
        raise HTTPException(status_code=404, detail="스크랩하지 않은 게시글입니다.")

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

 
        docs = (
            posts_ref
            .order_by("date", direction="DESCENDING")
            .order_by("title", direction="ASCENDING") 
            .stream()
        )

        results = []
        for doc in docs:
            data = doc.to_dict()
            title = data.get("title", "").lower()
        
            if query_lower in title:
                results.append({
                    "id": doc.id,
                    "title": data.get("title", ""),
                    "content": data.get("내용", ""),
                    "created_at": data.get("date", ""),
                })

        return {"results": results}

    except Exception as e:
        raise HTTPException(status_code=500, detail=f"검색 실패: {str(e)}")
@router.get("/autocomplete", tags=["Post"])
def autocomplete(query: str = Query(..., description="자동완성 검색어 입력")):
    try:
        query_lower = query.lower()
        docs = (
            posts_ref
            .order_by("title")  
            .start_at([query_lower])
            .end_at([query_lower + "\uf8ff"])
            .stream()
        )

        suggestions = [doc.to_dict().get("title", "") for doc in docs]

        return {"suggestions": suggestions}
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"자동완성 실패: {str(e)}")

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
