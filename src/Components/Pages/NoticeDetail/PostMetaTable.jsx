import React from 'react';

export default function PostMetaTable({ post }) {
  return (
    <table className="w-full border-collapse min-h-[15vh]">
      <thead className="border-b border-[#C4A08E] h-full">
        <tr className="border-b border-[#C4A08E] h-[10%]">
          <th className="bg-[#F0EDE6] border-r  border-[#C4A08E]">
            첨부 이미지
          </th>
          <td className=" border-[#C4A08E]">
            {post.image_url ? (
              <a
                href={post.image_url}
                download
                className="text-[#a57865] hover:underline ml-2"
              >
                이미지 다운로드
              </a>
            ) : (
              <span className="text-gray-500 ml-2">첨부 이미지 없음</span>
            )}
          </td>
          <th className="bg-[#F0EDE6] border-x border-[#C4A08E] ml-2">
            첨부 오디오
          </th>
          <td>
            {post.audio_url ? (
              <a
                href={post.audio_url}
                download
                className="text-[#a57865] hover:underline ml-2"
              >
                오디오 다운로드
              </a>
            ) : (
              <span className="text-gray-500 ml-2">첨부 오디오 없음</span>
            )}
          </td>
        </tr>
        <tr className="h-[10%]">
          <th className="bg-[#F0EDE6] border-r border-[#C4A08E] w-auto !important ">
            글쓴이
          </th>
          <td>
            <span className="ml-3">{post.writer}</span>
          </td>
          <th className="bg-[#F0EDE6] border-x border-[#C4A08E] w-auto !important">
            작성 날짜
          </th>
          <td>
            <span className="ml-3">{post.write_time}</span>
          </td>
          <th className="bg-[#F0EDE6] border-x border-[#C4A08E] w-[15%] !important">
            조회수
          </th>
          <td>
            <span className="ml-3">{post.view}</span>
          </td>
        </tr>
      </thead>
    </table>
  );
}
