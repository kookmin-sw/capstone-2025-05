import { useQuery } from '@tanstack/react-query';
import api from '../../../Utils/api';

const fetchRecords = (uid) => {
  return api.get(`/records/all`, {
    params: { uid },
  });
};

export const useRecordQuery = (uid) => {
  return useQuery({
    queryKey: ['AllRecord', uid],
    queryFn: () => fetchRecords(uid),
    select: (result) => result.data,
    staleTime: 0,
    cacheTime: 0,
  });
};
