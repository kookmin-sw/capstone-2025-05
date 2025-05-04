import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const deleteLike = ({ post_id, uid }) => {
  const numericPostId = parseInt(post_id, 10);

  return api.delete(`/posts/${numericPostId}/like`, {
    params: { uid },
  });
};

export const useDeletelikeMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: deleteLike,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['delete/like', variables.post_id]); // 캐시된 데이터 갱신
    },
  });
};
