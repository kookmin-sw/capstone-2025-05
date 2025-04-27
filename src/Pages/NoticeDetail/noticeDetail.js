import React, { useEffect, useState } from 'react';
import Button from '../../Components/Button/Button';
import Review from '../../Components/Review/Review';
import profile from '../../Assets/Images/google_profile.png';
import profile2 from '../../Assets/Images/google_profile2.png';
import heart from '../../Assets/Images/heart.png';
import { FaAngleDown, FaAngleUp } from 'react-icons/fa6';
import ClipLoader from 'react-spinners/ClipLoader';
import { useComentsQuery } from '../../Hooks/get/useCommentsQuery';
import { useLocation, useNavigate } from 'react-router-dom';
import { useEditPutMutation } from '../../Hooks/put/editPutMutation';
import { useLikePutMutation } from '../../Hooks/post/likePostMutation';
import { useDeletelikeMutation } from '../../Hooks/delete/deletelikeMutation';
import { useReportPostMutation } from '../../Hooks/post/reportPostMutation';
import { FaRegMinusSquare, FaRegPlusSquare } from 'react-icons/fa';
import Input from '../../Components/Input/input';
import { usePostCommentsMutation } from '../../Hooks/post/usePostCommentsMutation';
import Alert from '../../Components/Alert/Alert';
import fill_heart from '../../Assets/Images/fill_heart.png';
import fill_bookmark from '../../Assets/MyPage/filledBookmark.svg';
import { FaFlag } from 'react-icons/fa6';
import bookmark from '../../Assets/bookmark.svg';
import { FaTrashAlt } from 'react-icons/fa';
import { useDeletePostMutation } from '../../Hooks/delete/deletePostMutation';
import Modal from '../../Components/Modal/Modal';
import { useScrapPostMutation } from '../../Hooks/post/scrapPostMutation';
import { useDeleteCommentMutation } from '../../Hooks/delete/deleteCommentMutation';
import swal from 'sweetalert';
import ReportModal from '../../Components/Modal/ReportModal';
import { useAuth } from '../../Context/AuthContext';

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

  const navigate = useNavigate();
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
  //게수물 수정 여부
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
  const [likeNum, setLikeNum] = useState(post.likes);
  const [isScrap, setIsScrap] = useState(false);
  const [isModalOpen, setIsModalOpen] = useState(false);

  //수정 모드
  const toggleEditMode = () => {
    setIsEditing(!isEditing);
    setEditedTitle(post.title);
    setEditedContent(post.content);
  };

  //댓글 선택
  const handleSelectComment = (commentId) => {
    setSelectedComments(
      (prevSelected) =>
        prevSelected.includes(commentId)
          ? prevSelected.filter((id) => id !== commentId) // 이미 선택된 건 해제
          : [...prevSelected, commentId], // 새로 선택
    );
  };

  //댓글 삭제
  const handleDeleteSelectedComments = () => {
    if (selectedComments.length === 0) {
      swal('', '삭제할 댓글을 선택해주세요!', 'error');

      return;
    }
    if (!isAdmin) {
      swal(
        '',
        '관리자 또는 본인이 쓴 댓글일 경우에만 삭제가능합니다!',
        'error',
      );
      return;
    }
    swal({
      title: '⚠️ 정말 삭제할까요?',
      text: `선택한 댓글 ${selectedComments.length}개를 삭제하시겠습니까?`,
      icon: 'warning',
      buttons: ['취소', '삭제하기'],
      dangerMode: true,
    }).then((willDelete) => {
      if (willDelete) {
        selectedComments.forEach((comment_id) => {
          deleteComment(
            { comment_id },
            {
              onSuccess: () => {
                setComments((prev) =>
                  prev.filter((comment) => comment.id !== comment_id),
                );
                setSelectedComments([]);
                setIsShow(false);

                handleCancelSelect(); // 선택모드 해제
                swal('', '선택한 댓글이 삭제되었습니다🫡', 'success');
              },
              onError: () => {
                swal('', '댓글 삭제 실패 😥', 'error');
              },
            },
          );
        });
      }
    });
  };

  //댓글 삭제 취소
  const handleCancelSelect = () => {
    setIsSelecting(false);
    setSelectedComments([]);
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
          swal('', '게시물 수정 완료 🫡', 'success');
        },
        onError: (error) => {
          console.error('게시물 수정 중 오류 발생:', error);
          swal('', '게시물 수정에 실패했습니다.😥', 'error');
        },
      },
    );
  };

  const deletePost = () => {
    deletePostMutate(
      { post_id: post.id },
      {
        onSuccess: () => {
          swal('🫡', '게시물 삭제 완료', 'success');
        },
        onError: (error) => {
          console.error('게시물 삭제 중 오류 발생:', error);
          swal('', '게시물 삭제에 실패했습니다.😥', 'error');
        },
      },
    );
    setIsModalOpen(false);
  };

  const handlePostComment = () => {
    setIsPostComment(false);
    const commentData = {
      uid: '랜덤', //로그인 uid로 해야됨
      작성일시: new Date().toISOString(), // 현재 시간
      postid: post.id, // 현재 게시글 ID
      작성자: '누굴까', // 작성자 이름
      내용: reviewComment.trim(),
      비밀번호: '1234',
    };

    postCommentMutate(commentData, {
      onSuccess: () => {
        swal('', '댓글이 추가되었습니다.🫡', 'success');
        setReviewComment('');
      },
      onError: (error) => {
        console.error('댓글 작성 중 오류 발생:', error);
        swal('', '댓글 추가 실패😥', error);
      },
    });
  };

  const handlePlusComment = () => {
    const newIsPostComment = !isPostComment;
    return setIsPostComment(newIsPostComment);
  };

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
      localStorage.setItem('heart', post.id);
      setClickLiked(true);
    } else {
      deletelikeMutate(
        { post_id: post.id },
        {
          onSuccess: () => {
            console.log('좋아요 취소 성공');
          },
          onError: (error) => console.log('좋아요 취소 실패ㅠㅠ', error),
        },
      );
      localStorage.removeItem('heart', post.id);
      setClickLiked(false);
    }
  };

  //북바크
  const handleScrap = () => {
    setIsScrap(!isScrap);
    if (!isScrap) {
      //북마크 클릭
      scrapPostMutate(
        { post_id: post.id, post_uid: post.uid },
        {
          onSuccess: () => {
            console.log('북마크 성공');
          },
          onError: (error) => console.log('북마크 실패ㅠㅠ', error),
        },
      );
    } else {
      deleteScrapMutate(
        { post_id: post.id },
        {
          onSuccess: () => {
            console.log('북마크 취소 성공');
          },
          onError: (error) => console.log('북마크 취소 실패ㅠㅠ', error),
        },
      );
    }
  };

  const handlePostReport = (reason) => {
    postReportMutate(
      {
        post_id: post.id,
        reason,
      },
      {
        onSuccess: () => {
          setIsWriteReportOpen(false);
          swal('', '신고가 접수되었습니다🫡', 'success');
        },
        onError: () => {
          swal('❌', '신고 처리 중 오류가 발생했습니다', 'error');
        },
      },
    );
  };

  useEffect(() => {
    if (commentsInfo) {
      setComments(commentsInfo);
      if (commentsInfo.length > 3)
        setFilteredComments(commentsInfo.slice(0, 3));
    }
  }, [commentsInfo]);

  useEffect(() => {
    if (localStorage.getItem('heart', post.id)) {
      setLiked(true);
    } else {
      setLiked(false);
    }
  }, [liked]);

  return (
    <div>
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
                <div className="flex justify-between items-center">
                  <h1 className="text-2xl font-bold align-middle">
                    {post.title}
                  </h1>
                  <div className="flex items-center justify-center">
                    {/* 관리자 계정이거나 현재 글이 사용자가 작성한 글 일경우 삭제 가능 */}
                    {(isAdmin || isUser) && (
                      <div className="flex items-center duration-300 ease-in-out hover:scale-[110%]">
                        <FaTrashAlt
                          size={20}
                          onClick={() => setIsModalOpen(true)}
                        />
                      </div>
                    )}
                    <div className="flex items-center duration-300 ease-in-out hover:scale-[110%]">
                      <FaFlag
                        size={20}
                        className="ml-3 text-black-500 cursor-pointer hover:scale-110 transition-transform"
                        onClick={() => setIsWriteReportOpen(true)}
                        title="신고하기"
                      />
                    </div>
                    <ReportModal
                      isOpen={isWriteReportOpen}
                      onClose={() => setIsWriteReportOpen(false)}
                      onConfirm={handlePostReport}
                    />
                  </div>
                  <Modal
                    isOpen={isModalOpen}
                    onClose={() => setIsModalOpen(false)}
                  >
                    <h2 className="text-xl font-bold mb-4">게시물 삭제</h2>
                    <p>게시물을 정말 삭제하시겠습니까?</p>
                    <div className="flex justify-between">
                      <div></div>
                      <div id="button-box" className="flex">
                        <div className="rounded-[10px] border border-[#A57865] mr-[2px]">
                          <Button
                            width={'40px'}
                            height={'30px'}
                            backgroundColor="white"
                            onClick={deletePost}
                          >
                            <span className="text-[#A57865] !important">
                              확인
                            </span>
                          </Button>
                        </div>
                        <div>
                          <Button
                            width={'40px'}
                            height={'30px'}
                            onClick={() => setIsModalOpen(false)}
                          >
                            취소
                          </Button>
                        </div>
                      </div>
                    </div>
                  </Modal>
                </div>
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
        {/*heart버튼과 신고하기 버튼 */}
        <div className="flex w-[80%] justify-between mt-4">
          <div className="flex">
            <button className="mr-2 w-10 h-[40px]" onClick={handleHeartBttn}>
              <img
                src={liked ? fill_heart : heart}
                className="duration-300 ease-in-out hover:scale-[110%]"
              />
              <span>{clickLiked ? likeNum + 1 : likeNum}</span>
            </button>
            <button className="w-10" onClick={handleScrap}>
              <img
                src={isScrap ? fill_bookmark : bookmark}
                className="duration-300 ease-in-out hover:scale-[110%]"
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
      </section>
      <section id="reviews" className="flex flex-col items-center mt-14">
        {/* 댓글 헤더 */}
        <div
          id="review-caption"
          className="flex justify-between border-b-[2px] border-b-black w-[80%]"
        >
          <h className="font-bold text-xl mb-2">
            <strong>댓글 {comments ? comments?.length : 0}개</strong>{' '}
          </h>
          <div className="flex items-center justify-center">
            {!isPostComment && !isSelecting ? (
              <FaRegPlusSquare
                size={30}
                className="duration-300 ease-in-out hover:scale-[110%]"
                onClick={handlePlusComment}
              />
            ) : (
              !isSelecting && (
                <FaRegMinusSquare
                  size={30}
                  className="duration-300 ease-in-out hover:scale-[110%]"
                  onClick={handlePlusComment}
                />
              )
            )}

            {!isSelecting ? (
              <div className="mx-2 duration-300 ease-in-out hover:scale-[110%]">
                <FaTrashAlt
                  onClick={() => {
                    setIsSelecting(true);
                  }}
                  size={25}
                />
              </div>
            ) : (
              <div className="flex mb-1">
                <button
                  onClick={handleDeleteSelectedComments}
                  className="bg-white text-[#A57865] px-3 py-1 rounded hover:bg-gray-100"
                >
                  선택 삭제 ({selectedComments.length}개)
                </button>
                <button
                  onClick={handleCancelSelect}
                  className="bg-[#A57865] text-white px-3 py-1 mx-1 rounded hover:bg-[#8c5c4e]"
                >
                  선택 취소
                </button>
              </div>
            )}
          </div>
        </div>
        {/* 댓글 contents */}
        <div id="review-contents" className="my-4 w-[80%]">
          {comments?.length <= 3 &&
            comments?.map((item, index) => (
              <Review
                comments={item}
                fakeImg={item.프로필이미지 == 'profile' ? profile : profile2}
                isSelected={selectedComments.includes(item.id)}
                isSelectable={isSelecting}
                onSelect={() => handleSelectComment(item.id)}
                isAdmin={isAdmin}
                isUser={isUser}
              />
            ))}
          {comments &&
            comments?.length > 3 &&
            filteredComments?.map((item, index) => (
              <Review
                comments={item}
                fakeImg={item.프로필이미지 == 'profile' ? profile : profile2}
                isSelected={selectedComments.includes(item.id)}
                isSelectable={isSelecting}
                onSelect={() => handleSelectComment(item.id)}
                isAdmin={isAdmin}
                isUser={isUser}
              />
            ))}
          {comments?.length == 0 && (
            <Alert
              width={'full'}
              height={'8'}
              overwrite={'rounded-[6px] py-4'}
              content={'댓글이 없습니다. 댓글을 작성해주세요..!'}
            />
          )}
        </div>

        {/* 댓글 작성 박스 */}

        {!isSelecting && (
          <div
            className={`flex w-[85%] fixed bottom-[25px]  transition-opacity ${
              isPostComment ? 'visible' : 'hidden'
            } ease-in-out duration-700`}
          >
            <div className="flex w-full relative left-[45px] ">
              <Input
                className="w-full"
                height="60px"
                placeholder="댓글을 작성해주세요..."
                value={reviewComment}
                onChange={(e) => {
                  setReviewComment(e.target.value);
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
              <FaAngleDown size={'30px'} onClick={() => filterComments(true)} />
            ) : (
              <FaAngleUp size={'30px'} onClick={() => filterComments(false)} />
            )}
          </div>
        )}
      </section>
    </div>
  );
}
