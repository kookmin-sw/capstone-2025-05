import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postScrap = ({ post_id }) => {
  return api.post(`/posts/${post_id}/scrap`);
};

export const useScrapPostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postScrap,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['post/scrap', variables.post_id]); // 캐시된 데이터 갱신
    },
  });
};
