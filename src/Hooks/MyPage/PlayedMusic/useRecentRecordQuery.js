import { useQuery } from '@tanstack/react-query';
import api from '../../../Utils/api';

const fetchRecentRecords = (uid) => {
  return api.get(`/recent-4-record`, {
    params: { uid },
  });
};

export const useRecentRecordsQuery = (uid) => {
  return useQuery({
    queryKey: ['recentRecord', uid],
    queryFn: () => fetchRecentRecords(uid),
    select: (result) => result.data.recent_uploads,
    staleTime: 0,
    cacheTime: 0,
  });
};
