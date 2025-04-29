import { useQuery } from '@tanstack/react-query';
import api from '../../Utils/api';

const fetchTop = () => {
  return api.get('/posts/top-viewed');
};

export const useTopViewedPostsQuery = () => {
  return useQuery({
    queryKey: ['TopViewedPosts'],
    select: (result) => result.data,
    queryFn: () => fetchTop(),
    staleTime: 300000,
  });
};
