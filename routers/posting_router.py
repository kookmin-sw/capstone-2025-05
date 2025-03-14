# https://github.com/Bipasori/ProjectCafeCOP/tree/main/FastAPI_MYSQL
from fastapi import FastAPI, APIRouter, Form, File, UploadFile
from starlette.requests import Request
from fastapi.responses import HTMLResponse
from pydantic import BaseModel
from templates import templates
from fastapi.staticfiles import StaticFiles
from templates import templates
from typing import List
from starlette.middleware.cors import CORSMiddleware
import datetime
from model import Post, Comment
from fastapi.responses import JSONResponse
import socket
import time
from threading import Thread
import threading

from fastapi import UploadFile, File
import os

import base64
import uvicorn

import json

import firebase_admin
from firebase_admin import firestore
from firebase_admin import credentials
import json
from google.cloud.firestore_v1 import FieldFilter

from manager.firebase_manager import db

posts_ref = db.collection("post")
comments_ref=db.collection("comment")
reports_ref = db.collection("reports")

#postkeys = postTable.__dict__.keys()
#postkeylist = list(postkeys)
#todeletepostkeylist = ['__module__', '__tablename__', '__doc__', '_sa_class_manager', '__table__', '__init__',
 #                  '__mapper__']
#for i in range(len(todeletepostkeylist)):
 #   postkeylist.remove(todeletepostkeylist[i])


#commentkeys = commentTable.__dict__.keys()
#commentkeylist = list(commentkeys)
#todeletecommentkeylist = ['__module__', '__tablename__', '__doc__', '_sa_class_manager', '__table__', '__init__',
 #                  '__mapper__']
#for i in range(len(todeletecommentkeylist)):
#    commentkeylist.remove(todeletecommentkeylist[i])
posting_router = APIRouter()

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
STATIC_DIR = os.path.join(BASE_DIR,'static/')
IMG_DIR = os.path.join(STATIC_DIR,'images/')
SERVER_IMG_DIR = os.path.join('http://localhost:8000/','static/','images/')
UPLOAD_IMAGE_DIR = "static/images"
UPLOAD_AUDIO_DIR = "static/audio"

os.makedirs(UPLOAD_IMAGE_DIR, exist_ok = True)
os.makedirs(UPLOAD_AUDIO_DIR, exist_ok = True)

# -------------------------------
# --------------홈---------------
# -------------------------------
#전체 목록 가져오기
@posting_router.get("/",response_class=JSONResponse, tags=["Post"])
async def root(request: Request):
    postsdictlist = []
    alldocs = posts_ref.stream()
    for doc in alldocs:
        datadict = doc.to_dict()
        for key, value in datadict.items():
            if isinstance(value, datetime.datetime):
                datadict[key] = value.isoformat()
        postsdictlist.append(datadict)

    postsdictlist.reverse()
    return JSONResponse(content=postsdictlist)
# -------------------------------
# ----------------------------------------------------
# ----------글 추가수정삭제, 댓글 추가삭제------------
# ----------------------------------------------------
#조회수 증가 기능 추가
@posting_router.put("/posts/{post_id}/views", response_class=JSONResponse, tags=["Post"])
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
#조회수 가장 높은 게시글 불러오기
@posting_router.get("/posts/top-viewed", response_class=JSONResponse, tags=["Post"])
async def get_top_viewed_post():
    postsdictlist = []
    topdictlist = []
    alldocs = posts_ref.stream()

    for doc in alldocs:
        datadict = doc.to_dict()
        if '조회수' not in datadict:
            datadict['조회수'] = 0
        postsdictlist.append(datadict)
    postsdictlist.sort(key=lambda x: x.get('조회수', 0), reverse=True)

    top_post = postsdictlist[0] if postsdictlist else {}
    for key, value in top_post.items():
        if isinstance(value, datetime.datetime):
            top_post[key] = value.isoformat()
    topdictlist.append(top_post)
    return JSONResponse(content=topdictlist)
