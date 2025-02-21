import React from 'react';

export default function Box({ width, height, children }) {
    return (
      <div
        className="bg-white rounded-[10px]"
        style={{ width, height }}
      >
        {children}
      </div>
    );
  }
