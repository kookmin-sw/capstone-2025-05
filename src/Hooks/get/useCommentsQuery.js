import { useQuery } from '@tanstack/react-query';
import api from '../../Utils/api';
// id는 postId
const fetchComments = (post_id) => {
  return api.get(`post/posts/${post_id}`);
};

export const useComentsQuery = (id) => {
  return useQuery({
    queryKey: ['comments', id],
    queryFn: () => fetchComments(id),
    select: (result) => result.data.data.comments,
    staleTime: 300000,
  });
};
