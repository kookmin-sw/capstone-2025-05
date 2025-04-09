import React, { useState } from 'react';
import Box from '../Box/Box';
import { Link } from 'react-router';
import { useSpotifyPlayer } from '../../Context/SpotifyContext';
import PlayButton from '../Button/PlayButton';
import PauseButton from '../Button/PauseButton';

export default function Playbox({ img, title, artist, index, playurl }) {
  const { deviceId, isReady, authUrl } = useSpotifyPlayer();
  const [play, setPlay] = useState(false);
  const token = localStorage.getItem('spotify_access_token');
  const handlePlay = async () => {
    setPlay(true);
    if (!isReady || !deviceId) {
      alert('🎧 Spotify Player가 아직 준비되지 않았어요!');
      return;
    }

    try {
      const res = await fetch(
        `https://api.spotify.com/v1/me/player/play?device_id=${deviceId}`,
        {
          method: 'PUT',
          headers: {
            Authorization: `Bearer ${token}`,
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            uris: [playurl],
          }),
        },
      );
      if (res.status === 401) {
        console.log('토큰 만료! 다시 로그인');
        localStorage.removeItem('spotify_access_token');
        window.location.href = authUrl;
      } else if (!res.ok) {
        const err = await res.json();
        console.log('❌ 다른 오류 발생:', err);
      }
    } catch (error) {
      console.error('💥 fetch 실패:', error);
    }
  };

  const handlePause = async () => {
    setPlay(false);
    if (!isReady || !deviceId) {
      alert('🎧 Spotify Player가 아직 준비되지 않았어요!');
      return;
    }

    try {
      const res = await fetch(
        `https://api.spotify.com/v1/me/player/pause?device_id=${deviceId}`,
        {
          method: 'PUT',
          headers: {
            Authorization: `Bearer ${token}`,
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            uris: [playurl],
          }),
        },
      );
      if (res.status === 401) {
        console.log('토큰 만료! 다시 로그인');
        localStorage.removeItem('spotify_access_token');
        window.location.href = authUrl;
      } else if (!res.ok) {
        const err = await res.json();
        console.log('❌ 다른 오류 발생:', err);
      }
    } catch (error) {
      console.error('💥 fetch 실패:', error);
    }
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
        {/* 플레이 버튼 */}
        {/* <button id="play-bttn" className="rounded-full" onClick={handlePlay}>
          <img src={playButton} className="overflow-hidden" />
        </button> */}
        {!play ? (
          <PlayButton onClick={handlePlay} />
        ) : (
          <PauseButton onClick={handlePause} />
        )}
      </div>
    </Box>
  );
}
