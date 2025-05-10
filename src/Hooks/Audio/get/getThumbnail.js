import { useQuery } from '@tanstack/react-query';
import audioApi from '../../../Utils/audioApi';

const fetchthumbnail = (song_id) => {
  return audioApi.get(`/songs/${song_id}/thumbnail`);
};

export const useThumbnailQuery = (song_id) => {
  return useQuery({
    queryKey: ['thumbnail', song_id],
    queryFn: () => fetchthumbnail(song_id),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
