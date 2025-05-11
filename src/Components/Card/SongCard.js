import React from 'react';

export default function SongCard({ cover_url, title, artist, onClick }) {
  return (
    <div
      className="flex items-center gap-4 bg-white p-4 rounded-lg shadow mb-6 w-full lg:w-[60%]"
      onClick={onClick}
    >
      <img
        src={cover_url}
        alt="Cover"
        className="w-20 h-20 object-cover rounded"
      />
      <div>
        <p className="text-sm text-gray-500">분석 대상 곡</p>
        <h2 className="text-xl font-bold text-[#463936]">{title}</h2>
        <p className="text-sm text-gray-600">{artist || 'Unknown Artist'}</p>
      </div>
    </div>
  );
}
