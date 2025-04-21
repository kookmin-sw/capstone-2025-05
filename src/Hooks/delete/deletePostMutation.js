import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const deletePost = ({ postid }) => {
  return api.delete(`/post/posts/${postid}`);
};

export const useDeletePostMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: deletePost,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('deletePost', variables.postid); // 캐시된 데이터 갱신
    },
  });
};
