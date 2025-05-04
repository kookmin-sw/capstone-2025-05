import { useQuery } from '@tanstack/react-query';
import api from '../../../../Utils/api';

const fetchScraps = (uid) => {
  return api.get(`/my-scraps`, { params: { uid } });
};

export const useMyscrapQuery = (uid) => {
  return useQuery({
    queryKey: ['scrap', uid],
    queryFn: () => fetchScraps(uid),
    select: (result) => result.data.my_scraps,
    staleTime: 300000,
  });
};
