import { useQuery } from '@tanstack/react-query';
import api from '../../Utils/api';

const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

const fetchTrendMusic = () => {
  return api.get(`${BACKEND_URL}/spotify/new-releases`);
};

export const useNewReleases = () => {
  return useQuery({
    queryKey: ['TrendMusic'],
    queryFn: () => fetchTrendMusic(),
    select: (result) => result.data.albums.items,
    staleTime: 300000,
  });
};
