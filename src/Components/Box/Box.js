import React from 'react';

export default function Box({
  width,
  height,
  backgroundColor = 'white',
  children,
}) {
  return (
    <div className="rounded-[10px]" style={{ width, height, backgroundColor }}>
      {children}
    </div>
  );
}
