import React from 'react';

export default function AudioChart({
  waveWidth,
  waveRef,
  refCurrentTime,
  filter,
}) {
  const refTitle = '🎵 원본 연주';
  const userTitle = '🎸 사용자 연주';
  console.log(waveRef);
  return (
    <div className="border rounded-2xl shadow-lg p-6 w-full bg-white mt-2">
      <h3 className="text-xl font-bold mb-4">
        {filter === '원본' ? userTitle : refTitle}
      </h3>
      <div className="w-full overflow-x-auto">
        <div className={`w-[${waveWidth}px]`}>
          <div ref={waveRef}></div>
        </div>
      </div>
      <div className="mt-4 text-sm text-gray-700">
        현재 재생 위치: {refCurrentTime}초
      </div>
    </div>
  );
}
