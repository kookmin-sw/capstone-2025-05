import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const deletePost = ({ post_id, uid }) => {
  return api.delete(`/posts/${post_id}`, {
    params: {
      uid,
    },
  });
};

export const useDeletePostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: deletePost,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('deletePost', variables.post_id); // 캐시된 데이터 갱신
    },
  });
};
