import { useQuery } from '@tanstack/react-query';
import api from '../../Utils/api';
// id는 postId
const fetchComments = (id) => {
  return api.get(`post/comments/${id}`);
};

export const useComentsQuery = (id) => {
  return useQuery({
    queryKey: ['comments', id],
    queryFn: () => fetchComments(id),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
