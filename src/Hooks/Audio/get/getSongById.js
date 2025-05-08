import { useQuery } from '@tanstack/react-query';
import audioApi from '../../../Utils/audioApi';
// id는 postId
const fetchSong = (song_id) => {
  return audioApi.get(`/songs/${song_id}`);
};

export const useSongByIdQuery = (song_id) => {
  return useQuery({
    queryKey: ['song', song_id],
    queryFn: () => fetchSong(song_id),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
