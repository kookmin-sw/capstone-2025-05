import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const updateViews = ({ post_id }) => {
  return api.put(`/post/posts/${post_id}/views`);
};

export const useViewMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: updateViews,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('increaseViews'); // 캐시된 데이터 갱신
    },
  });
};
