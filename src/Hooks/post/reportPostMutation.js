import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postReport = ({ post_id, reason }) => {
  return api.post(`/report/post/${post_id}?reason=${reason}`, {
    post_id,
    reason,
  });
};

export const useReportPostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postReport,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['post/report', variables.post_id]); // 캐시된 데이터 갱신
    },
  });
};
