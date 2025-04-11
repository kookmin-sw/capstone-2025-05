import React from 'react';
import {
  LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer
} from 'recharts';

const data = [
  { name: '1월', score: 40 },
  { name: '2월', score: 50 },
  { name: '3월', score: 65 },
  { name: '4월', score: 70 },
  { name: '5월', score: 80 },
  { name: '6월', score: 90 },
];

export default function ImprovementChart() {
  return (
    <ResponsiveContainer width={700} height={300}>
      <LineChart data={data} margin={{ top: 40, right: 30, left: 0, bottom: 5 }}>

        <CartesianGrid strokeDasharray="3 3" stroke="#e2e2e2" />

        <XAxis dataKey="name" tick={{ fontSize: 12 }} />
        <YAxis domain={[0, 100]} tick={{ fontSize: 12 }} />

        <Tooltip />

        <defs>
          <linearGradient id="colorScore" x1="0" y1="0" x2="0" y2="1">
            <stop offset="0%" stopColor="#8884d8" stopOpacity={0.4} />
            <stop offset="100%" stopColor="#8884d8" stopOpacity={0} />
          </linearGradient>
        </defs>

        <Line
          type="monotone" 
          dataKey="score"
          stroke= "#a57865"
          strokeWidth={3}
          dot={{ r: 4 }}
          activeDot={{ r: 6 }}
          fill="url(#colorScore)"
        />
      </LineChart>
    </ResponsiveContainer>
  );
}
