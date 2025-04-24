import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postLike = ({ post_id, uid }) => {
  return api.post(`/posts/${post_id}/like?uid=${uid}`);
};

export const useLikePutMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postLike,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['post/like', variables.post_id]); // 캐시된 데이터 갱신
    },
  });
};
