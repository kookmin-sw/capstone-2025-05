import React from 'react';

export default function Button({
  width,
  height,
  children,
  backgroundColor = '#A57865',
  cursor = 'pointer',
  onClick,
  disabled = false,
}) {
  return (
    <div
      className={`text-white rounded-[10px] flex justify-center items-center ${
        disabled ? 'cursor-not-allowed' : ''
      }`}
      style={{
        width,
        height,
        backgroundColor: disabled ? '#AFAFAF' : backgroundColor,
        cursor: disabled ? 'not-allowed' : cursor,
      }}
      onClick={!disabled ? onClick : undefined}
    >
      {children}
    </div>
  );
}
