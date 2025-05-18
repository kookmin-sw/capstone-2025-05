import React, { useState } from 'react';
import { AiFillAlert, AiFillEdit } from 'react-icons/ai';
import ReportModal from '../Modal/ReportModal';
import { useReportCommentMutation } from '../../Hooks/post/reportCommentsMutation';
import { useEditCommentMutation } from '../../Hooks/put/editCommentMutation';
import swal from 'sweetalert';
import fakeImg from '../../Assets/Images/google_profile2.png';

export default function Review({
  comments,
  profile_image,
  isSelectable,
  isSelected,
  onSelect,
  isAdmin,
  isUser,
}) {
  const [isCommentReportOpen, setIsCommentReportOpen] = useState(false);
  const [isEditing, setIsEditing] = useState(false);
  const [editedContent, setEditedContent] = useState('');

  const { mutate: reportComment } = useReportCommentMutation();
  const { mutate: editComment } = useEditCommentMutation();

  // "2025년 04월 25일 06시 41분" → ISO 포맷으로 바꿔주는 함수
  const parseDateString = (dateString) => {
    const [year, month, day, hour, minute] = dateString
      .replace('년', '')
      .replace('월', '')
      .replace('일', '')
      .replace('시', '')
      .replace('분', '')
      .split(' ')
      .map(Number);

    // JS는 월을 0부터 시작해서 -1 필요 없음
    const date = new Date(year, month - 1, day, hour, minute);
    return date.toISOString();
  };

  const handleReport = (reason) => {
    reportComment(
      { comment_id: comments.id, reason },
      {
        onSuccess: () => {
          swal('✅', '댓글 신고가 접수되었습니다.', 'success');
        },
        onError: (error) => {
          swal('❌', `댓글 신고 실패: ${error.message}`, 'error');
        },
      },
    );
  };

  const handleEditClick = () => {
    setIsEditing(true);
    setEditedContent(comments.내용);
  };

  const handleSaveClick = () => {
    if (!editedContent.trim()) {
      alert('내용을 입력해주세요.');
      return;
    }

    const editCommentData = {
      uid: comments.uid,
      postid: comments.postid,
      내용: editedContent, // 내용만 변경하여 전달
      비밀번호: comments.비밀번호,
      작성일시: parseDateString(comments.작성일시),
      작성자: comments.작성자,
    };

    editComment(
      { comment_id: comments.id, editCommentData },
      {
        onSuccess: () => {
          swal('✅', '댓글이 수정되었습니다.', 'success');
          setIsEditing(false);
        },
        onError: (error) => {
          swal('❌', `댓글 수정 실패: ${error.message}`, 'error');
        },
      },
    );
  };

  return (
    <div className="flex items-center mb-8">
      {isSelectable && (isAdmin || isUser) && (
        <input
          type="checkbox"
          checked={isSelected}
          onChange={onSelect}
          className="mr-2"
        />
      )}
      <div className="w-[80px] aspect-square rounded-full overflow-hidden mr-2">
        <img
          src={profile_image ? profile_image : fakeImg}
          width="80"
          height="80"
          className="object-cover"
        />
      </div>
      <div className="flex flex-col">
        <strong>{comments.작성자}</strong>

        {isEditing ? (
          <div className="flex flex-col">
            <textarea
              value={editedContent}
              onChange={(e) => setEditedContent(e.target.value)}
              className="border rounded p-2 w-full h-20 mt-2"
            />
            <div className="flex mt-2 gap-2">
              <button
                onClick={handleSaveClick}
                className="px-4 py-2 bg-green-500 text-white rounded hover:bg-green-600"
              >
                저장
              </button>
              <button
                onClick={() => setIsEditing(false)}
                className="px-4 py-2 bg-gray-300 rounded hover:bg-gray-400"
              >
                취소
              </button>
            </div>
          </div>
        ) : (
          <p className="my-1">{comments.내용}</p>
        )}

        <div className="flex">
          <div className="relative group">
            <button onClick={() => setIsCommentReportOpen(true)}>
              <AiFillAlert />
            </button>
            <div className="absolute -translate-x-[50%] ml-2 px-2 py-1 text-sm bg-black text-white rounded opacity-0 group-hover:opacity-100 transition-opacity duration-200">
              신고하기
            </div>
            <ReportModal
              isOpen={isCommentReportOpen}
              onClose={() => setIsCommentReportOpen(false)}
              onConfirm={handleReport}
            />
          </div>

          <span className="mx-2">|</span>

          <div className="relative group">
            <button onClick={handleEditClick}>
              <AiFillEdit />
            </button>
            <div className="absolute -translate-x-[50%] ml-2 px-2 py-1 text-sm bg-black text-white rounded opacity-0 group-hover:opacity-100 transition-opacity duration-200">
              수정
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
