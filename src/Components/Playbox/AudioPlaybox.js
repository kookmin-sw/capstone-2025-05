import React, { useEffect, useState } from 'react';
import Box from '../Box/Box';
import { Link, useNavigate } from 'react-router-dom';
import PlayButton from '../Button/PlayButton';

export default function AudioPlaybox({ img, title, artist, playurl, song_id }) {
  const navigate = useNavigate();
  const handlePlay = () => {
    // 곡에 대한 유저들의 연주기록
    navigate('/songResult', { state: { song_id: song_id, title: title } });
  };

  console.log(img, '이미지url이자식아');

  return (
    <Box width="220px" height="288px">
      <div className="flex justify-center items-center mt-4">
        <div
          className="relative group w-[200px] h-[200px] cursor-pointer"
          onClick={handlePlay}
        >
          <img
            src={img}
            alt="Album Cover"
            className="w-[200px] h-[200px] object-cover"
          />
          <div className="absolute top-0 left-0 w-full h-full bg-black bg-opacity-60 text-white flex items-center scale-[108%] rounded-[10px] justify-center opacity-0 group-hover:opacity-100 group-hover:-translate-y-2 stransition-opacity duration-300">
            <div className="flex flex-col items-center">
              <p className="text-md font-semibold text-center leading-tight">
                지금 내 연주, 얼마나 정확할까? <br />
              </p>
              <br />
              <small className="text-xs">아래 ▶ 버튼을 눌러보세요!</small>
            </div>
          </div>
        </div>
      </div>
      <div className="flex items-center justify-between px-4 mt-2">
        <div className="flex flex-col w-[140px]">
          <span className="text-lg font-semibold truncate">{title}</span>
          <span className="text-sm text-gray-500 mt-[-4px] truncate">
            {artist}
          </span>
        </div>
        <PlayButton onClick={handlePlay} />
      </div>
    </Box>
  );
}
