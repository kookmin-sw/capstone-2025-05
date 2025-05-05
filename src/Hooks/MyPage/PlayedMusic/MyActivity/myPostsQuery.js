import { useQuery } from '@tanstack/react-query';
import api from '../../../../Utils/api';

const fetchPosts = (uid) => {
  return api.get(`/my-posts`, { params: { uid } });
};

export const useMypostQuery = (uid) => {
  return useQuery({
    queryKey: ['mypost', uid],
    queryFn: () => fetchPosts(uid),
    select: (result) => result.data.my_posts,
    staleTime: 300000,
  });
};
