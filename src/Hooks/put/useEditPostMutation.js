import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const updatePost = ({
  id,
  editedTitle,
  writer,
  write_time,
  view,
  editedContent,
  commentNum,
}) => {
  return api.put(`post/posts/${id}`, {
    uid: 'userid',
    게시판종류이름: 'string',
    제목: editedTitle,
    작성자: writer,
    작성일시: write_time,
    조회수: view,
    내용: editedContent,
    댓글갯수: commentNum,
    비밀번호: '1234', //추후에 바꿀것
    좋아요수: 0, //추후에 바꿀것
  });
};

export const useEditPostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: updatePost,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('postNoticeBoard'); // 캐시된 데이터 갱신
    },
  });
};
