import React from 'react';

export default function Box({
  width,
  height,
  backgroundColor = 'white',
  children,
  overwrite, //rounded 변경을 위한 오버라이딩
}) {
  return (
    <div
      className={`rounded-[10px] ${overwrite}`}
      style={{ width, height, backgroundColor }}
    >
      {children}
    </div>
  );
}
