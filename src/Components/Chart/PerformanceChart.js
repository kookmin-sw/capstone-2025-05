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
  ReferenceLine,
  ReferenceArea,
} from 'recharts';

export default function PerformanceChart({ data, measureTimes = [], largeDiffSections = [] }) {
  const allValues = data.flatMap(d => [d.original, d.played]);
  const minY = Math.max(0, Math.min(...allValues) - 50);
  const maxY = Math.max(...allValues) + 50;

  const ticks = [];
  for (let y = minY; y <= maxY; y += 20) ticks.push(y);


  const FillBetweenLines = ({ height, width, xAxisMap, yAxisMap }) => {
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
          fill="rgba(255, 99, 132, 0.3)"
        />
      );
    }

    return <g>{areas}</g>;
  };

  return (
    <div className="overflow-x-auto h-full">
      <div className="mt-5 min-w-[100%] h-[100%]">
        <ResponsiveContainer width="100%" height="100%">
          <ComposedChart data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="second" tickFormatter={(value) => value.toFixed(1)} />
            <YAxis domain={[minY, maxY]} ticks={ticks} tickFormatter={(value) => `${value}`} />
            <Tooltip />
            <Legend />

            {/* 🔹 마디 경계선 표시 */}
            {measureTimes.map((time, index) => (
              <ReferenceLine
                key={`measure-${index}`}
                x={time}
                stroke="#aaa"
                strokeDasharray="4 4"
                label={{
                  value: `|`,
                  position: "top",
                  fontSize: 12,
                  fill: "#aaa"
                }}
              />
            ))}

            {largeDiffSections.map((section, index) => (
              <ReferenceArea
                key={`diff-${index}`}
                x1={section.startTime}
                x2={section.endTime}
                strokeOpacity={0}
                fill="rgba(255, 165, 0, 0.3)"  // 연한 주황
              />
            ))}


            <Customized component={(props) => <FillBetweenLines {...props} />} />


            <Line type="linear" dataKey="original" stroke="#8884d8" dot={false} name="Original Pitch" />
            <Line type="linear" dataKey="played" stroke="#82ca9d" dot={false} name="User Pitch" />
          </ComposedChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
