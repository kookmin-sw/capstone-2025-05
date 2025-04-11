import axios from 'axios';
// 자유게시판 전체 데이터
const PlaySpotifyApi = (token, playurl) =>
  axios.create({
    baseURL: '`https://api.spotify.com/v1/me/player/',
    headers: {
      Authorization: `Bearer ${token}`,
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      uris: [playurl],
    }),
  });

export default PlaySpotifyApi;
