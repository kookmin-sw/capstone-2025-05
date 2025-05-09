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

export default function BeatChart({
  data,
  measureTimes = [],
  largeDiffSections = [],
}) {
  const allValues = data.flatMap((d) => [d.original, d.played]);
  const minY = Math.max(0, Math.min(...allValues) - 0.5);
  const maxY = Math.max(...allValues) + 0.5;

  const ticks = [];
  for (let y = minY; y <= maxY; y += 0.5) {
    ticks.push(parseFloat(y.toFixed(1)));
  }

  const FillBetweenLines = ({ xAxisMap, yAxisMap }) => {
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
          points={` ${x1},${yScale(top1)} ${x2},${yScale(top2)} ${x2},${yScale(bottom2)} ${x1},${yScale(bottom1)} `}
          fill="rgba(255, 99, 132, 0.3)"
        />,
      );
    }

    return <g>{areas}</g>;
  };

  return (
    <div className="overflow-x-auto h-full">
      <div className="mt-12 min-w-[1200px] h-[280px]">
        <ResponsiveContainer width="100%" height="100%">
          <ComposedChart data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis
              dataKey="second"
              tickFormatter={(value) => value.toFixed(1)}
              label={{
                value: 'Time (s)',
                position: 'insideBottomRight',
                offset: -10,
              }}
            />
            <YAxis
              domain={[minY, maxY]}
              ticks={ticks}
              tickFormatter={(value) => `${value}s`}
              label={{
                value: 'Beat Timing (s)',
                angle: -90,
                position: 'insideLeft',
              }}
            />
            <Tooltip />
            <Legend />

            {measureTimes.map((time, index) => (
              <ReferenceLine
                key={`measure-${index}`}
                x={time}
                stroke="#aaa"
                strokeDasharray="4 4"
                label={{
                  value: `|`,
                  position: 'top',
                  fontSize: 12,
                  fill: '#aaa',
                }}
              />
            ))}

            {largeDiffSections.map((section, index) => (
              <ReferenceArea
                key={`diff-${index}`}
                x1={section.startTime}
                x2={section.endTime}
                strokeOpacity={0}
                fill="rgba(255, 165, 0, 0.3)"
              />
            ))}

            <Customized
              component={(props) => <FillBetweenLines {...props} />}
            />

            <Line
              type="linear"
              dataKey="original"
              stroke="green"
              dot={false}
              name="Original Onset"
            />
            <Line
              type="linear"
              dataKey="played"
              stroke="blue"
              dot={false}
              name="User Onset"
            />
          </ComposedChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
