export default function ProgressBar({ graph, percentage = 0 }) {
  return (
    <div className="w-full">
      <div className="flex justify-between items-center mb-1">
        <span className={`font-semibold text-${graph.color}-500`}>
          {graph.label}
        </span>
        <span className="text-sm font-bold">{percentage.toFixed(1)}%</span>
      </div>
      <div className="bg-gray-200 rounded-full h-3 overflow-hidden">
        <div
          className={`bg-${graph.color}-500 h-full transition-all duration-500`}
          style={{ width: `${percentage.toFixed(1)}%` }}
        />
      </div>
    </div>
  );
}
