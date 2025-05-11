import React from 'react';
import NoDataImg from '../../Assets/no_data.svg';
export default function NoRecentPage() {
  return (
    <div className="col-span-full flex flex-col items-center justify-center py-12 text-center text-gray-500">
      <img
        src={NoDataImg}
        alt="No result"
        className="w-40 h-40 mb-6 opacity-70"
      />
      <h2 className="text-lg font-semibold">연주한 곡이 없습니다</h2>
      <p className="mt-2 text-sm text-gray-400">
        아직 연주한 노래가 없어요. 지금 하나 업로드해보세요!
      </p>
    </div>
  );
}
