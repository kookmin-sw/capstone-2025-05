import React from 'react';

export default function Box({ width, height, children }) {
  return (
    <div
      className="bg-[#A57865] text-white rounded-[10px] cursor-pointer flex justify-center items-center"
      style={{ width, height }}
    >
      {children}
    </div>
  );
}
