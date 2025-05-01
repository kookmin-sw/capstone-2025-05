import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const deleteComment = ({ comment_id }) => {
  return api.delete(`/comment/${comment_id}`);
};

export const useDeleteCommentMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: deleteComment,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['delete/comment', variables.comment_id]); // 캐시된 데이터 갱신
    },
  });
};
