import React from 'react';

export default function PlayButton({ onClick }) {
  return (
    <div>
      <button
        className="w-10 h-10 rounded-full bg-[#A57865] flex items-center justify-center shadow-none hover:bg-white hover:shadow-xl transition-colors group"
        onClick={onClick}
      >
        <div className="w-0 h-0 border-l-[12px] border-l-white border-t-[8px] border-b-[8px] border-t-transparent border-b-transparent group-hover:border-l-[#A57865] transition-colors"></div>
      </button>
    </div>
  );
}
