import { useQuery } from '@tanstack/react-query';
import analysisApi from '../../../Utils/analysisApi';

const fetchUserResult = (uid) => {
  return analysisApi.get(
    `/user/${uid}/results?limit=10&result_type=comparison`,
  );
};

export const useUserResultsQuery = (uid) => {
  return useQuery({
    queryKey: ['userResult', uid],
    queryFn: () => fetchUserResult(uid),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
