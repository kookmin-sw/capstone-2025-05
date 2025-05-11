import React from 'react';
import { useSongByIdQuery } from '../../Hooks/Audio/get/getSongById';

const COVER_URL = process.env.REACT_APP_COVER_URL;

export default function Album({ record, index }) {
  const { data: songInfo } = useSongByIdQuery(record.song_id);
  return (
    songInfo && (
      <li
        key={index}
        className="flex items-center justify-between pb-2 mb-2 min-w-0"
      >
        <img
          src={COVER_URL + '/' + songInfo.thumbnail}
          alt="Album"
          className="w-16 h-16 mr-8 mt-7"
        />
        <div className="flex flex-col justify-center flex-grow min-w-0">
          <p className="font-semibold text-[20px] mt-7 truncate overflow-hidden whitespace-nowrap text-ellipsis">
            {songInfo.title}
          </p>
          <p className="text-[15px] text-gray-600 mt-0 truncate">
            {songInfo.artist || '아티스트 미정'}
          </p>
        </div>
      </li>
    )
  );
}
