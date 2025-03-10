import React from 'react';

function PagePrevButton({ width, height, onClick }) {
  return (
    <div
      className="flex items-center justify-center bg-white text-[#A57865]"
      style={{ width, height }}
      onClick={onClick}
    >
      이전
    </div>
  );
}

export default PagePrevButton;
