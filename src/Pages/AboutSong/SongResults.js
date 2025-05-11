import React from 'react';

import results from '../../Data/songDetail.json';
import { useLocation, useNavigate } from 'react-router-dom';
import { useSongByIdQuery } from '../../Hooks/Audio/get/getSongById';
import { useSongResultsQuery } from '../../Hooks/Audio/get/getSongResults';
import FeedbackCard from '../../Components/Card/FeedbackCard';

const COVER_URL = process.env.REACT_APP_COVER_URL;

export default function SongResults() {
  const navigate = useNavigate();
  const location = useLocation();
  const { song_id, title } = location.state;
  console.log(song_id);
  const { data: songInfo } = useSongByIdQuery(song_id);
  const { data: results } = useSongResultsQuery(song_id);

  console.log(results, '곡 결과');

  return (
    <div className="min-h-screen bg-[#f1ede5] p-10 flex flex-col items-center">
      <div className="flex items-center gap-4 bg-white p-4 rounded-lg shadow mb-6 w-full lg:w-[60%]">
        <img
          src={COVER_URL + '/' + songInfo?.thumbnail}
          alt="Cover"
          className="w-20 h-20 object-cover rounded"
        />
        <div>
          <p className="text-sm text-gray-500">분석 대상 곡</p>
          <h2 className="text-xl font-bold text-[#463936]">
            {songInfo?.title}
          </h2>
          <p className="text-sm text-gray-600">
            {songInfo?.artist || 'Unknown Artist'}
          </p>
        </div>
      </div>

      {results?.length === 0 ? (
        <p className="text-gray-500">분석 결과가 없습니다.</p>
      ) : (
        <div className="flex flex-col items-center space-y-6">
          {results &&
            results.map((result, idx) => (
              <FeedbackCard result={result} idx={idx} />
            ))}
        </div>
      )}
    </div>
  );
}
