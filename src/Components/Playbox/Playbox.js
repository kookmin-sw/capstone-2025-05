import React, { useEffect, useState } from 'react';
import Box from '../Box/Box';
import { Link } from 'react-router-dom';
import { useSpotifyPlayer } from '../../Context/SpotifyContext';
import PlayButton from '../Button/PlayButton';
import PauseButton from '../Button/PauseButton';
import { useSpotifyPlayback } from '../../Hooks/Music/useSpotifyPlayback';
import PlayerBar from '../PlayerBar/PlayerBar';
import swal from 'sweetalert';

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
    onError: (msg) => {
      swal({
        text:
          msg +
          '\n이 곡은 spotify정책으로 인해 이용불가합니다\n다른 곡을 재생시켜주세요',
        icon: 'warning',
        buttons: {
          confirm: {
            text: '확인',
            className: 'custom-confirm-button',
          },
        },
      });
      handlePause();
    },
    onTokenExpired: () => console.log('로그인 필요'),
  });

  const handlePlay = () => {
    setPlay(true);
    setPlayerTarget({ img, title, artist, playurl });
    const isSameTrack = playerTarget?.playurl === playurl;

    sendPlaybackCommand({
      authUrl,
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
      authUrl,
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
        <div className="relative group w-[200px] h-[200px] cursor-pointer">
          <Link
            to="/ranking"
            state={{
              song_name: title,
            }}
          >
            <img
              src={img}
              alt="Album Cover"
              className="w-[200px] h-[200px] object-cover"
            />
            <div className="absolute top-0 left-0 w-full h-full bg-black bg-opacity-60 text-white flex items-center justify-center opacity-0 group-hover:opacity-100 transition-opacity duration-300">
              <div className="flex flex-col items-center">
                <p className="text-md font-semibold text-center">
                  👆🏻 이미지 클릭 시 연주 랭킹 페이지로 이동
                </p>
                <br />
                <small className="text-xs">
                  * 스트리밍은 아래 ▶ 버튼 클릭
                </small>
              </div>
            </div>
          </Link>
        </div>
      </div>
      <div className="flex items-center justify-between px-4 mt-2">
        <div className="flex flex-col w-[140px]">
          <span className="text-lg font-semibold truncate">{title}</span>
          <span className="text-sm text-gray-500 mt-[-4px] truncate">
            {artist}
          </span>
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
