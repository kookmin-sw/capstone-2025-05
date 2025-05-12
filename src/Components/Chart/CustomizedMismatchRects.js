import React, { useState } from 'react';

export default function CustomizedMismatchRects({
  playedPoints = [],
  originalPoints = [],
  xAxisMap,
  yAxisMap,
}) {
  const [hoveredX, setHoveredX] = useState(null);
  const [hoveredIndex, setHoveredIndex] = useState(null);
  if (!playedPoints.length || !originalPoints.length) return null;

  const xScale = xAxisMap[Object.keys(xAxisMap)[0]].scale;
  const yScale = yAxisMap[Object.keys(yAxisMap)[0]].scale;

  // 모든 기술의 픽셀 범위 계산 (높이 자동화)
  const yDomain = yScale.domain(); // 예: ['bend', 'vibrato', 'normal']
  const yMin = yScale(yDomain[0]);
  const yMax = yScale(yDomain[yDomain.length - 1]);
  const yTop = Math.min(yMin, yMax);
  const yBottom = Math.max(yMin, yMax);
  console.log(yDomain);
  const rectHeight = yMax + yMin - 10; // 패딩 약간 추가

  const rectY = yTop - 5;

  // 불일치 시간들만 추출
  const mismatchSeconds = playedPoints
    .map((p, i) => {
      const o = originalPoints[i];
      if (!o || p.technique === o.technique) return null;
      return p.second;
    })
    .filter((sec) => sec !== null);

  return (
    <g>
      {mismatchSeconds.map((sec, i) => {
        const x = xScale(sec) - 5;
        return (
          <>
            <rect
              key={`mismatch-${i}`}
              x={x}
              y={yTop}
              width={10}
              height={rectHeight}
              fill="rgba(255, 0, 0, 0.15)"
              onMouseEnter={() => {
                setHoveredX(x + 5);
                setHoveredIndex(i);
              }}
              onMouseLeave={() => {
                setHoveredX(null);
                setHoveredIndex(null);
              }}
            />
            <text
              x={x + 5}
              y={rectY - 3}
              textAnchor="middle"
              fontSize={10}
              fill="red"
              fontWeight="bold"
            >
              ❌
            </text>
          </>
        );
      })}
    </g>
  );
}
