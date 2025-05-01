import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const deleteLike = ({ postid }) => {
  return api.post(`/posts/${postid}/like`);
};

export const useDeletelikeMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: deleteLike,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['delete/like', variables.postid]); // 캐시된 데이터 갱신
    },
  });
};
