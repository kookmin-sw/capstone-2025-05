import { useQuery } from '@tanstack/react-query';
import spotifyApi from '../../Utils/spotifyApi';

const fetchSearch = (keyword) => {
  return spotifyApi.get('/search', {
    params: {
      q: keyword,
      type: 'track',
    },
  });
};

export const useSearchPlaylistsQuery = (keyword) => {
  return useQuery({
    queryKey: ['search', keyword],
    select: (result) => result.data.tracks.items,
    queryFn: () => fetchSearch(keyword),
    staleTime: 300000,
  });
};
