import { useQuery } from '@tanstack/react-query';
import audioApi from '../../../Utils/audioApi';

const fetchSongs = () => {
  return audioApi.get(`/songs`);
};

export const useAllSongQuery = () => {
  return useQuery({
    queryKey: ['Allsongs'],
    queryFn: () => fetchSongs(),
    select: (result) => result.data,
    staleTime: 300000,
  });
};
