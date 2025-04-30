import React from 'react';
export default function CommentSection({ postId }) {
  return (
    <section className="w-full max-w-[900px] mt-10">
      <div className="border-t border-gray-300 pt-4">
        <h2 className="text-lg font-bold">댓글 영역 (postId: {postId})</h2>
      </div>
    </section>
  );
}
