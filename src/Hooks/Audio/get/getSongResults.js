import { useQuery } from '@tanstack/react-query';
import audioApi from '../../../Utils/audioApi';

const fetchSong = (song_id) => {
  return audioApi.get(`/api/v1/song/${song_id}/results`);
};

export const useSongResultsQuery = (song_id) => {
  return useQuery({
    queryKey: ['songResults', song_id],
    queryFn: () => fetchSong(song_id),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
