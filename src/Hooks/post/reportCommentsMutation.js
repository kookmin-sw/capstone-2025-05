import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const commentReport = ({ commentId, reason }) => {
  return api.post(`/post/comments/${commentId}/report`, {
    reason,
  });
};

export const useReportPostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: commentReport,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['comment/report', variables.commentId]); // 캐시된 데이터 갱신
    },
  });
};
