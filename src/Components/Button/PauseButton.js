import React from 'react';

export default function PauseButton({ onClick }) {
  return (
    <div>
      <button
        className="w-10 h-10 rounded-full bg-[#A57865] flex items-center justify-center shadow-none hover:bg-white hover:shadow-xl transition-colors group"
        onClick={onClick}
      >
        <div className="flex space-x-[4px]">
          <div className="w-[4px] h-[16px] bg-white group-hover:bg-[#A57865] transition-colors rounded-sm"></div>
          <div className="w-[4px] h-[16px] bg-white group-hover:bg-[#A57865] transition-colors rounded-sm"></div>
        </div>
      </button>
    </div>
  );
}
