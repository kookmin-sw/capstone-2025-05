import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const deleteScrap = ({ post_id }) => {
  return api.post(`/posts/${post_id}/scrap`);
};

export const useDeleteScrapMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: deleteScrap,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['delete/scrap', variables.post_id]); // 캐시된 데이터 갱신
    },
  });
};
