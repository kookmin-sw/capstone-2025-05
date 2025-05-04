import React from 'react';
import heart from '../../../Assets/Images/heart.png';
import fill_heart from '../../../Assets/Images/fill_heart.png';
import bookmark from '../../../Assets/bookmark.svg';
import fill_bookmark from '../../../Assets/MyPage/filledBookmark.svg';
import Button from '../../Button/Button';
import swal from 'sweetalert';
import { useNavigate } from 'react-router-dom';

export default function PostActionBar({
  post,
  uid,
  liked,
  setLiked,
  isScrap,
  setIsScrap,
  editedTitle,
  setEditedTitle,
  editedContent,
  setEditedContent,
  postLikeMutate,
  setClickLiked,
  deletelikeMutate,
  scrapPostMutate,
  deleteScrapMutate,
  mutate,
  setIsEditing,
  isEditing,
  clickLiked,
  likeNum,
}) {
  const navigate = useNavigate();
  //좋아요 버튼 클릭 이벤트
  const handleHeartBttn = () => {
    setLiked(!liked);
    //여기에는 좋아요 + 1기능을 하는 mutate를 집어넣어주면 됨
    if (!liked) {
      //하트를 새로 누른경우
      postLikeMutate(
        { post_id: post.id, uid },
        {
          onSuccess: () => {
            console.log('좋아요 성공');
          },
          onError: (error) => console.log('좋아요 실패ㅠㅠ', error),
        },
      );
      localStorage.setItem('heart', uid, post.id);
      setClickLiked(true);
    } else {
      deletelikeMutate(
        { post_id: post.id, uid },
        {
          onSuccess: () => {
            console.log('좋아요 취소 성공');
          },
          onError: (error) => console.log('좋아요 취소 실패ㅠㅠ', error),
        },
      );
      localStorage.removeItem('heart', uid, post.id);
      setClickLiked(false);
    }
  };
  //북바크
  const handleScrap = () => {
    setIsScrap(!isScrap);
    if (!isScrap) {
      //북마크 클릭
      scrapPostMutate(
        { post_id: post.id, uid },
        {
          onSuccess: () => {
            console.log('북마크 성공');
          },
          onError: (error) => console.log('북마크 실패ㅠㅠ', error),
        },
      );
    } else {
      deleteScrapMutate(
        { post_id: post.id, uid },
        {
          onSuccess: () => {
            console.log('북마크 취소 성공');
          },
          onError: (error) => console.log('북마크 취소 실패ㅠㅠ', error),
        },
      );
    }
  };

  //저장 기능
  const handleSave = () => {
    const post_id = post.id;

    mutate(
      { post_id, editedTitle, editedContent },
      {
        onSuccess: () => {
          post.title = editedTitle;
          post.content = editedContent;
          setIsEditing(false);
          swal('', '게시물 수정 완료 🫡', 'success');
        },
        onError: (error) => {
          console.error('게시물 수정 중 오류 발생:', error);
          swal('', '게시물 수정에 실패했습니다.😥', 'error');
        },
      },
    );
  };

  //수정 모드
  const toggleEditMode = () => {
    setIsEditing(!isEditing);
    setEditedTitle(post.title);
    setEditedContent(post.content);
  };
  return (
    <div className="flex sm:w-[80%] lg:w-[60%] md:w-[70%] justify-between mt-4">
      <div className="flex items-center">
        <button className="mr-2 w-10 h-[40px]" onClick={handleHeartBttn}>
          <img
            src={liked ? fill_heart : heart}
            className="duration-300 ease-in-out hover:scale-[110%]"
          />
          <span>{clickLiked ? likeNum + 1 : likeNum}</span>
        </button>
        <button className="w-[45px]" onClick={handleScrap}>
          <img
            src={isScrap ? fill_bookmark : bookmark}
            className=" duration-300 ease-in-out hover:scale-[110%]"
          />
        </button>
      </div>
      <div className="flex">
        {isEditing ? (
          <div className="flex">
            <div className="duration-300 ease-in-out hover:scale-[110%]">
              <Button width="60px" height="40px" onClick={handleSave}>
                저장
              </Button>
            </div>
            <div className="duration-300 ease-in-out hover:scale-[110%]">
              <Button
                width="60px"
                height="40px"
                onClick={toggleEditMode}
                backgroundColor="white"
              >
                <span className="text-[#A57865] !important">취소</span>
              </Button>
            </div>
          </div>
        ) : (
          <>
            {/* 사용자가 작성한 글일 경우에만 수정 가능(현재는 테스트 중이므로 그냥 true로 설정)*/}
            {/* {isUser &&  */}
            {true && (
              <div className="duration-300 ease-in-out hover:scale-[110%]">
                <Button width="60px" height="40px" onClick={toggleEditMode}>
                  수정
                </Button>
              </div>
            )}
            <div className="duration-300 ease-in-out hover:scale-[110%]">
              <Button
                width="60px"
                height="40px"
                backgroundColor="white"
                onClick={() => navigate('/notice')}
              >
                <span className="text-[#A57865] !important">목록</span>
              </Button>
            </div>
            <div
              className="duration-300 ease-in-out hover:scale-[110%]"
              onClick={() => navigate('/write')}
            >
              <Button width="60px" height="40px">
                글쓰기
              </Button>
            </div>
          </>
        )}
      </div>
    </div>
  );
}
