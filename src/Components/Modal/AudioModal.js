import React from 'react';

export default function AudioModal({ isOpen, onClose, children }) {
  if (!isOpen) return null;

  return (
    <div
      className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 "
      onClick={onClose}
    >
      <div
        className="bg-white p-6 rounded-xl w-[60%] h-auto max-h-[90%]  overflow-auto"
        onClick={(e) => e.stopPropagation()} // 모달 안 클릭은 닫히지 않게
      >
        <button className="text-gray-400 float-right" onClick={onClose}>
          ✖
        </button>
        <div className="mt-4">{children}</div>
      </div>
    </div>
  );
}
