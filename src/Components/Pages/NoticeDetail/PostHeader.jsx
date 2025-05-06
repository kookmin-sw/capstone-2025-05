import React from 'react';
import { FaTrashAlt, FaFlag } from 'react-icons/fa';
import Modal from '../../Modal/Modal';
import Button from '../../Button/Button';
import ReportModal from '../../Modal/ReportModal';
import swal from 'sweetalert';
import PostMetaTable from './PostMetaTable';
import { useAuth } from '../../../Context/AuthContext';

export default function PostHeader({
  post,
  isEditing,
  editedTitle,
  setEditedTitle,
  isModalOpen,
  setIsModalOpen,
  isAdmin,
  isUser,
  isWriteReportOpen,
  setIsWriteReportOpen,
  postReportMutate,
  deletePostMutate,
  editedContent,
  setEditedContent,
}) {
  const { uid } = useAuth();
  const insertSpaces = (text, maxLength = 50) => {
    return text.replace(new RegExp(`(.{${maxLength}})`, 'g'), '$1 ');
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
          swal({
            text: '신고가 접수되었습니다🫡',
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
            text: '신고 처리 중 오류가 발생했습니다',
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
  };

  const deletePost = () => {
    deletePostMutate(
      { post_id: post.id, uid },
      {
        onSuccess: () => {
          swal({
            text: '게시물 삭제 완료',
            icon: 'success',
            buttons: {
              confirm: {
                text: '확인',
                className: 'custom-confirm-button',
              },
            },
          });
        },
        onError: (error) => {
          swal({
            text: `게시물 삭제 중 오류 발생:${error}`,
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
    setIsModalOpen(false);
  };

  return (
    <div className="sm:w-[80%] lg:w-[60%] md:w-[70%] bg-white shadow-lg rounded-lg ">
      <div className="h-[60vh]">
        <div
          id="title"
          className=" border-b border-[#C4A08E] px-4 py-4 min-h-[8vh]"
        >
          {isEditing ? (
            <input
              type="text"
              value={editedTitle}
              onChange={(e) => setEditedTitle(e.target.value)}
              className="border p-2 w-full"
            />
          ) : (
            <div className="flex justify-between items-center py-2 px-2">
              <h1 className="text-4xl font-bold align-middle">{post.title}</h1>
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
              <Modal isOpen={isModalOpen} onClose={() => setIsModalOpen(false)}>
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
                        <span className="text-[#A57865] !important">확인</span>
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
        <PostMetaTable post={post} />
        {isEditing ? (
          <textarea
            value={editedContent}
            onChange={(e) => setEditedContent(e.target.value)}
            className="p-2 block mx-auto my-2 w-[98%] h-1/2 border resize-none"
          />
        ) : (
          <textarea
            className="mt-6 w-[98%] block mx-auto w-full h-1/2 focus:outline-none focus:border-none resize-none"
            readOnly
            wrap="soft"
          >
            {post.content}
          </textarea>
        )}
      </div>
    </div>
  );
}
