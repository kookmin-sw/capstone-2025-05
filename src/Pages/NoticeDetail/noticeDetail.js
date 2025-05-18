import React, { useEffect, useState } from 'react';
import { useComentsQuery } from '../../Hooks/get/useCommentsQuery';
import { useLocation, useNavigate } from 'react-router-dom';
import { useEditPutMutation } from '../../Hooks/put/editPutMutation';
import { useLikePutMutation } from '../../Hooks/post/likePostMutation';
import { useDeletelikeMutation } from '../../Hooks/delete/deletelikeMutation';
import { useReportPostMutation } from '../../Hooks/post/reportPostMutation';
import { usePostCommentsMutation } from '../../Hooks/post/usePostCommentsMutation';
import { useDeletePostMutation } from '../../Hooks/delete/deletePostMutation';
import { useScrapPostMutation } from '../../Hooks/post/scrapPostMutation';
import { useDeleteCommentMutation } from '../../Hooks/delete/deleteCommentMutation';
import { useAuth } from '../../Context/AuthContext';
import PostHeader from '../../Components/Pages/NoticeDetail/PostHeader';
import PostActionBar from '../../Components/Pages/NoticeDetail/PostActionBar';
import CommentSection from '../../Components/Pages/NoticeDetail/CommentSection';
import CommentInputBox from '../../Components/Pages/NoticeDetail/CommentInputBox';
import CommentAngleButton from '../../Components/Pages/NoticeDetail/CommentAngleButton';
import { useUserQuery } from '../../Hooks/MyPage/PlayedMusic/useUserInfoQuery';
import { useRef } from 'react';

