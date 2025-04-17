import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postReport = ({ postid, reason }) => {
  return api.post(`/post/posts/${postid}/report`, {
    reason,
  });
};

export const useReportPostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postReport,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['post/report', variables.postid]); // 캐시된 데이터 갱신
    },
  });
};
