import { useQuery } from '@tanstack/react-query';
import analysisApi from '../../../Utils/analysisApi';

const fetchSong = (song_id) => {
  return analysisApi.get(`/song/${song_id}/results`);
};

export const useSongResultsQuery = (song_id) => {
  return useQuery({
    queryKey: ['songResults', song_id],
    queryFn: () => fetchSong(song_id),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
