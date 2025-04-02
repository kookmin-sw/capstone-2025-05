import React from 'react';
import Box from '../Box/Box';
import { Link } from 'react-router';

export default function playbox({ img, title, artist, index }) {
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
      </div>
    </Box>
  );
}
