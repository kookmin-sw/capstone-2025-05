import React, { useState } from 'react';
import Box from '../Box/Box';
import { Link } from 'react-router';
import { useSpotifyPlayer } from '../../Context/SpotifyContext';
import PlayButton from '../Button/PlayButton';
import PauseButton from '../Button/PauseButton';
import { useSpotifyPlayback } from '../../Hooks/Music/useSpotifyPlayback';

export default function Playbox({ img, title, artist, index, playurl }) {
  const { deviceId, isReady, authUrl } = useSpotifyPlayer();
  const [play, setPlay] = useState(false);
  const token = localStorage.getItem('spotify_access_token');
  const { sendPlaybackCommand } = useSpotifyPlayback({
    token,
    deviceId,
    isReady,
    authUrl,
    onError: (msg) => alert(msg),
    onTokenExpired: () => console.log('로그인 필요'),
  });

  const handlePlay = () => {
    setPlay(true);
    sendPlaybackCommand({
      action: 'play',
      body: {
        uris: [playurl],
      },
    });
  };

  const handlePause = () => {
    setPlay(false);
    sendPlaybackCommand({
      action: 'pause',
      body: {},
    });
  };

  return (
    <Box key={index} width="220px" height="288px">
      <div className="flex justify-center items-center mt-4">
        <Link to="/feedback">
          <img
            src={img}
            alt="Album Cover"
            className="w-[200px] h-[200px] object-cover"
          />
        </Link>
      </div>
      <div className="flex items-center justify-between px-4 mt-2">
        <div className="flex flex-col w-[140px]">
          <span className="text-lg font-semibold truncate">{title}</span>
          <span className="text-lg mt-[-4px] truncate">{artist}</span>
        </div>
        {!play ? (
          <PlayButton onClick={handlePlay} />
        ) : (
          <PauseButton onClick={handlePause} />
        )}
      </div>
    </Box>
  );
}
