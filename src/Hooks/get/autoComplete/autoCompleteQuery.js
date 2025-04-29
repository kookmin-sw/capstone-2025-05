import { useQuery } from '@tanstack/react-query';
import api from '../../../Utils/api';

const fetchAuto = (query) => {
  return api.get(`/autocomplete?query=${encodeURIComponent(query)}`);
};

export const useAutoCompleteQuery = (query) => {
  return useQuery({
    queryKey: ['autoComplete', query],
    queryFn: () => fetchAuto(query),
    enabled: !!query, // query가 없을 땐 요청하지 않음
    select: (result) => result.data.suggestions || [],
    staleTime: 300000,
  });
};
