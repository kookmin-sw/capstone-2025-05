import React from 'react';

export default function CustomizedLines({
  originalPoints,
  playedPoints,
  xAxisMap,
  yAxisMap,
}) {
  const xScale = xAxisMap?.second?.scale;
  const yScale = yAxisMap?.technique?.scale;

  if (!xScale || !yScale) return null;

  const lines = [];

  for (
    let i = 0;
    i < Math.min(originalPoints.length, playedPoints.length);
    i++
  ) {
    const o = originalPoints[i];
    const p = playedPoints[i];

    // 필수 데이터 확인
    if (
      !o ||
      !p ||
      o.second == null ||
      p.second == null ||
      !o.technique ||
      !p.technique
    )
      continue;

    lines.push(
      <line
        key={`line-${i}`}
        x1={xScale(o.second)}
        y1={yScale(o.technique)}
        x2={xScale(p.second)}
        y2={yScale(p.technique)}
        stroke="#e53935"
        strokeWidth={2}
        strokeDasharray="4 2"
        opacity={0.8}
      />,
    );
  }

  return <g>{lines}</g>;
}
