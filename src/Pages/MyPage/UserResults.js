import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { useAuth } from '../../Context/AuthContext.js';
import { useNavigate } from 'react-router-dom';
import { useUserResultsQuery } from '../../Hooks/Audio/get/getUserResult.js';
import ResultCard from '../../Components/Card/ResultCard.js';

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
      {results && results.length === 0 ? (
        <p className="text-gray-500">분석 결과가 없습니다.</p>
      ) : (
        <div className="flex flex-col items-center space-y-6">
          {results &&
            results.map((result, idx) => (
              <ResultCard idx={idx} result={result} />
            ))}
        </div>
      )}
    </div>
  );
}
