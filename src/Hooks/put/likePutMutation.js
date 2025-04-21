import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const putLike = ({ postid }) => {
  return api.put(`/post/posts/${postid}/like`);
};

export const useLikePutMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: putLike,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['putt/like', variables.postid]); // 캐시된 데이터 갱신
    },
  });
};
