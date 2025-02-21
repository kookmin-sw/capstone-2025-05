import React from 'react';

export default function Box({ width, height, content }) {
    return (
      <div
        className="bg-[#A57865] text-white rounded-[10px] cursor-pointer"
        style={{ width, height }}
      >
        {content}
      </div>
    );
}
