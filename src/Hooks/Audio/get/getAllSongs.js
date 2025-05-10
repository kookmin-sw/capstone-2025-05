import { useQuery } from '@tanstack/react-query';
import mediaApi from '../../../Utils/audioApi';

const fetchSongs = () => {
  return mediaApi.get(`/songs`);
};

export const useAllSongQuery = () => {
  return useQuery({
    queryKey: ['Allsongs'],
    queryFn: () => fetchSongs(),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
