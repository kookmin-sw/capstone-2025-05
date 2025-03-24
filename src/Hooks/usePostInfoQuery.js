import { useQuery } from '@tanstack/react-query';
import api from '../Utils/api';

const fetchPostInfo = () => {
  return api.get(`/posts`);
};

export const usePostInfoQuery = () => {
  return useQuery({
    queryKey: ['postNoticeBoard'],
    queryFn: () => fetchPostInfo(),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
