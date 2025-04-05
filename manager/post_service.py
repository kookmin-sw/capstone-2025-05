import datetime
from manager.firebase_manager import firestore_db
from fastapi import HTTPException
from firebase_admin import firestore
from model import Post, Comment
from typing import List, Dict, Any, Optional

class PostService:
    """게시글 관련 서비스를 처리하는 클래스"""
    
    @staticmethod
    async def get_all_posts() -> List[Dict[str, Any]]:
        """모든 게시글 목록을 반환합니다."""
        try:
            posts_ref = firestore_db.collection("post")
            posts = []
            
            for doc in posts_ref.stream():
                post_data = doc.to_dict()
                # datetime 객체를 ISO 형식 문자열로 변환
                for key, value in post_data.items():
                    if isinstance(value, datetime.datetime):
                        post_data[key] = value.isoformat()
                posts.append(post_data)
                
            return posts
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 조회 실패: {str(e)}")
    
    @staticmethod
    async def increase_view_count(post_id: int) -> Dict[str, Any]:
        """게시글 조회수를 증가시킵니다."""
        try:
            string_number = str(post_id).zfill(8)
            post_ref = firestore_db.collection("post").document(string_number)
            post = post_ref.get()

            if not post.exists:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            post_data = post.to_dict()
            new_views = post_data.get("조회수", 0) + 1
            post_ref.set({"조회수": new_views}, merge=True)

            post_data["조회수"] = new_views
            # datetime 객체를 ISO 형식 문자열로 변환
            for key, value in post_data.items():
                if isinstance(value, datetime.datetime):
                    post_data[key] = value.isoformat()
            
            return post_data
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"조회수 증가 실패: {str(e)}")
    
    @staticmethod
    async def get_top_viewed_post() -> List[Dict[str, Any]]:
        """조회수가 가장 높은 게시글을 반환합니다."""
        try:
            posts_ref = firestore_db.collection("post")
            post_list = []
            
            for doc in posts_ref.stream():
                post_data = doc.to_dict()
                if '조회수' not in post_data:
                    post_data['조회수'] = 0
                post_list.append(post_data)
                
            if not post_list:
                return []
                
            post_list.sort(key=lambda x: x.get('조회수', 0), reverse=True)
            top_post = post_list[0]
            
            # datetime 객체를 ISO 형식 문자열로 변환
            for key, value in top_post.items():
                if isinstance(value, datetime.datetime):
                    top_post[key] = value.isoformat()
                    
            return [top_post]
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"인기 게시글 조회 실패: {str(e)}")
    
    @staticmethod
    async def get_post_by_id(post_id: int) -> Dict[str, Any]:
        """ID로 게시글을 조회합니다."""
        try:
            posts_ref = firestore_db.collection("post")
            comments_ref = firestore_db.collection("comment")
            
            post_data = None
            for doc in posts_ref.stream():
                data = doc.to_dict()
                if data.get("id") == post_id:
                    post_data = data
                    break
            
            if not post_data:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            # datetime 객체를 ISO 형식 문자열로 변환
            for key, value in post_data.items():
                if isinstance(value, datetime.datetime):
                    post_data[key] = value.isoformat()
            
            # 댓글 조회
            comments = []
            for doc in comments_ref.stream():
                data = doc.to_dict()
                if data.get("postid") == post_id:
                    # datetime 객체를 ISO 형식 문자열로 변환
                    for key, value in data.items():
                        if isinstance(value, datetime.datetime):
                            data[key] = value.isoformat()
                    comments.append(data)
            
            return {
                "post": post_data,
                "comments": comments
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 상세 조회 실패: {str(e)}")
    
    @staticmethod
    async def create_post(post: Post) -> Dict[str, Any]:
        """새 게시글을 생성합니다."""
        try:
            posts_ref = firestore_db.collection("post")
            
            # 새 게시글 ID 생성
            all_ids = []
            for doc in posts_ref.stream():
                doc_dict = doc.to_dict()
                if "id" in doc_dict:
                    all_ids.append(doc_dict["id"])
                    
            new_id = 1 if not all_ids else max(all_ids) + 1
            post_id_string = str(new_id).zfill(8)
            
            # 게시글 데이터 준비
            post_data = post.dict()
            post_data["id"] = new_id
            
            # Firestore에 저장
            posts_ref.document(post_id_string).set(post_data)
            
            return {"id": new_id, "message": "게시글이 성공적으로 생성되었습니다."}
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 생성 실패: {str(e)}")
    
    @staticmethod
    async def update_post(post_id: int, post: Post) -> Dict[str, Any]:
        """게시글을 수정합니다."""
        try:
            post_id_string = str(post_id).zfill(8)
            post_ref = firestore_db.collection("post").document(post_id_string)
            
            # 게시글 존재 확인
            post_doc = post_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            # 업데이트할 필드
            update_fields = {
                "제목": post.제목,
                "내용": post.내용
            }
            
            # Firestore 업데이트
            post_ref.update(update_fields)
            
            return {"id": post_id, "message": "게시글이 성공적으로 수정되었습니다."}
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 수정 실패: {str(e)}")
    
    @staticmethod
    async def delete_post(post_id: int) -> Dict[str, Any]:
        """게시글과 관련 댓글을 삭제합니다."""
        try:
            posts_ref = firestore_db.collection("post")
            comments_ref = firestore_db.collection("comment")
            
            # 게시글 삭제
            post_id_string = str(post_id).zfill(8)
            post_ref = posts_ref.document(post_id_string)
            
            # 게시글 존재 확인
            post_doc = post_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            post_ref.delete()
            
            # 연관된 댓글 삭제
            deleted_comments = 0
            for comment_doc in comments_ref.stream():
                comment_data = comment_doc.to_dict()
                if comment_data.get("postid") == post_id:
                    comments_ref.document(comment_doc.id).delete()
                    deleted_comments += 1
            
            return {
                "id": post_id, 
                "deleted_comments": deleted_comments,
                "message": f"게시글과 관련 댓글 {deleted_comments}개가 삭제되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 삭제 실패: {str(e)}")
    
    @staticmethod
    async def create_comment(comment: Comment) -> Dict[str, Any]:
        """게시글에 새 댓글을 생성합니다."""
        try:
            posts_ref = firestore_db.collection("post")
            comments_ref = firestore_db.collection("comment")
            
            # 트랜잭션으로 댓글 등록 및 게시글 댓글 수 업데이트
            @firestore.transactional
            def create_comment_transaction(transaction, posts_ref, comments_ref, comment_data):
                # 게시글 존재 확인
                post_id_string = str(comment_data["postid"]).zfill(8)
                post_ref = posts_ref.document(post_id_string)
                post_doc = post_ref.get(transaction=transaction)
                
                if not post_doc.exists:
                    raise HTTPException(status_code=404, detail=f"게시글 ID {comment_data['postid']}를 찾을 수 없습니다.")
                
                # 새 댓글 ID 생성
                comments_snapshot = comments_ref.order_by("id", direction=firestore.Query.DESCENDING).limit(1).stream()
                last_comment_id = 0
                for doc in comments_snapshot:
                    doc_data = doc.to_dict()
                    if "id" in doc_data:
                        last_comment_id = doc_data["id"]
                        break
                
                new_comment_id = last_comment_id + 1
                comment_data["id"] = new_comment_id
                comment_id_string = str(new_comment_id).zfill(8)
                
                # 댓글 저장
                comment_ref = comments_ref.document(comment_id_string)
                transaction.set(comment_ref, comment_data)
                
                # 게시글 댓글 수 업데이트
                transaction.update(post_ref, {"댓글갯수": firestore.Increment(1)})
                
                return new_comment_id
            
            # 트랜잭션 실행
            comment_data = comment.dict()
            transaction = firestore_db.transaction()
            new_comment_id = create_comment_transaction(transaction, posts_ref, comments_ref, comment_data)
            
            return {
                "id": new_comment_id, 
                "postid": comment.postid,
                "message": "댓글이 성공적으로 등록되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 작성 실패: {str(e)}")
    
    @staticmethod
    async def update_comment(comment_id: int, comment: Comment) -> Dict[str, Any]:
        """댓글을 수정합니다."""
        try:
            comment_id_string = str(comment_id).zfill(8)
            comment_ref = firestore_db.collection("comment").document(comment_id_string)
            
            # 댓글 존재 확인
            comment_doc = comment_ref.get()
            if not comment_doc.exists:
                raise HTTPException(status_code=404, detail=f"댓글 ID {comment_id}를 찾을 수 없습니다.")
                
            # 업데이트할 필드
            comment_ref.update({
                "내용": comment.내용
            })
            
            return {
                "id": comment_id,
                "message": "댓글이 성공적으로 수정되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 수정 실패: {str(e)}")
            
    @staticmethod
    async def delete_comment(comment_id: int) -> Dict[str, Any]:
        """댓글을 삭제하고 게시글의 댓글 수를 감소시킵니다."""
        try:
            comments_ref = firestore_db.collection("comment")
            posts_ref = firestore_db.collection("post")
            
            # 트랜잭션으로 댓글 삭제 및 게시글 댓글 수 업데이트
            @firestore.transactional
            def delete_comment_transaction(transaction, comments_ref, posts_ref, comment_id_string):
                comment_ref = comments_ref.document(comment_id_string)
                comment_doc = comment_ref.get(transaction=transaction)
                
                if not comment_doc.exists:
                    raise HTTPException(status_code=404, detail=f"댓글 ID {comment_id}를 찾을 수 없습니다.")
                    
                comment_data = comment_doc.to_dict()
                post_id = comment_data.get("postid")
                
                if post_id is None:
                    raise HTTPException(status_code=400, detail="댓글에 연결된 게시글 정보가 없습니다.")
                    
                # 게시글 존재 확인
                post_id_string = str(post_id).zfill(8)
                post_ref = posts_ref.document(post_id_string)
                post_doc = post_ref.get(transaction=transaction)
                
                if post_doc.exists:
                    post_data = post_doc.to_dict()
                    current_comment_count = post_data.get("댓글갯수", 0)
                    
                    if isinstance(current_comment_count, int) and current_comment_count > 0:
                        # 댓글 수 감소
                        transaction.update(post_ref, {"댓글갯수": current_comment_count - 1})
                
                # 댓글 삭제
                transaction.delete(comment_ref)
                
                return post_id
            
            # 트랜잭션 실행
            comment_id_string = str(comment_id).zfill(8)
            transaction = firestore_db.transaction()
            post_id = delete_comment_transaction(transaction, comments_ref, posts_ref, comment_id_string)
            
            return {
                "id": comment_id,
                "postid": post_id,
                "message": "댓글이 성공적으로 삭제되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 삭제 실패: {str(e)}")
            
    @staticmethod
    async def like_post(post_id: int) -> Dict[str, Any]:
        """게시글에 좋아요를 추가합니다."""
        try:
            post_id_string = str(post_id).zfill(8)
            post_ref = firestore_db.collection("post").document(post_id_string)
            
            # 게시글 존재 확인
            post_doc = post_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            # 좋아요 수 증가
            post_ref.update({"좋아요수": firestore.Increment(1)})
            
            # 업데이트된 좋아요 수 가져오기
            updated_doc = post_ref.get()
            updated_likes = updated_doc.to_dict().get("좋아요수", 0)
            
            return {
                "id": post_id,
                "likes": updated_likes,
                "message": "게시글에 좋아요가 추가되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"좋아요 추가 실패: {str(e)}")
            
    @staticmethod
    async def unlike_post(post_id: int) -> Dict[str, Any]:
        """게시글의 좋아요를 취소합니다."""
        try:
            post_id_string = str(post_id).zfill(8)
            post_ref = firestore_db.collection("post").document(post_id_string)
            
            # 게시글 존재 확인
            post_doc = post_ref.get()
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            post_data = post_doc.to_dict()
            current_likes = post_data.get("좋아요수", 0)
            
            if current_likes > 0:
                # 좋아요 수 감소
                post_ref.update({"좋아요수": current_likes - 1})
                updated_likes = current_likes - 1
            else:
                updated_likes = 0
            
            return {
                "id": post_id,
                "likes": updated_likes,
                "message": "게시글의 좋아요가 취소되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"좋아요 취소 실패: {str(e)}")
            
    @staticmethod
    async def report_post(post_id: int, reason: str) -> Dict[str, Any]:
        """게시글을 신고합니다."""
        try:
            reports_ref = firestore_db.collection("reports")
            
            # 게시글 존재 확인
            posts_ref = firestore_db.collection("post")
            post_id_string = str(post_id).zfill(8)
            post_doc = posts_ref.document(post_id_string).get()
            
            if not post_doc.exists:
                raise HTTPException(status_code=404, detail=f"게시글 ID {post_id}를 찾을 수 없습니다.")
                
            # 신고 데이터 생성
            report_data = {
                "type": "post",
                "post_id": post_id,
                "reason": reason,
                "timestamp": datetime.datetime.now()
            }
            
            # Firestore에 저장
            report_doc = reports_ref.add(report_data)
            
            return {
                "id": report_doc[1].id,
                "post_id": post_id,
                "message": "게시글이 신고되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"게시글 신고 실패: {str(e)}")
            
    @staticmethod
    async def report_comment(comment_id: int, reason: str) -> Dict[str, Any]:
        """댓글을 신고합니다."""
        try:
            reports_ref = firestore_db.collection("reports")
            
            # 댓글 존재 확인
            comments_ref = firestore_db.collection("comment")
            comment_id_string = str(comment_id).zfill(8)
            comment_doc = comments_ref.document(comment_id_string).get()
            
            if not comment_doc.exists:
                raise HTTPException(status_code=404, detail=f"댓글 ID {comment_id}를 찾을 수 없습니다.")
                
            # 신고 데이터 생성
            report_data = {
                "type": "comment",
                "comment_id": comment_id,
                "reason": reason,
                "timestamp": datetime.datetime.now()
            }
            
            # Firestore에 저장
            report_doc = reports_ref.add(report_data)
            
            return {
                "id": report_doc[1].id,
                "comment_id": comment_id,
                "message": "댓글이 신고되었습니다."
            }
        except HTTPException as e:
            raise e
        except Exception as e:
            raise HTTPException(status_code=500, detail=f"댓글 신고 실패: {str(e)}")