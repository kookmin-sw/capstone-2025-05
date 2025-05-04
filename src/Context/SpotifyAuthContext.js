import React, { createContext, useContext, useState, useEffect } from 'react';
import axios from 'axios';

const SpotifyAuthContext = createContext({
  accessToken: null,
  isReady: false,
});

export const SpotifyAuthProvider = ({ children }) => {
  const [accessToken, setAccessToken] = useState(null);
  const [isReady, setIsReady] = useState(false);

  const initializeSpotify = async () => {
    const params = new URLSearchParams();
    params.append('grant_type', 'client_credentials');
    params.append('client_id', process.env.REACT_APP_SPOTIFY_CLIENT_ID);
    params.append('client_secret', process.env.REACT_APP_SPOTIFY_CLIENT_SECRET);

    try {
      const response = await axios.post(
        'https://accounts.spotify.com/api/token',
        params,
        {
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
          },
        },
      );

      setAccessToken(response.data.access_token);
      setIsReady(true);
    } catch (err) {
      console.error('Spotify 토큰 요청 실패:', err);
    }
  };

  useEffect(() => {
    initializeSpotify();
  }, []);

  return (
    <SpotifyAuthContext.Provider value={{ accessToken, isReady }}>
      {children}
    </SpotifyAuthContext.Provider>
  );
};

export const useSpotify = () => useContext(SpotifyAuthContext);
