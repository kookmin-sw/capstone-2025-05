import React from 'react';
import { useSongByIdQuery } from '../../Hooks/Audio/get/getSongById';
import Box from '../Box/Box';
import { Link } from 'react-router-dom';
import Cover_1 from '../../Assets/Main/album/iveCover.svg';
const COVER_URL = process.env.REACT_APP_COVER_URL;

export default function BigAlbum({ record, index }) {
  const { data: songInfo } = useSongByIdQuery(record.song_id);
  return (
    <Box
      key={record.song_id || index}
      overwrite="p-4 flex flex-col justify-between w-[90%]"
    >
      <div className="flex flex-col justify-center items-center m-4">
        <div className="w-full">
          <Link
            to={`/ranking`}
            state={{ song_name: songInfo.title }}
            className="w-full flex justify-center"
          >
            <img
              src={COVER_URL + '/' + songInfo.thumbnail || Cover_1}
              alt="Album Cover"
              className="object-cover w-[280px] h-[280px] rounded"
            />
          </Link>
          <div className="flex flex-col w-full mt-2 ml-2">
            <span className="text-lg font-semibold truncate">
              {songInfo.title}
            </span>
            <span className="text-sm mt-[-2px] truncate">
              {songInfo.artist || 'Unknown Artist'}
            </span>
          </div>
        </div>
      </div>
    </Box>
  );
}
