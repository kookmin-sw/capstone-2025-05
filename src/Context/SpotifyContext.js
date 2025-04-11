import React, { createContext, useState, useEffect, useContext } from 'react';

const SpotifyPlayerContext = createContext();

export const useSpotifyPlayer = () => useContext(SpotifyPlayerContext);

export const SpotifyPlayerProvider = ({ token, children }) => {
  const [player, setPlayer] = useState(null);
  const [deviceId, setDeviceId] = useState(null);
  const [isReady, setIsReady] = useState(false);

  useEffect(() => {
    if (!token) return;

    window.onSpotifyWebPlaybackSDKReady = () => {
      const newPlayer = new window.Spotify.Player({
        name: 'React Spotify Player',
        getOAuthToken: (cb) => cb(token),
        volume: 0.5,
      });

      newPlayer.addListener('ready', ({ device_id }) => {
        console.log('✅ SDK Ready - device_id:', device_id);
        setDeviceId(device_id);
        setIsReady(true);
      });

      newPlayer.addListener('initialization_error', ({ message }) => {
        console.error('🚫 init error', message);
      });

      newPlayer.addListener('authentication_error', ({ message }) => {
        console.error('🚫 auth error', message);
      });

      newPlayer.connect();
      setPlayer(newPlayer);
    };

    // Spotify SDK 스크립트 로드
    const script = document.createElement('script');
    script.src = 'https://sdk.scdn.co/spotify-player.js';
    script.async = true;
    document.body.appendChild(script);
  }, [token]);

  return (
    <SpotifyPlayerContext.Provider value={{ player, deviceId, isReady }}>
      {children}
    </SpotifyPlayerContext.Provider>
  );
};
