import React from 'react';
import Input from '../../Input/input';
import swal from 'sweetalert';
import Button from '../../Button/Button';

export default function CommentInputBox({
  reviewComment,
  setReviewComment,
  isPostComment,
  setIsPostComment,
  uid,
  post,
  postCommentMutate,
  isSelecting,
}) {
  const handlePostComment = () => {
    setIsPostComment(false);
    const commentData = {
      uid: uid, //로그인 uid로 해야됨
      작성일시: new Date().toISOString(), // 현재 시간
      postid: post.id, // 현재 게시글 ID
      작성자: '누굴까', // 작성자 이름
      내용: reviewComment.trim(),
      비밀번호: '1234',
    };

    postCommentMutate(commentData, {
      onSuccess: () => {
        console.log('댓글 추가 성공');

        setReviewComment('');
      },
      onError: (error) => {
        swal('', '댓글 추가 실패😥', error);
      },
    });
  };
  return (
    <>
      {!isSelecting && (
        <div
          className={`flex sm:w-[85%] lg:w-[65%] md:w-[75%] my-5 transition-opacity ${
            isPostComment ? 'visible' : 'hidden'
          } ease-in-out duration-700`}
        >
          <div className="flex w-full justify-center relative left-[45px]">
            <Input
              className="w-full"
              height="60px"
              placeholder="댓글을 작성해주세요..."
              value={reviewComment}
              onChange={(e) => {
                setReviewComment(e.target.value);
              }}
              onKeyDown={(e) => {
                if (e.key === 'Enter') {
                  e.preventDefault(); // 줄바꿈 방지
                  handlePostComment(); // 댓글 전송 함수 호출
                }
              }}
            />
            <div className="flex items-center relative right-[90px]">
              <Button
                height={'45px'}
                width={'80px'}
                onClick={handlePostComment}
              >
                완료
              </Button>
            </div>
          </div>
        </div>
      )}
    </>
  );
}
