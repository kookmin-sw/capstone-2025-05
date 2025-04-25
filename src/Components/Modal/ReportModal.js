import React, { useState } from 'react';
import Modal from './Modal';
import { useEffect } from 'react';

export default function ReportModal({ isOpen, onClose, onConfirm }) {
  const [reason, setReason] = useState('');

  const handleSubmit = () => {
    if (!reason.trim()) {
      alert('신고 사유를 입력해주세요.');
      return;
    }
    onConfirm(reason); // 부모에게 reason 전달
    setReason('');
    onClose();
  };

  return (
    <Modal isOpen={isOpen} onClose={onClose}>
      <h2 className="text-lg font-bold mb-4">신고하기</h2>
      <p className="text-sm text-gray-600 mb-2">신고 사유를 입력해주세요.</p>
      <textarea
        className="w-full h-24 border border-gray-300 rounded p-2 mb-4 resize-none"
        placeholder="ex) 욕설, 음란물 등"
        value={reason}
        onChange={(e) => setReason(e.target.value)}
      />
      <div className="flex justify-end gap-3">
        <button
          onClick={() => {
            setReason('');
            onClose();
          }}
          className="px-4 py-2 bg-gray-200 rounded hover:bg-gray-300"
        >
          취소
        </button>
        <button
          onClick={handleSubmit}
          className="px-4 py-2 bg-red-500 text-white rounded hover:bg-red-600"
        >
          신고하기
        </button>
      </div>
    </Modal>
  );
}
