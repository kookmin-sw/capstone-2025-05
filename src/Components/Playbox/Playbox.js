import React, { useEffect, useState } from 'react';
import Box from '../Box/Box';
import { Link } from 'react-router';
import { useSpotifyPlayer } from '../../Context/SpotifyContext';
import PlayButton from '../Button/PlayButton';
import PauseButton from '../Button/PauseButton';
import { useSpotifyPlayback } from '../../Hooks/Music/useSpotifyPlayback';
import PlayerBar from '../PlayerBar/PlayerBar';

export default function Playbox({
  img,
  title,
  artist,
  playurl,
  playerTarget,
  setPlayerTarget,
  isSelected,
}) {
  const { deviceId, isReady, authUrl, player } = useSpotifyPlayer();
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
    setPlayerTarget({ img, title, artist, playurl });
    const isSameTrack = playerTarget?.playurl === playurl;

    sendPlaybackCommand({
      action: 'play',
      body: isSameTrack
        ? {} //같은 트랙이면 이어서 재생
        : {
            uris: [playurl], // 새로운 트랙이면 처음부터
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

  useEffect(() => {
    if (!isSelected) {
      setPlay(false);
    }
  }, [isSelected]);
  return (
    <Box width="220px" height="288px">
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
      {isSelected && title == playerTarget.title && (
        <PlayerBar
          title={playerTarget.title}
          artist={playerTarget.artist}
          image={playerTarget.img}
          isPlay={play}
          handlePlay={handlePlay}
          handlePause={handlePause}
          player={player}
        />
      )}
    </Box>
  );
}
