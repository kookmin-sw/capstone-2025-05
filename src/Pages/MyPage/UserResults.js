import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { useAuth } from '../../Context/AuthContext.js';
import { useNavigate } from 'react-router-dom';
import { useUserResultsQuery } from '../../Hooks/Audio/get/getUserResult.js';

export default function UserResults() {
  const { uid } = useAuth();
  const { data: results, isLoading } = useUserResultsQuery(uid);
  const navigate = useNavigate();

  if (isLoading) {
  } else {
    console.log(results, '사용자 분석 결과');
  }

  return (
    <div className="min-h-screen bg-[#f1ede5] p-10 flex flex-col items-center">
      <h2 className="text-2xl font-bold mb-6">
        <span className="mr-2">🎼</span>내 연주 분석 결과
      </h2>

      {results && results.length === 0 ? (
        <p className="text-gray-500">분석 결과가 없습니다.</p>
      ) : (
        <div className="flex flex-col items-center space-y-6">
          {results &&
            results.map((result, idx) => (
              <div
                key={idx}
                className="bg-white rounded-lg shadow-md p-6 hover:shadow-xl transition-shadow duration-200 lg:w-[60%]"
              >
                <div className="flex justify-between items-center mb-2">
                  <h3 className="text-lg font-semibold text-gray-800">
                    📊 비교 분석 -{' '}
                    {new Date(result.result.created_at).toLocaleString()}
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
                  <p>
                    🎯 총점: {result.result.scores.overall_score.toFixed(1)}
                  </p>
                  <p>
                    ⏱ 템포 일치:{' '}
                    {result.result.scores.tempo_match_percentage.toFixed(1)}%
                  </p>
                  <p>
                    🎵 음정 일치:{' '}
                    {result.result.scores.pitch_match_percentage.toFixed(1)}%
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
            ))}
        </div>
      )}
    </div>
  );
}
