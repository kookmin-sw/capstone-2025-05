import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

/*게시글 수정하기 */
const editComment = ({ comment_id, editCommentData }) => {
  return api.put(`/comment/${comment_id}`, editCommentData);
};

export const useEditCommentMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: editComment,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('editComment', variables.comment_id); // 캐시된 데이터 갱신
    },
  });
};
