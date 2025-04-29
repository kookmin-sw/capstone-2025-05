import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const commentReport = ({ comment_id, reason }) => {
  return api.post(`/report/comment/${comment_id}?reason=${reason}`, {
    comment_id: comment_id,
    reason,
  });
};

export const useReportCommentMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: commentReport,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['comment/report', variables.comment_id]); // 캐시된 데이터 갱신
    },
  });
};
