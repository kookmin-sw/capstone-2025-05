import React from 'react';

function PageNextButton({ width, height, onClick }) {
  return (
    <div
      className="flex items-center justify-center bg-white text-[#A57865] 
      duration-300 ease-in-out hover:bg-[#A57865] hover:text-white
      rounded-r-lg
      "
      style={{ width, height }}
      onClick={onClick}
    >
      다음
    </div>
  );
}

export default PageNextButton;
