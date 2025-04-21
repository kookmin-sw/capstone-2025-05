import React from 'react';

export default function Modal({ isOpen, onClose, children }) {
  if (!isOpen) return null;

  return (
    <div
      className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50"
      onClick={onClose}
    >
      <div
        className="bg-white rounded-2xl p-6 w-96 shadow-xl"
        onClick={(e) => e.stopPropagation()} // 모달 바깥 클릭 시 닫힘 방지
      >
        <button
          className="text-gray-500 hover:text-gray-800 float-right"
          onClick={onClose}
        >
          &times;
        </button>
        <div className="mt-[2px]">{children}</div>
      </div>
    </div>
  );
}