// 하트 아이콘 저작권(fariha begum)
//깃발 아이콘 저작권(Hilmy Abiyyu A.)
export default function NoticeDetail() {
  const location = useLocation();
  const post = location.state;
  const { uid } = useAuth();
  const post_uid = post.uid;
  const isAdmin = true; //로그인 계정이 관리자 계정일경우
  const isUser = uid == post_uid;
  /*Mutation 영역*/
  const { mutate } = useEditPutMutation(); //게시글 수정하기
  const { mutate: postCommentMutate } = usePostCommentsMutation(); //댓글 추가하기
  const { mutate: postLikeMutate } = useLikePutMutation(); //게시글 좋아요
  const { mutate: postReportMutate } = useReportPostMutation();
  const { mutate: deletelikeMutate } = useDeletelikeMutation(); //게시글 좋아요 취소
  const { mutate: deletePostMutate } = useDeletePostMutation(); //게시물 삭제하기
  const { mutate: scrapPostMutate } = useScrapPostMutation(); //북마크
  const { mutate: deleteScrapMutate } = useDeletePostMutation(); //북마크 취소
  const { mutate: deleteComment } = useDeleteCommentMutation(); //댓글 삭제
  /***************/

  const userInfo = useUserQuery(uid);

  const filterComments = (isShow) => {
    setIsLoading(true); // 로딩 시작
    if (comments) {
      setTimeout(() => {
        setIsShow(() => {
          const newIsShow = isShow;
          setFilteredComments(newIsShow ? comments : comments.slice(0, 3));
          setIsLoading(false); // 로딩 완료
          return newIsShow;
        });
      }, 500);
    }
  };
  //신고하기 게시글 모달 창을 위한 상태 관리
  const [isWriteReportOpen, setIsWriteReportOpen] = useState(false);

  const [isShow, setIsShow] = useState(false);
  const [comments, setComments] = useState();
  //접기&펼치기에 따라 보여줄 댓글 데이터
  const [filteredComments, setFilteredComments] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  //전체 게시물 리뷰 데이터
  const { data: commentsInfo } = useComentsQuery(post.id);
  //게시물 수정 여부
  const [isEditing, setIsEditing] = useState(false);
  //게시물 수정 타이틀
  const [editedTitle, setEditedTitle] = useState(post.title);
  //게시물 수정 내용
  const [editedContent, setEditedContent] = useState(post.content);
  const [isPostComment, setIsPostComment] = useState(false);

  // 댓글 관련 state
  const [reviewComment, setReviewComment] = useState('');
  const [selectedComments, setSelectedComments] = useState([]);
  const [isSelecting, setIsSelecting] = useState(false);

  //좋아요,신고하기 버튼 체크 여부
  const [liked, setLiked] = useState(false);
  const [clickLiked, setClickLiked] = useState(false);
  const [likeNum, setLikeNum] = useState(parseInt(post?.likes));
  const [isScrap, setIsScrap] = useState(false);
  const [isModalOpen, setIsModalOpen] = useState(false);

  const actionBarRef = useRef(null);
  const [isActionBarVisible, setIsActionBarVisible] = useState(true);

  useEffect(() => {
    if (commentsInfo) {
      setComments(commentsInfo);
      if (commentsInfo.length > 3)
        setFilteredComments(commentsInfo.slice(0, 3));
    }
  }, [commentsInfo]);

  useEffect(() => {
    // 이전에 좋아요 했던 글인지 체크
    const heartKey = `heart_${uid}_${post.id}`;
    const heartStored = localStorage.getItem(heartKey);
    setLiked(heartStored === 'true');
    // 이전에 스크랩 했던 글인지 체크
    const scrapKey = `scrap_${uid}_${post.id}`;
    const scrapStored = localStorage.getItem(scrapKey);
    setIsScrap(scrapStored === 'true');
  }, [post.id, uid]);

  useEffect(() => {
    if (!actionBarRef.current) return;

    const observer = new ResizeObserver(([entry]) => {
      const height = entry.contentRect.height;
      setIsActionBarVisible(height > 440); // 예: 30px보다 작으면 숨김
    });

    observer.observe(actionBarRef.current);
    return () => observer.disconnect();
  }, []);

  return (
    <div>
      <section
        id="NoticeBoard"
        className="bg-[F0EDE6] flex flex-col items-center mt-10"
        ref={actionBarRef}
      >
        {/* 컨텐츠 헤더 */}
        <PostHeader
          post={post}
          isEditing={isEditing}
          editedTitle={editedTitle}
          setEditedTitle={setEditedTitle}
          isModalOpen={isModalOpen}
          setIsModalOpen={setIsModalOpen}
          isAdmin={isAdmin}
          isUser={isUser}
          isWriteReportOpen={isWriteReportOpen}
          setIsWriteReportOpen={setIsWriteReportOpen}
          postReportMutate={postReportMutate}
          deletePostMutate={deletePostMutate}
          editedContent={editedContent}
          setEditedContent={setEditedContent}
        />
        {/*heart버튼과 신고하기 버튼 */}
        {isActionBarVisible && (
          <div className="flex mt-2 sm:w-[80%] lg:w-[60%] md:w-[70%]">
            <PostActionBar
              post={post}
              uid={uid}
              liked={liked}
              setLiked={setLiked}
              isScrap={isScrap}
              setIsScrap={setIsScrap}
              editedTitle={editedTitle}
              setEditedTitle={setEditedTitle}
              editedContent={editedContent}
              setEditedContent={setEditedContent}
              postLikeMutate={postLikeMutate}
              setClickLiked={setClickLiked}
              deletelikeMutate={deletelikeMutate}
              scrapPostMutate={scrapPostMutate}
              deleteScrapMutate={deleteScrapMutate}
              mutate={mutate}
              setIsEditing={setIsEditing}
              isEditing={isEditing}
              clickLiked={clickLiked}
              likeNum={likeNum}
              setLikeNum={setLikeNum}
            />
          </div>
        )}
      </section>

      {/* 댓글 영역 */}
      <section
        id="reviews"
        className="flex flex-col items-center mt-14 overflow-y-auto"
      >
        <CommentSection
          comments={comments}
          isPostComment={isPostComment}
          isSelecting={isSelecting}
          setIsSelecting={setIsSelecting}
          selectedComments={selectedComments}
          setSelectedComments={setSelectedComments}
          deleteComment={deleteComment}
          setComments={setComments}
          setIsPostComment={setIsPostComment}
          isAdmin={isAdmin}
          setIsShow={setIsShow}
          isUser={isUser}
          filteredComments={filteredComments}
        />
        {/* 댓글 작성 박스 */}
        <CommentInputBox
          reviewComment={reviewComment}
          setReviewComment={setReviewComment}
          isPostComment={isPostComment}
          setIsPostComment={setIsPostComment}
          uid={uid}
          post={post}
          postCommentMutate={postCommentMutate}
          isSelecting={isSelecting}
          userInfo={userInfo}
        />
        {/* 댓글 펼치기 & 접기 */}
        <CommentAngleButton
          comments={comments}
          isLoading={isLoading}
          isShow={isShow}
          filterComments={filterComments}
        />
      </section>
    </div>
  );
}
