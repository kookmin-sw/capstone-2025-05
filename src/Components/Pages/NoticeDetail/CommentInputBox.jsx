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
  userInfo,
}) {
  const profile_picture = sessionStorage.getItem('profile_picture');
  console.log(userInfo);
  const handlePostComment = () => {
    setIsPostComment(false);
    const commentData = {
      uid: uid,
      postid: post.id,
      작성자: userInfo == 'google' ? 'mapleDG' : userInfo?.data.nickname,
      내용: reviewComment.trim(),
      프로필이미지:
        userInfo == 'google'
          ? profile_picture
          : userInfo?.data.profile_image_url,
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
          } ease-in-out duration-700 fixed bottom-[1px]`}
        >
          <div className="flex w-full justify-center relative left-[45px] absolute bottom-4">
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
