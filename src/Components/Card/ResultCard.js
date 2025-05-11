import React, { useState } from 'react';
import { useSongByIdQuery } from '../../Hooks/Audio/get/getSongById';
import FeedbackCard from '../Card/FeedbackCard';
import SongCard from './SongCard';

import { motion, AnimatePresence } from 'framer-motion';

const COVER_URL = process.env.REACT_APP_COVER_URL;

export default function ResultCard({ result, idx }) {
  const { data: songInfo } = useSongByIdQuery(result.song_id);
  const [isExpanded, setIsExpanded] = useState(false);

  return (
    <div className="w-full flex flex-col items-center mt-2">
      {songInfo && (
        <SongCard
          cover_url={COVER_URL + '/' + songInfo.thumbnail}
          title={songInfo.title}
          artist={songInfo.artist}
          onClick={() => setIsExpanded((prev) => !prev)}
        />
      )}
      <AnimatePresence>
        {isExpanded && (
          <motion.div
            initial={{ opacity: 0, height: 0 }}
            animate={{ opacity: 1, height: 'auto' }}
            exit={{ opacity: 0, height: 0 }}
            transition={{ duration: 0.3 }}
            className="flex justify-center"
          >
            <FeedbackCard result={result} idx={idx} />
          </motion.div>
        )}
      </AnimatePresence>
    </div>
  );
}
