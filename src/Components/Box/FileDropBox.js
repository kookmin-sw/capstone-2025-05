import React, { useState } from 'react';

function FileDropBox({ label, onFileSelect, accept }) {
  const [dragOver, setDragOver] = useState(false);
  const [fileName, setFileName] = useState(null);

  const handleDrop = (e) => {
    e.preventDefault();
    setDragOver(false);
    const file = e.dataTransfer.files[0];
    setFileName(file.name);
    onFileSelect(file);
  };

  const handleChange = (e) => {
    const file = e.target.files[0];
    setFileName(file.name);
    onFileSelect(file);
  };

  return (
    <div
      onDrop={handleDrop}
      onDragOver={(e) => {
        e.preventDefault();
        setDragOver(true);
      }}
      onDragLeave={() => setDragOver(false)}
      className={`border-2 border-dashed ${
        dragOver ? 'border-[#a57865] bg-[#fffaf6]' : 'border-[#ddd]'
      } rounded-xl px-4 py-6 text-center text-[#5f4532] transition-all duration-200 w-[49%]`}
    >
      <label className="cursor-pointer">
        <span className="font-medium">{label}</span>
        <input
          type="file"
          accept={accept}
          className="hidden"
          onChange={handleChange}
        />
      </label>
      <p className="text-sm mt-2 text-gray-500">
        {fileName || '클릭하거나 파일을 드래그해서 업로드하세요'}
      </p>
    </div>
  );
}

export default FileDropBox;
