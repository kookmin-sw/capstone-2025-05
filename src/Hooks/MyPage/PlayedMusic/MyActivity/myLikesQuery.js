import { useQuery } from '@tanstack/react-query';
import api from '../../../../Utils/api';

const fetchLikes = (uid) => {
  return api.get(`/my-likes`, { params: { uid } });
};

export const useMylikeQuery = (uid) => {
  return useQuery({
    queryKey: ['mylike', uid],
    queryFn: () => fetchLikes(uid),
    select: (result) => result.data.my_likes,
    staleTime: 300000,
  });
};
