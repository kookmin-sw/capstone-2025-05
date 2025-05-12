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
  Customized,
} from 'recharts';
import CustomizedMismatchRects from './CustomizedMismatchRects';

const normalizeTechnique = (tech) => {
  if (typeof tech === 'string' && tech.includes(',')) {
    return tech.split(',')[0].trim();
  }
  return tech;
};

export default function TechniqueChart({ data }) {
  const matchedPoints = [];
  const originalPoints = [];
  const playedPoints = [];

  // x축 눈금 계산
  const allSeconds = data.map((d) => d.second);
  const maxSecond = Math.max(...allSeconds, 0);
  const xTicks = [];
  for (let i = 0; i <= maxSecond + 0.5; i += 0.5) {
    xTicks.push(parseFloat(i.toFixed(1)));
  }

  // 데이터 분류
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
      matchedPoints.push(
        ...originalList.map((technique) => ({
          second,
          technique: normalizeTechnique(technique),
        })),
      );
    } else {
      originalPoints.push(
        ...originalList.map((technique) => ({
          second,
          technique: normalizeTechnique(technique),
        })),
      );
      playedPoints.push(
        ...playedList.map((technique) => ({
          second,
          technique: normalizeTechnique(technique),
        })),
      );
    }
  });

  // Y축 domain 자동 생성
  const TECHNIQUES = Array.from(
    new Set(
      [...matchedPoints, ...originalPoints, ...playedPoints].map(
        (d) => d.technique,
      ),
    ),
  ).sort();

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
            <Customized
              component={(props) => (
                <CustomizedMismatchRects
                  {...props}
                  playedPoints={playedPoints}
                  originalPoints={originalPoints}
                />
              )}
            />
            <Scatter
              name="Matched"
              data={matchedPoints}
              fill="limegreen"
              stroke="black"
              strokeWidth={1.2}
              r={6}
              shape={(props) => (
                <circle
                  {...props}
                  r={6}
                  style={{
                    filter: 'drop-shadow(0 0 4px limegreen)',
                  }}
                />
              )}
            />
            <Scatter
              name="Original"
              data={originalPoints}
              fill="orange"
              r={5}
              shape="square"
            />
            <Scatter
              name="Played"
              data={playedPoints}
              fill="blue"
              r={5}
              shape="triangle"
            />
            <Scatter
              name="Mismatch"
              data={[]}
              fill="rgba(255, 0, 0, 0.3)"
              shape={(props) => (
                <text
                  {...props}
                  dy={4}
                  fontSize={18}
                  textAnchor="middle"
                  fill="red"
                >
                  ❌
                </text>
              )}
            />
          </ScatterChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
