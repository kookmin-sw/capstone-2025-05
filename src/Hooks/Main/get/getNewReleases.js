import { useQuery } from '@tanstack/react-query';
import api from '../../../Utils/api';

const fetchTrendMusic = () => {
  return api.get(`/api/spotify/new-releases`);
};

export const useNewReleases = () => {
  return useQuery({
    queryKey: ['TrendMusic'],
    queryFn: () => fetchTrendMusic(),
    select: (result) => result.data.albums.items,
    staleTime: 300000,
  });
};
