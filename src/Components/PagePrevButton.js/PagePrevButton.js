import React from 'react';

function PagePrevButton({ width, height, onClick }) {
  return (
    <div
      className="flex items-center justify-center bg-white text-[#A57865] 
      duration-300 ease-in-out hover:bg-[#A57865] hover:text-white
      rounded-l-lg
      "
      style={{ width, height }}
      onClick={onClick}
    >
      이전
    </div>
  );
}

export default PagePrevButton;
