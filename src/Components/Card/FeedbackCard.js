import React from 'react';
import { useNavigate } from 'react-router-dom';
export default function FeedbackCard({ result, idx }) {
  const navigate = useNavigate();
  return (
    <div
      key={idx}
      className="bg-white rounded-lg shadow-md p-6 hover:shadow-xl transition-shadow duration-200 lg:w-[60%]"
    >
      <div className="flex justify-between items-center mb-2">
        <h3 className="text-lg font-semibold text-gray-800">
          📊 비교 분석 -{' '}
          {new Date(result.result.created_at).toLocaleString('ko-KR', {
            timeZone: 'Asia/Seoul',
          })}
        </h3>
        <button
          className="text-sm text-[#a57865] hover:underline"
          onClick={() =>
            navigate(`/results/${result.task_id}`, {
              state: {
                song_id: result.song_id,
                type: 'userResults',
                uid: result.user_id,
              },
            })
          }
        >
          상세 보기 →
        </button>
      </div>

      <div className="grid grid-cols-2 gap-4 text-sm text-gray-700">
        <p>🎯 총점: {result.result.scores.overall_score.toFixed(1)}</p>
        <p>
          ⏱ 템포 일치: {result.result.scores.tempo_match_percentage.toFixed(1)}
          %
        </p>
        <p>
          🎵 음정 일치: {result.result.scores.pitch_match_percentage.toFixed(1)}
          %
        </p>
        <p>
          🥁 리듬 일치:{' '}
          {result.result.scores.rhythm_match_percentage.toFixed(1)}%
        </p>
      </div>

      {result.result.feedback && (
        <div className="mt-4 bg-gray-50 p-3 rounded-md border-l-4 border-[#a57865]">
          <h4 className="font-bold text-gray-800 mb-1">AI 피드백</h4>
          <p className="text-gray-600">{result.result.feedback}</p>
        </div>
      )}
    </div>
  );
}
