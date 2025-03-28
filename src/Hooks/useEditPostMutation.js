import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../Utils/api';

const updatePost = ({ post, editedTitle, editedContent }) => {
  return api.put(`/posts/${post.id}`, {
    id: post.id,
    제목: editedTitle,
    작성자: post.writer,
    작성일시: post.write_time,
    조회수: post.view,
    내용: editedContent,
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
