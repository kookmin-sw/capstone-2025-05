import React, { useState } from 'react';
import { useSongByIdQuery } from '../../Hooks/Audio/get/getSongById';
import FeedbackCard from '../Card/FeedbackCard';
import SongCard from './SongCard';

const COVER_URL = process.env.REACT_APP_COVER_URL;

export default function ResultCard({ result, idx }) {
  const { data: songInfo } = useSongByIdQuery(result.song_id);
  const [isExpanded, setIsExpanded] = useState(false);

  return (
    <div className="w-full flex flex-col items-center space-y-4">
      {songInfo && (
        <SongCard
          cover_url={COVER_URL + '/' + songInfo.thumbnail}
          title={songInfo.title}
          artist={songInfo.artist}
          onClick={() => setIsExpanded((prev) => !prev)}
        />
      )}
      {isExpanded && <FeedbackCard idx={idx} result={result} />}
    </div>
  );
}
