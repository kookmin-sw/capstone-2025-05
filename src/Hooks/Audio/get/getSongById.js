import { useQuery } from '@tanstack/react-query';
import mediaApi from '../../../Utils/audioApi';

const fetchSong = (song_id) => {
  return mediaApi.get(`/songs/${song_id}`);
};

export const useSongByIdQuery = (song_id) => {
  return useQuery({
    queryKey: ['song', song_id],
    queryFn: () => fetchSong(song_id),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
