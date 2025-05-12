import React from 'react';

export default function CustomizedLines({
  originalPoints = [],
  playedPoints = [],
}) {
  if (!originalPoints.length || !playedPoints.length) return null;

  // 각 점쌍을 선으로 연결
  const lines = originalPoints.map((orig, idx) => {
    const played = playedPoints[idx];

    // 좌표가 없거나 index가 초과된 경우 방어
    if (!orig || !played) return null;

    return (
      <line
        key={`line-${idx}`}
        x1={orig.cx}
        y1={orig.cy}
        x2={played.cx}
        y2={played.cy}
        stroke="gray"
        strokeWidth={1}
        strokeDasharray="4 2"
      />
    );
  });

  return <g>{lines}</g>;
}
