import os
import datetime
from typing import Dict, List, Optional, Any
from fastapi import HTTPException, UploadFile, APIRouter
from fastapi.responses import JSONResponse, StreamingResponse
from firebase_admin import auth, firestore
from manager.firebase_manager import firestore_db, storage_bucket
from manager.post_model import Post, Comment

router = APIRouter()

class PostService:
    @staticmethod
    def _get_user_profile_image(uid: str) -> str:
        user_doc = firestore_db.collection("users").document(uid).get()
        if user_doc.exists:
            return user_doc.to_dict().get("profile_image", "")
        return ""

    @staticmethod
    async def get_posts(limit: int = 10) -> Dict[str, Any]:
        try:
            postsdictlist = []
            alldocs = firestore_db.collection("post").order_by("date", direction=firestore.Query.DESCENDING).limit(limit).stream()
            for doc in alldocs:
                datadict = doc.to_dict()
                for key, value in datadict.items():
                    if isinstance(value, datetime.datetime):
                        datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
                postsdictlist.append(datadict)
            return {"limit": limit, "posts": postsdictlist}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"목록 가져오기 실패: {str(e)}")

    @staticmethod
    async def increase_views(post_id: int) -> Dict[str, Any]:
        try:
            string_number = str(post_id).zfill(8)
            post_ref = firestore_db.collection("post").document(string_number)
            post = post_ref.get()
            if not post.exists:
                return {"error": "Post not found"}
            post_data = post.to_dict()
            new_views = post_data.get("조회수", 0) + 1
            post_ref.set({"조회수": new_views}, merge=True)
            post_data["조회수"] = new_views
            return post_data
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"조회수 증가 실패: {str(e)}")

    @staticmethod
    async def get_top_viewed_post() -> Dict[str, Any]:
        try:
            postsdictlist = []
            alldocs = firestore_db.collection("post").stream()
            for doc in alldocs:
                datadict = doc.to_dict()
                if '조회수' not in datadict:
                    datadict['조회수'] = 0
                postsdictlist.append(datadict)
            postsdictlist.sort(key=lambda x: x.get('조회수', 0), reverse=True)
            top_post = postsdictlist[0] if postsdictlist else {}
            for key, value in top_post.items():
                if isinstance(value, datetime.datetime):
                    top_post[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
            return top_post
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"인기 게시글 조회 실패: {str(e)}")

    @staticmethod
    async def read_post(post_id: int) -> Dict[str, Any]:
        try:
            context = {'post_id': post_id}
            postkeyvaluedict = []
            alldocs = firestore_db.collection("post").stream()
            for doc in alldocs:
                datadict = doc.to_dict()
                if datadict.get("id") == post_id:
                    for key, value in datadict.items():
                        if isinstance(value, datetime.datetime):
                            datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
                    postkeyvaluedict.append(datadict)
                    break
            if not postkeyvaluedict:
                context['postexist'] = "no"
            else:
                context['postexist'] = "yes"
                comments_keyvaluelists = []
                alldocs = firestore_db.collection("comment").stream()
                for doc in alldocs:
                    datadict = doc.to_dict()
                    if datadict.get("postid") == post_id:
                        for key, value in datadict.items():
                            if isinstance(value, datetime.datetime):
                                datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
                        comments_keyvaluelists.append(datadict)
                context['keyvaluedict'] = postkeyvaluedict
                context['comments_keyvaluelists'] = comments_keyvaluelists
            return context
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 조회 실패: {str(e)}")

    @staticmethod
    async def _upload_file_to_firebase(file: UploadFile, path: str) -> Optional[str]:
        try:
            blob = storage_bucket.blob(path)
            file.file.seek(0)
            blob.upload_from_file(file.file, content_type=file.content_type)
            blob.make_public()
            return blob.public_url
        except Exception as e:
            return None

    @staticmethod
    async def create_post(uid: str, content: str, author: str, title: str, image: Optional[UploadFile] = None, audio: Optional[UploadFile] = None) -> Dict[str, Any]:
        try:
            now = datetime.datetime.now()
            alldbidlist = [doc.to_dict().get("id") for doc in firestore_db.collection("post").stream()]
            alldbidlist = [id for id in alldbidlist if id is not None]
            lastdbid = max(alldbidlist) if alldbidlist else 0
            string_number = str(lastdbid + 1).zfill(8)

            image_url = None
            if image:
                image_path = f"post/image/{string_number}_{image.filename}"
                image_url = await PostService._upload_file_to_firebase(image, image_path)

            audio_url = None
            if audio:
                audio_path = f"post/audio/{string_number}_{audio.filename}"
                audio_url = await PostService._upload_file_to_firebase(audio, audio_path)

            post_data = {
                "uid": int(string_number),
                "내용": content,
                "댓글갯수": 0,
                "date": now.isoformat(),
                "작성자": author,
                "title": title,
                "조회수": 0,
                "좋아요수": 0,
                "image_url": image_url,
                "audio_url": audio_url
            }

            firestore_db.collection("post").document(string_number).set(post_data)
            firestore_db.collection("my_activity").document(str(uid)).collection("post").document(string_number).set({
                "post_id": string_number,
                "title": title,
                "date": now.isoformat()
            })

            return {"result_msg": "게시글이 성공적으로 등록되었습니다.", "post_id": string_number}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 생성 실패: {str(e)}")

    @staticmethod
    async def modify_post(post_id: int, uid: str, title: str, content: str, author: str, image: Optional[UploadFile] = None, audio: Optional[UploadFile] = None) -> Dict[str, str]:
        try:
            string_number = str(post_id).zfill(8)
            updated_at = datetime.datetime.utcnow()
            update_data = {
                "title": title,
                "내용": content,
                "date": updated_at
            }
            if image:
                image_path = f"post/image/{string_number}_{image.filename}"
                image_url = await PostService._upload_file_to_firebase(image, image_path)
                update_data["image_url"] = image_url
            if audio:
                audio_path = f"post/audio/{string_number}_{audio.filename}"
                audio_url = await PostService._upload_file_to_firebase(audio, audio_path)
                update_data["audio_url"] = audio_url
            firestore_db.collection("post").document(string_number).update(update_data)
            post_doc = firestore_db.collection("post").document(string_number).get()
            if post_doc.exists:
                firestore_db.collection("my_activity").document(uid).collection("post").document(string_number).update({
                    "title": title,
                    "date": updated_at.isoformat()
                })
            return {'result_msg': f"{post_id} updated..."}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 수정 실패: {str(e)}")

    @staticmethod
    async def delete_post(post_id: int) -> Dict[str, str]:
        try:
            string_number = str(post_id).zfill(8)
            post_ref = firestore_db.collection("post").document(string_number)
            post_doc = post_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail="게시글이 존재하지 않습니다.")
            post_data = post_doc.to_dict()
            uid = post_data.get("uid")
            post_ref.delete()
            alldocs = firestore_db.collection("comment").stream()
            for doc in alldocs:
                docdict = doc.to_dict()
                if docdict["postid"] == post_id:
                    firestore_db.collection("comment").document(doc.id).delete()
            if uid:
                firestore_db.collection("my_activity").document(str(uid)).collection("post").document(string_number).delete()
            return {'result_msg': f"Post {post_id} and its comments deleted."}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 삭제 실패: {str(e)}")

    @staticmethod
    async def create_comment(comment: Comment) -> Dict[str, str]:
        try:
            comment_data = comment.dict()
            now = datetime.datetime.utcnow()
            comment_data["date"] = now
            if comment_data.get("uid"):
                comment_data["profile_image"] = PostService._get_user_profile_image(comment_data["uid"])
            comments_snapshot = firestore_db.collection("comment").order_by("id", direction=firestore.Query.DESCENDING).limit(1).stream()
            last_comment_id = 0
            for doc in comments_snapshot:
                last_comment_id = doc.to_dict().get("id", 0)
            new_comment_id = last_comment_id + 1
            comment_data["id"] = new_comment_id
            comment_doc_ref = firestore_db.collection("comment").document(str(new_comment_id).zfill(8))
            comment_doc_ref.set(comment_data)
            post_ref = firestore_db.collection("post").document(str(comment_data["postid"]).zfill(8))
            post_ref.update({"댓글갯수": firestore.Increment(1)})
            if comment_data.get("uid"):
                uid = comment_data["uid"]
                post_id_str = str(comment_data["postid"]).zfill(8)
                firestore_db.collection("my_activity").document(uid).collection("comment").document(str(new_comment_id).zfill(8)).set({
                    "post_id": post_id_str,
                    "date": now.isoformat()
                })
            return {'result_msg': f"put Registered..."}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 생성 실패: {str(e)}")

    @staticmethod
    async def modify_comment(comment: Comment, comment_id: int) -> Dict[str, str]:
        try:
            string_number = str(comment_id).zfill(8)
            toupdate_commentcontent = comment.내용
            updated_at = datetime.datetime.utcnow()
            comments_ref = firestore_db.collection("comment").document(string_number)
            comments_ref.update({
                "내용": toupdate_commentcontent,
                "date": updated_at
            })
            existing_comment = comments_ref.get()
            if existing_comment.exists:
                comment_data = existing_comment.to_dict()
                uid = comment_data.get("uid")
                if uid:
                    firestore_db.collection("my_activity").document(uid).collection("comment").document(string_number).update({
                        "내용": toupdate_commentcontent,
                        "date": updated_at.isoformat()
                    })
            return {'result_msg': f"{comment_id} updated..."}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 수정 실패: {str(e)}")

    @staticmethod
    async def delete_comment(comment_id: int) -> Dict[str, str]:
        try:
            string_number = str(comment_id).zfill(8)
            comment_ref = firestore_db.collection("comment").document(string_number)
            comment_doc = comment_ref.get()
            if not comment_doc.exists:
                return {'result_msg': f"ID {comment_id}의 댓글을 찾을 수 없습니다."}
            comment_data = comment_doc.to_dict()
            post_id = comment_data.get('postid')
            uid = comment_data.get('uid')
            postkey = str(post_id).zfill(8)
            if post_id is None:
                return {'result_msg': "이 댓글에 연결된 게시글이 없습니다."}
            comment_ref.delete()
            post_ref = firestore_db.collection("post").document(postkey)
            post_doc = post_ref.get()
            if post_doc.exists:
                post_data = post_doc.to_dict()
                current_comment_count = post_data.get('댓글갯수', 0)
                if isinstance(current_comment_count, int) and current_comment_count > 0:
                    new_comment_count = current_comment_count - 1
                    post_ref.update({"댓글갯수": new_comment_count})
            if uid:
                firestore_db.collection("my_activity").document(uid).collection("comment").document(string_number).delete()
            return {'result_msg': f"댓글 {comment_id} 삭제 성공"}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 삭제 실패: {str(e)}")

    @staticmethod
    async def like_post(post_id: int, uid: str) -> Dict[str, Any]:
        try:
            string_number = str(post_id).zfill(8)
            like_doc_ref = firestore_db.collection("my_activity").document(uid).collection("like").document(string_number)
            post_doc_ref = firestore_db.collection("post").document(string_number)
            if like_doc_ref.get().exists:
                raise HTTPException(status_code=400, detail="이미 좋아요한 게시글입니다.")
            post_doc = post_doc_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail="게시글이 존재하지 않습니다.")
            post_doc_ref.update({"좋아요수": firestore.Increment(1)})
            like_doc_ref.set({
                "user_id": uid,
                "post_id": string_number,
                "like_date": datetime.datetime.utcnow()
            })
            return {"result_msg": "좋아요가 추가되었습니다.", "post_id": post_id}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"좋아요 추가 실패: {str(e)}")

    @staticmethod
    async def unlike_post(post_id: int, uid: str) -> Dict[str, Any]:
        try:
            string_number = str(post_id).zfill(8)
            like_doc_ref = firestore_db.collection("my_activity").document(uid).collection("like").document(string_number)
            post_doc_ref = firestore_db.collection("post").document(string_number)
            like_doc = like_doc_ref.get()
            if not like_doc.exists:
                raise HTTPException(status_code=404, detail="좋아요하지 않은 게시글입니다.")
            post_doc = post_doc_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail="게시글이 존재하지 않습니다.")
            post_doc_ref.update({"좋아요수": firestore.Increment(-1)})
            like_doc_ref.delete()
            return {"result_msg": "좋아요가 취소되었습니다.", "post_id": post_id}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"좋아요 취소 실패: {str(e)}")

    @staticmethod
    async def scrap_post(post_id: str, uid: str) -> Dict[str, Any]:
        try:
            post_ref = firestore_db.collection("post").document(post_id)
            post = post_ref.get()
            if not post.exists:
                raise HTTPException(status_code=404, detail="게시글이 존재하지 않습니다.")
            scrap_doc_ref = firestore_db.collection("my_activity").document(uid).collection("scrap").document(post_id)
            if scrap_doc_ref.get().exists:
                raise HTTPException(status_code=400, detail="이미 스크랩한 게시글입니다.")
            scrap_doc_ref.set({
                "user_id": uid,
                "post_id": post_id,
                "scrap_date": datetime.datetime.utcnow()
            })
            return {"message": "스크랩이 추가되었습니다.", "post_id": post_id}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"스크랩 추가 실패: {str(e)}")

    @staticmethod
    async def remove_scrap(post_id: str, uid: str) -> Dict[str, Any]:
        try:
            scrap_doc_ref = firestore_db.collection("my_activity").document(uid).collection("scrap").document(post_id)
            if not scrap_doc_ref.get().exists:
                raise HTTPException(status_code=404, detail="스크랩하지 않은 게시글입니다.")
            scrap_doc_ref.delete()
            return {"message": "스크랩이 취소되었습니다.", "post_id": post_id}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"스크랩 취소 실패: {str(e)}")

    @staticmethod
    async def report_post(post_id: int, reason: str) -> Dict[str, str]:
        try:
            report_data = {
                "type": "post",
                "post_id": post_id,
                "reason": reason,
                "timestamp": datetime.datetime.now()
            }
            firestore_db.collection("reports").add(report_data)
            return {"result_msg": f"Post {post_id} reported for reason: {reason}"}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 신고 실패: {str(e)}")

    @staticmethod
    async def report_comment(comment_id: int, reason: str) -> Dict[str, str]:
        try:
            report_data = {
                "type": "comment",
                "comment_id": comment_id,
                "reason": reason,
                "timestamp": datetime.datetime.now()
            }
            firestore_db.collection("reports").add(report_data)
            return {"result_msg": f"Comment {comment_id} reported for reason: {reason}"}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 신고 실패: {str(e)}")

    @staticmethod
    async def search_post(query: str) -> Dict[str, List[Dict[str, Any]]]:
        try:
            query_lower = query.lower()
            docs = firestore_db.collection("post").order_by("date", direction="DESCENDING").order_by("title", direction="ASCENDING").stream()
            results = []
            for doc in docs:
                data = doc.to_dict()
                title = data.get("title", "").lower()
                if query_lower in title:
                    results.append({
                        "id": doc.id,
                        "title": data.get("title", ""),
                        "content": data.get("내용", ""),
                        "created_at": data.get("date", "")
                    })
            return {"results": results}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"검색 실패: {str(e)}")

    @staticmethod
    async def autocomplete(query: str) -> Dict[str, List[str]]:
        try:
            query_lower = query.lower()
            docs = firestore_db.collection("post").order_by("title").start_at([query_lower]).end_at([query_lower + "\uf8ff"]).stream()
            suggestions = [doc.to_dict().get("title", "") for doc in docs]
            return {"suggestions": suggestions}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"자동완성 실패: {str(e)}")

    @staticmethod
    async def read_comment(post_id: int) -> Dict[str, Any]:
        try:
            context = {'post_id': post_id}
            comments_keyvaluelists = []
            alldocs = firestore_db.collection("comment").stream()
            for doc in alldocs:
                datadict = doc.to_dict()
                if datadict.get("postid") == post_id:
                    for key, value in datadict.items():
                        if isinstance(value, datetime.datetime):
                            datadict[key] = value.strftime("%Y년 %m월 %d일 %H시 %M분")
                    comments_keyvaluelists.append(datadict)
            if not comments_keyvaluelists:
                context['comments_exist'] = "no"
            else:
                context['comments_exist'] = "yes"
                context['comments_keyvaluelists'] = comments_keyvaluelists
            return context
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 조회 실패: {str(e)}")