#한 글 상세보기 페이지 (댓글 리스트도 표시해야함)
@posting_router.get("/posts/{post_id}", response_class=JSONResponse, tags=["Post"])
async def read_post(request: Request, post_id: int):
    #Request객체를 JSONResponse에 포함시키려고 하면 문제가 발생함
    #Request객체는 JSON으로 직렬화할 수 없기 때문에 이 객체를 content에 포함시키는 것 자체가 문제가 됨
    context = {'post_id': post_id}

    print("read_postdetail >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

    #이 글의 상세정보 firestore DB에서 가져오기
    postkeyvaluedict=[]
    alldocs=posts_ref.stream()
    for doc in alldocs:
        datadict=doc.to_dict()
        if datadict.get("id")==post_id:
            for key, value in datadict.items():
                if isinstance(value, datetime.datetime):
                    datadict[key] = value.isoformat()
            postkeyvaluedict.append(datadict)
            break
    #글 없으면 바로 리턴
    if not postkeyvaluedict:
        context['postexist']="no"
    else:
        context['postexist'] = "yes"
  
    #이 글의 댓글들 정보 firestore DB에서 가져오기
        comments_keyvaluelists=[]
        alldocs=comments_ref.stream()
        for doc in alldocs:
            datadict=doc.to_dict()
            if datadict.get("postid")==post_id:
                for key, value in datadict.items():
                    if isinstance(value, datetime.datetime):
                        datadict[key] = value.isoformat()
                comments_keyvaluelists.append(datadict)

        context['keyvaluedict'] = postkeyvaluedict
        context['comments_keyvaluelists'] = comments_keyvaluelists
    return JSONResponse(content=context)


#게시글 추가
@posting_router.post("/posts", tags=["Post"])
async def create_post(post: Post):

    print("create_post >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

    #추가할 DB키 생성하기
    postlist = list(post.dict())
    now = datetime.datetime.now()

    #post = postTable()

    alldbidlist=[]
    alldocs=posts_ref.stream()
    for doc in alldocs:
        docidstr=doc.id
        docdict=doc.to_dict()

        if docdict['id'] not in alldbidlist:
            alldbidlist.append(docdict['id'])

    lastdbid = max(alldbidlist) if alldbidlist else 0
    string_number = str(lastdbid + 1).zfill(8)

    # Firebase에 추가할 데이터 dict로 변환
    datadict = {key: value for key, value in post.dict().items()}
    datadict['id'] = int(string_number)

    # Firestore에 추가
    posts_ref.document(string_number).set(datadict)

    return { 'result_msg': f'{"put"} Registered...' }

#게시글 수정
@posting_router.put("/posts/{post_id}", tags=["Post"])
async def modify_post(post: Post, post_id : int):

    print("modify_post >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

    postid = str(post_id)
    string_number=postid.zfill(8)

    toupdate_posttitle=post.제목
    toupdate_postcontent=post.내용

    #firestore DB에 업데이트
    posts_ref.document(string_number).update({
        "제목": toupdate_posttitle,
        "내용": toupdate_postcontent
    })

    return {'result_msg': f"{postid} updated..."}
#게시글 삭제
@posting_router.delete("/posts/{post_id}", tags=["Post"])
async def delete_post(post_id: int):
    global posts_ref, comments_ref
    print("delete_post >>>")

    string_number = str(post_id)
    dbkey = string_number.zfill(8)

    posts_ref.document(dbkey).delete()

    alldocs = comments_ref.stream()
    for doc in alldocs:
        docdict = doc.to_dict()
        if docdict["postid"] == post_id:
            comments_ref.document(doc.id).delete()
    return {'result_msg' : f"Post {post_id} and its comments deleted."}
#댓글 추가 페이지
@posting_router.post("/comment", tags=["Comment"])
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

    comment_data= comment.dict()
    comment_data["작성일시"] = datetime.datetime.utcnow()

    with db.transaction() as transaction:
        transaction_create_comment(transaction, db, comment_data)
    #댓글이 달린 글의 firestore DB에 댓글갯수 +1
    return { 'result_msg': f'{"put"} Registered...' }
#댓글 수정
@posting_router.put("/comment/{comment_id}", tags=["Comment"])
async def modify_post(comment: Comment, comment_id : int):

    print("modify_post >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")

    commentid = str(comment_id)
    string_number=commentid.zfill(8)

    toupdate_commentcontent=comment.내용

    #firestore DB에 업데이트
    comments_ref.document(string_number).update({
    
        "내용": toupdate_commentcontent
    })

    return {'result_msg': f"{commentid} updated..."}
#댓글 삭제 기능
@posting_router.delete("/comment/{comment_id}", tags=["Comment"])
async def delete_comment(comment_id: int):
    global posts_ref, comments_ref
    print("delete_post >>>")

    string_number = str(comment_id)
    dbkey = string_number.zfill(8)

    comment_ref = comments_ref.document(dbkey)
    comment_doc = comment_ref.get()

    if not comment_doc.exists:
        return {'result_msg' : f"ID {comment_id}의 댓글을 찾을 수 없습니다."}
    comment_data = comment_doc.to_dict()
    post_id = comment_data.get('postid')
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
    return {'result_msg' : f"댓글 {comment_id} 삭제 성공"}

#게시글 좋아요 기능 추가
@posting_router.put("/posts_like/{post_id}", tags=["Post"])
async def like_post(post_id : int):
    global posts_ref
    print("like_post >>>")
    string_number = str(post_id)
    dbkey = string_number.zfill(8)
    post_doc_ref = posts_ref.document(dbkey)
    post_doc = post_doc_ref.get()
    
    if not post_doc.exists:
        return {'result_msg': f"Post {post_id} not found"}
    post_doc_ref.update({"좋아요수": firestore.Increment(1)})
    return {'result_msg': f"Post {post_id} liked!"}
#게시글 좋아요 삭제
@posting_router.put("/posts_unlike/{post_id}", tags=["Post"])
async def unlike_post(post_id: int):
    global posts_ref
    print("delete_post >>>")

    string_number = str(post_id)
    dbkey = string_number.zfill(8)

    post_doc_ref = posts_ref.document(dbkey)
    post_doc = post_doc_ref.get()

    if not post_doc.exists:
        return {'result_msg' : f"Post {post_id} not found"}
    post_data = post_doc.to_dict()
    current_likes = post_data.get("좋아요수", 0)

    if current_likes > 0:
        post_doc_ref.update({"좋아요수": current_likes - 1})
        return {'result_msg' : f"Post {post_id} unliked! Total likes: {currentlikes - 1}"}
    else:
        return {'result_msg' : f"Post {post_id} disliked!"}

#게시글 신고 기능
@posting_router.post("/report/post/{post_id}", tags=["Post"])
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

#댓글 신고 기능
@posting_router.post("/report/comment/{comment_id}", tags=["Comment"])
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

