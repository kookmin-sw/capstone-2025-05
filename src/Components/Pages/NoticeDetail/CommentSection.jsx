import React from 'react';
import swal from 'sweetalert';
import { FaRegPlusSquare, FaRegMinusSquare } from 'react-icons/fa';
import { FaTrashAlt } from 'react-icons/fa';
import Review from '../../Review/Review';
import profile from '../../../Assets/Images/google_profile.png';
import profile2 from '../../../Assets/Images/google_profile2.png';
import Alert from '../../../Components/Alert/Alert';

export default function CommentSection({
  comments,
  isPostComment,
  isSelecting,
  setIsSelecting,
  selectedComments,
  setSelectedComments,
  deleteComment,
  setComments,
  setIsPostComment,
  isAdmin,
  setIsShow,
  isUser,
  filteredComments,
}) {
  //댓글 선택
  const handleSelectComment = (commentId) => {
    setSelectedComments(
      (prevSelected) =>
        prevSelected.includes(commentId)
          ? prevSelected.filter((id) => id !== commentId) // 이미 선택된 건 해제
          : [...prevSelected, commentId], // 새로 선택
    );
  };
  //댓글 삭제 취소
  const handleCancelSelect = () => {
    setIsSelecting(false);
    setSelectedComments([]);
  };
  //댓글 삭제
  const handleDeleteSelectedComments = () => {
    if (selectedComments.length === 0) {
      swal({
        text: '삭제할 댓글을 선택해주세요!😥',
        icon: 'error',
        buttons: {
          confirm: {
            text: '확인',
            className: 'custom-confirm-button',
          },
        },
      });

      return;
    }
    if (!isAdmin) {
      swal({
        text: '관리자 또는 본인이 쓴 댓글일 경우에만 삭제가능합니다!😥',
        icon: 'error',
        buttons: {
          confirm: {
            text: '확인',
            className: 'custom-confirm-button',
          },
        },
      });
      return;
    }
    swal({
      title: '⚠️ 정말 삭제할까요?',
      text: `선택한 댓글 ${selectedComments.length}개를 삭제하시겠습니까?`,
      icon: 'warning',
      buttons: {
        cancel: {
          text: '취소',
          visible: true,
          className: '',
          closeModal: true,
        },
        confirm: {
          text: '삭제하기',
          visible: true,
          className: 'custom-confirm-button', // 클래스 지정
          closeModal: true,
        },
      },
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
                swal({
                  text: '선택한 댓글이 삭제되었습니다🫡',
                  icon: 'success',
                  buttons: {
                    confirm: {
                      text: '확인',
                      className: 'custom-confirm-button',
                    },
                  },
                });
              },
              onError: () => {
                swal({
                  text: '댓글 삭제 실패 😥',
                  icon: 'error',
                  buttons: {
                    confirm: {
                      text: '확인',
                      className: 'custom-confirm-button',
                    },
                  },
                });
              },
            },
          );
        });
      }
    });
  };

  const handlePlusComment = () => {
    const newIsPostComment = !isPostComment;
    return setIsPostComment(newIsPostComment);
  };

  return (
    <>
      {/* 댓글 헤더 */}
      <div
        id="review-caption"
        className="flex justify-between border-b-[2px] border-b-black sm:w-[80%] lg:w-[60%] md:w-[70%]"
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
      <div
        id="review-contents"
        className="my-4 sm:w-[80%] lg:w-[60%] md:w-[70%]"
      >
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
    </>
  );
}
