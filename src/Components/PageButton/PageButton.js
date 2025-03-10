import React from 'react';

function PageButton({ content, width, height, onClick }) {
  return (
    <div
      className="flex items-center justify-center bg-white text-black cursor-pointer"
      style={{ width, height }}
      onClick={onClick}
    >
      {content}
    </div>
  );
}

export default PageButton;
