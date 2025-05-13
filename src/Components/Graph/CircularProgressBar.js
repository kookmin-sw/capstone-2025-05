export default function CircularProgressBar({ graph, percentage = 0 }) {
  const radius = 40;
  const stroke = 8;
  const normalizedRadius = radius - stroke / 2;
  const circumference = 2 * Math.PI * normalizedRadius;
  const strokeDashoffset = circumference - (percentage / 100) * circumference;

  const colorMap = {
    red: 'stroke-red-500 text-red-500',
    blue: 'stroke-blue-500 text-blue-500',
    green: 'stroke-green-500 text-green-500',
    yellow: 'stroke-yellow-500 text-yellow-500',
    // 필요 시 확장
  };
  const colorClass = colorMap[graph.color] || 'stroke-gray-500 text-gray-500';

  return (
    <div className="flex flex-col items-center space-y-2 w-28">
      <svg height={radius * 2} width={radius * 2}>
        <circle
          stroke="lightgray"
          fill="transparent"
          strokeWidth={stroke}
          r={normalizedRadius}
          cx={radius}
          cy={radius}
        />
        <circle
          className={`transition-all duration-700 ${colorClass}`}
          fill="transparent"
          strokeWidth={stroke}
          strokeLinecap="round"
          strokeDasharray={circumference}
          strokeDashoffset={strokeDashoffset}
          r={normalizedRadius}
          cx={radius}
          cy={radius}
        />
      </svg>
      <span className={`text-sm font-semibold ${colorClass}`}>
        {graph.label}
      </span>
      <span className="text-sm font-bold">{percentage.toFixed(1)}%</span>
    </div>
  );
}
