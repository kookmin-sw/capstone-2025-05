// hooks/useKoreaTopTracks.js
import { useQuery } from '@tanstack/react-query';
import spotifyApi from '../../Utils/spotifyApi';
import axios from 'axios';

const top_playlist_id = '4cRo44TavIHN54w46OqRVc'; //우리나라 top50음악Id

export const useKoreaTopTracks = () => {
  return useQuery({
    queryKey: ['koreaTopTracks'],
    queryFn: () =>
      spotifyApi.get(`/playlists/${top_playlist_id}/tracks?limit=20`),
    select: (res) =>
      res.data.items.map((item) => ({
        id: item.track.id,
        title: item.track.name,
        artist: item.track.artists.map((a) => a.name).join(', '),
        cover: item.track.album.images[0]?.url,
        preview_url: item.track.preview_url,
        uri: item.track.uri,
        id: item.id,
      })),
    staleTime: 300000,
  });
};
