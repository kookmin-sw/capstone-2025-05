import { useQuery } from '@tanstack/react-query';
import api from '../../../Utils/api';

const fetchUser = async (uid) => {
  try {
    const response = await api.get('/get-user-info', {
      params: { uid },
    });
    return response.data;
  } catch (error) {
    if (error.response?.status === 500) {
      return 'google';
    }
    throw error; // 다른 에러는 react-query가 인식하도록 throw
  }
};

export const useUserQuery = (uid) => {
  return useQuery({
    queryKey: ['userInfo', uid],
    queryFn: () => fetchUser(uid),
    staleTime: 300000,
  });
};
