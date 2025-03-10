import React from 'react';

function PageButton({ content, width, height, onClick, active }) {
  return (
    <div
      className={`flex items-center justify-center cursor-pointer 
      ${active ? 'bg-[#A57865] text-white' : 'bg-white text-[#A57865]'}
      hover:bg-[#A57865] hover:text-white
      `}
      style={{ width, height }}
      onClick={onClick}
    >
      {content}
    </div>
  );
}

export default PageButton;
