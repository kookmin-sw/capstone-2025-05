import React from 'react';
import {
  ScatterChart,
  Scatter,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Legend,
} from 'recharts';

// 고정된 테크닉 카테고리 (필요시 동적 추출 가능)
const TECHNIQUES = ['normal', 'bend', 'vibrato', 'hammer'];

export default function TechniqueChart({ data }) {
  const matchedPoints = [];
  const originalPoints = [];
  const playedPoints = [];

  // x축 눈금: 0.5초 간격으로 생성
  const allSeconds = data.map((d) => d.second);
  const maxSecond = Math.max(...allSeconds, 0);
  const xTicks = [];
  for (let i = 0; i <= maxSecond + 0.5; i += 0.5) {
    xTicks.push(parseFloat(i.toFixed(1)));
  }

  // 데이터 분류 처리
  data.forEach((point) => {
    const second = point.second;

    const originalList = Array.isArray(point.original)
      ? point.original
      : [point.original || 'unknown'];

    const playedList = Array.isArray(point.played)
      ? point.played
      : [point.played || 'unknown'];

    const isMatched =
      originalList.length === playedList.length &&
      originalList.every((tech, i) => tech === playedList[i]);

    if (isMatched) {
      // 일치 시 matched 표시
      matchedPoints.push(
        ...originalList.map((technique) => ({ second, technique })),
      );
    } else {
      // 불일치 시 각각 표시
      originalPoints.push(
        ...originalList.map((technique) => ({ second, technique })),
      );
      playedPoints.push(
        ...playedList.map((technique) => ({ second, technique })),
      );
    }
  });

  return (
    <div className="overflow-x-auto h-full">
      <div className="mt-12 min-w-[1200px] h-[280px]">
        <ResponsiveContainer width="100%" height="100%">
          <ScatterChart margin={{ top: 20, right: 30, left: 20, bottom: 20 }}>
            <CartesianGrid />
            <XAxis
              type="number"
              dataKey="second"
              ticks={xTicks}
              name="Time (s)"
              tickFormatter={(v) => v.toFixed(1)}
              domain={['auto', 'auto']}
              label={{
                value: 'Time (s)',
                position: 'insideBottomRight',
                offset: -10,
              }}
            />
            <YAxis
              type="category"
              dataKey="technique"
              domain={TECHNIQUES}
              allowDuplicatedCategory={false}
              tick={{ fontSize: 12 }}
            />
            <Tooltip cursor={{ strokeDasharray: '3 3' }} />
            <Legend />

            <Scatter
              name="Matched"
              data={matchedPoints}
              fill="green"
              shape="circle"
            />
            <Scatter
              name="Original"
              data={originalPoints}
              fill="orange"
              shape="circle"
            />
            <Scatter
              name="Played"
              data={playedPoints}
              fill="blue"
              shape="circle"
            />
          </ScatterChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
