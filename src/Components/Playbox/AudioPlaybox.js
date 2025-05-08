import React, { useEffect, useState } from 'react';
import Box from '../Box/Box';
import { Link, useNavigate } from 'react-router-dom';
import PlayButton from '../Button/PlayButton';

export default function AudioPlaybox({ img, title, artist, playurl, song_id }) {
  const handlePlay = () => {
    // 곡 상세 정보 페이지로
    navigate(`/song/${song_id}`);
  };
  const navigate = useNavigate();

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
            className="w-full h-full object-cover"
          />
          <div className="absolute top-0 left-0 w-full h-full bg-black bg-opacity-60 text-white flex items-center justify-center opacity-0 group-hover:opacity-100 transition-opacity duration-300">
            <p className="text-md font-semibold text-center leading-tight">
              지금 내 연주, 얼마나 정확할까? <br />
              <span className="font-bold">▶ 버튼을 눌러 분석해보세요!</span>
            </p>
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
