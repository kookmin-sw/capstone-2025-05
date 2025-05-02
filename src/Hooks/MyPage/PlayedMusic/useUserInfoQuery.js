import { useQuery } from '@tanstack/react-query';
import api from '../../../Utils/api';

const fetchUser = (uid) => {
  return api.get(`/get-user-info`, {
    params: { uid },
  });
};

export const useUserQuery = (uid) => {
  console.log(uid, 'sd');
  return useQuery({
    queryKey: ['userInfo', uid],
    queryFn: () => fetchUser(uid),
    staleTime: 300000,
    select: (result) => result.data,
  });
};
