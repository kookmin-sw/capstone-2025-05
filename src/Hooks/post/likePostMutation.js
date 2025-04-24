import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postLike = ({ postid, uid }) => {
  return api.post(`/posts/${postid}/like?uid=${uid}`);
};

export const useLikePutMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postLike,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['post/like', variables.postid]); // 캐시된 데이터 갱신
    },
  });
};
