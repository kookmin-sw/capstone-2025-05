import { useQuery } from '@tanstack/react-query';
import spotifyApi from '../../Utils/spotifyApi';

const fetchTrendTracks = async () => {
  const res = await spotifyApi.get(`/browse/new-releases?country=KR&limit=10`);
  const albums = res.data.albums.items;

  const albumTracks = await Promise.all(
    albums.map(async (album) => {
      const tracksRes = await spotifyApi.get(`/albums/${album.id}/tracks`);
      const firstTrack = tracksRes.data.items[0];

      return {
        title: firstTrack?.name || album.name,
        artist: album.artists[0].name,
        cover: album.images[0]?.url,
        uri: firstTrack?.uri,
        id: firstTrack?.id,
      };
    }),
  );

  return albumTracks;
};

export const useNewReleases = () => {
  return useQuery({
    queryKey: ['TrendMusic'],
    queryFn: fetchTrendTracks,
    staleTime: 300000,
    select: (res) => res,
  });
};
