import React from 'react';
import {
  ComposedChart,
  Line,
  XAxis,
  YAxis,
  Tooltip,
  ResponsiveContainer,
  CartesianGrid,
  Legend,
  Customized,
} from 'recharts';

export default function PerformanceChart({ data }) {
  // 평균값 중심으로 Y축 범위 조정
  const allValues = data.flatMap(d => [d.original, d.played]);
  const minY = Math.max(0, Math.min(...allValues) - 50);  // 최소값보다 50 낮게
  const maxY = Math.max(...allValues) + 50;  // 최대값보다 50 높게

  // Y축 눈금 설정 (20 단위로 조정하여 더 세밀하게)
  const ticks = [];
  for (let y = minY; y <= maxY; y += 20) {
    ticks.push(y);
  }

  // 두 선 사이 영역 색칠 컴포넌트
  const FillBetweenLines = ({ height, width, xAxisMap, yAxisMap, data }) => {
    const yScale = yAxisMap[0].scale;
    const xScale = xAxisMap[0].scale;

    const areas = [];

    for (let i = 0; i < data.length - 1; i++) {
      const x1 = xScale(data[i].second);
      const x2 = xScale(data[i + 1].second);

      const o1 = data[i].original;
      const o2 = data[i + 1].original;
      const p1 = data[i].played;
      const p2 = data[i + 1].played;

      const top1 = Math.max(o1, p1);
      const top2 = Math.max(o2, p2);
      const bottom1 = Math.min(o1, p1);
      const bottom2 = Math.min(o2, p2);

      areas.push(
        <polygon
          key={i}
          points={`
            ${x1},${yScale(top1)}
            ${x2},${yScale(top2)}
            ${x2},${yScale(bottom2)}
            ${x1},${yScale(bottom1)}
          `}
          fill="rgba(255, 99, 132, 0.3)" // 연한 빨간색으로 차이 강조
        />
      );
    }

    return <g>{areas}</g>;
  };

  return (
    <div className="overflow-x-auto">
      <div className="mt-5 min-w-[1200px] h-[280px]">
        <ResponsiveContainer width="100%" height="100%">
          <ComposedChart data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="second" tickFormatter={(value) => value.toFixed(1)} />
            <YAxis
              domain={[minY, maxY]}
              ticks={ticks}
              tickFormatter={(value) => `${value}`}
            />
            <Tooltip />
            <Legend />

            {/* 🔥 두 선 사이 간격 강조 */}
            <Customized component={(props) => <FillBetweenLines {...props} data={data} />} />

            {/* 선 그래프 */}
            <Line type="linear" dataKey="original" stroke="#8884d8" dot={false} name="Original Pitch" />
            <Line type="linear" dataKey="played" stroke="#82ca9d" dot={false} name="User Pitch" />
          </ComposedChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
