import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const putUnlike = ({ postid }) => {
  return api.put(`/post/posts/${postid}/unlike`);
};

export const useUnlikePutMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: putUnlike,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['put/unlike', variables.postid]); // 캐시된 데이터 갱신
    },
  });
};
