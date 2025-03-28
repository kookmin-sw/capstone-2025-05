import React, { useEffect, useState } from 'react';
import MapleHeader from '../../Components/MapleHeader';

import Button from '../../Components/Button/Button';
import Review from '../../Components/Review/Review';
import profile from '../../Assets/Images/google_profile.png';
import profile2 from '../../Assets/Images/google_profile2.png';
import heart from '../../Assets/Images/heart.png';
import flag from '../../Assets/Images/flag.png';
import { FaAngleDown, FaAngleUp } from 'react-icons/fa6';
import ClipLoader from 'react-spinners/ClipLoader';
import { useComentsQuery } from '../../Hooks/get/useCommentsQuery';
import { useLocation, useNavigate } from 'react-router-dom';
import { useEditPostMutation } from '../../Hooks/put/useEditPostMutation';
import { FaRegMinusSquare, FaRegPlusSquare } from 'react-icons/fa';
import Input from '../../Components/Input/input';
import { usePostCommentsMutation } from '../../Hooks/post/usePostCommentsMutation';
import { useMutation } from '@tanstack/react-query';

// 하트 아이콘 저작권(fariha begum)
//깃발 아이콘 저작권(Hilmy Abiyyu A.)
export default function NoticeDetail() {
  const location = useLocation();
  const post = location.state;
  const { mutate } = useEditPostMutation();
  const { mutate: postCommentMutate } = usePostCommentsMutation();
  const navigate = useNavigate();
  const filterComments = () => {
    setIsLoading(true); // 로딩 시작
    if (comments) {
      setTimeout(() => {
        setIsShow((prevIsShow) => {
          const newIsShow = !prevIsShow;
          setFilteredComments(newIsShow ? comments : comments.slice(0, 3));
          setIsLoading(false); // 로딩 완료
          return newIsShow;
        });
      }, 500);
    }
  };

  const [isShow, setIsShow] = useState(false);
  const [comments, setComments] = useState();
  const [filteredComments, setFilteredComments] = useState([]);
  const [isLoading, setIsLoading] = useState(false);
  const { data: commentsInfo } = useComentsQuery(post.id);
  const [isEditing, setIsEditing] = useState(false);
  const [editedTitle, setEditedTitle] = useState(post.title);
  const [editedContent, setEditedContent] = useState(post.content);
  const [isPostComment, setIsPostComment] = useState(false);

  //수정 모드
  const toggleEditMode = () => {
    setIsEditing(!isEditing);
    setEditedTitle(post.title);
    setEditedContent(post.content);
  };

  //저장 기능
  const handleSave = () => {
    const id = post.id;
    const writer = post.writer;
    const write_time = post.write_time;
    const view = post.view;

    mutate(
      { id, editedTitle, writer, write_time, view, editedContent },
      {
        onSuccess: () => {
          post.title = editedTitle;
          post.content = editedContent;
          setIsEditing(false);
          alert('✅게시물 수정 완료');
        },
        onError: (error) => {
          console.error('게시물 수정 중 오류 발생:', error);
          alert('❎게시물 수정에 실패했습니다.');
        },
      },
    );
  };

  const handlePostComment = () => {
    const commentData = {
      id: post.id,
      uid: '랜덤',
      작성일시: new Date().toISOString(), // 현재 시간
      프로필이미지: 'profile',
      postid: post.id, // 현재 게시글 ID
      작성자: '누굴까', // 작성자 이름
      // 내용: 여기에는 댓글 작성창에 들어갈 내용 넣어야함, // 입력된 댓글 내용
    };

    postCommentMutate(commentData, {
      onSuccess: () => {
        alert('✅ 댓글이 추가되었습니다.');
      },
      onError: (error) => {
        console.error('댓글 작성 중 오류 발생:', error);
        alert('❎ 댓글 작성에 실패했습니다.');
      },
    });
  };

  const handlePlusComment = () => {
    const newIsPostComment = !isPostComment;
    return setIsPostComment(newIsPostComment);
  };

  useEffect(() => {
    if (commentsInfo) {
      setComments(commentsInfo);
      if (commentsInfo.length > 3)
        setFilteredComments(commentsInfo.slice(0, 3));
    }
  }, [commentsInfo]);

  return (
    <div>
      <MapleHeader />
      <section
        id="NoticeBoard"
        className="bg-[F0EDE6] flex flex-col items-center mt-10"
      >
        <div className="w-[80%]  bg-white shadow-lg  rounded-lg">
          <div className="min-h-[60vh]">
            <div
              id="title"
              className=" border-b border-[#C4A08E] p-4 min-h-[11vh]"
            >
              {isEditing ? (
                <input
                  type="text"
                  value={editedTitle}
                  onChange={(e) => setEditedTitle(e.target.value)}
                  className="border p-2 w-full"
                />
              ) : (
                <h1 className="text-2xl font-bold align-middle">
                  {post.title}
                </h1>
              )}
            </div>
            <table className="w-full  border-collapse min-h-[8vh]">
              <thead className="border-b border-[#C4A08E]">
                <tr>
                  <th className="bg-[#F0EDE6] border-r border-[#C4A08E] w-auto !important ">
                    글쓴이
                  </th>
                  <td>
                    <span className="ml-3">{post.writer}</span>
                  </td>
                  <th className="bg-[#F0EDE6] border-x border-[#C4A08E] w-auto !important">
                    작성 날짜
                  </th>
                  <td>
                    <span className="ml-3">{post.write_time}</span>
                  </td>
                  <th className="bg-[#F0EDE6] border-x border-[#C4A08E] w-auto !important">
                    조회수
                  </th>
                  <td>
                    <span className="ml-3">{post.view}</span>
                  </td>
                </tr>
              </thead>
            </table>
            {isEditing ? (
              <textarea
                value={editedContent}
                onChange={(e) => setEditedContent(e.target.value)}
                className="border p-2 w-full h-40"
              />
            ) : (
              <p className="mt-6 ml-2 whitespace-pre-line">{post.content}</p>
            )}
          </div>
        </div>

        <div className="flex w-[80%] justify-between mt-4">
          <div className="flex">
            <div className="mr-2 w-10">
              <img
                src={heart}
                className="duration-300 ease-in-out hover:scale-[110%]"
              />
            </div>
            <div className="w-10">
              <img
                src={flag}
                className="duration-300 ease-in-out hover:scale-[110%]"
              />
            </div>
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
                <div className="duration-300 ease-in-out hover:scale-[110%]">
                  <Button width="60px" height="40px" onClick={toggleEditMode}>
                    수정
                  </Button>
                </div>
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
      </section>
      <section id="reviews" className="flex flex-col items-center mt-14">
        {/* 댓글 헤더 */}
        <div
          id="review-caption"
          className="flex justify-between border-b-[2px] border-b-black w-[80%]"
        >
          <h className="font-bold text-xl mb-2">
            <strong>댓글 {comments?.length}개</strong>{' '}
          </h>
          {!isPostComment ? (
            <FaRegPlusSquare
              size={30}
              className="duration-300 ease-in-out hover:scale-[110%]"
              onClick={handlePlusComment}
            />
          ) : (
            <FaRegMinusSquare
              size={30}
              className="duration-300 ease-in-out hover:scale-[110%]"
              onClick={handlePlusComment}
            />
          )}
        </div>
        {/* 댓글 contents */}
        <div id="review-contents" className="mt-4 w-[80%]">
          {comments?.length <= 3 &&
            comments?.map((item, index) => (
              <Review
                comments={item}
                fakeImg={item?.프로필이미지 == 'profile' ? profile : profile2}
              />
            ))}
          {comments &&
            comments?.length > 3 &&
            filteredComments?.map((item, index) => (
              <Review
                comments={item}
                fakeImg={item.프로필이미지 == 'profile' ? profile : profile2}
              />
            ))}
        </div>
        {/* 댓글 펼치기 & 접기 */}
        {comments?.length > 3 && (
          <div>
            {
              <ClipLoader
                color="#0a0a0a"
                loading={isLoading}
                size={30}
                aria-label="Loading Spinner"
                data-testid="loader"
              />
            }
            {!isShow ? (
              <FaAngleDown size={'30px'} onClick={filterComments} />
            ) : (
              <FaAngleUp size={'30px'} onClick={filterComments} />
            )}
          </div>
        )}
        {/* 댓글 작성 박스 */}

        <div
          className={`flex w-[85%] fixed bottom-[10px]  transition-opacity ${
            isPostComment ? 'visible' : 'hidden'
          } ease-in-out duration-700`}
        >
          <div className="flex w-full relative left-[45px] ">
            <Input className="w-full" height="60px" />
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
      </section>
    </div>
  );
}
