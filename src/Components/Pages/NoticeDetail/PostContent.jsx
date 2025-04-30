import React from 'react';

export default function PostContent({ isEditing, content, setContent }) {
  return isEditing ? (
    <textarea
      value={content}
      onChange={(e) => setContent(e.target.value)}
      className="border p-2 w-full h-40 mt-4"
    />
  ) : (
    <p className="mt-6 ml-2 whitespace-pre-line">{content}</p>
  );
}
