import { useQuery } from '@tanstack/react-query';
import api from '../Utils/api';

const fetchPostInfo = () => {
  return api.get(`/noticeBoard`);
};

export const usePostInfoQuery = () => {
  return useQuery({
    queryKey: ['post-info'],
    queryFn: () => fetchPostInfo(),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
