import axios from 'axios';

const CLIENT_ID = process.env.REACT_APP_SPOTIFY_CLIENT_ID;
const CLIENT_SECRET = process.env.REACT_APP_SPOTIFY_CLIENT_SECRET;

const getSpotifyToken = async () => {
  const url = 'https://accounts.spotify.com/api/token';
  const params = new URLSearchParams();
  params.append('grant_type', 'client_credentials');
  params.append('client_id', CLIENT_ID);
  params.append('client_secret', CLIENT_SECRET);

  try {
    const response = await axios.post(url, params, {
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    });

    return {
      accessToken: response.data.access_token,
      expiresAt: Date.now() + response.data.expires_in * 1000, // 만료 시간(1시간임)
    };
  } catch (error) {
    console.error('Error fetching token:', error);
    return error;
  }
};

const spotifyApi = axios.create({
  baseURL: 'https://api.spotify.com/v1',
  headers: { 'Content-Type': 'application/json' },
});

let accessToken = null;
let tokenExpiresAt = null;

const initializeSpotifyApi = async () => {
  const tokenData = await getSpotifyToken();
  if (tokenData) {
    accessToken = tokenData.accessToken;
    tokenExpiresAt = tokenData.expiresAt;
    spotifyApi.defaults.headers['Authorization'] = `Bearer ${accessToken}`;
  }
};

spotifyApi.interceptors.request.use(
  async (config) => {
    if (!accessToken || Date.now() >= tokenExpiresAt) {
      console.log('Access Token 갱신 중...');
      await initializeSpotifyApi();
      config.headers['Authorization'] = `Bearer ${accessToken}`;
    }
    return config;
  },
  (error) => Promise.reject(error),
);

initializeSpotifyApi();

export default spotifyApi;
