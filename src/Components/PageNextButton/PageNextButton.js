import React from 'react';

function PageNextButton({ width, height, onClick }) {
  return (
    <div
      className="flex items-center justify-center bg-white text-[#A57865]"
      style={{ width, height }}
      onClick={onClick}
    >
      다음
    </div>
  );
}

export default PageNextButton;
