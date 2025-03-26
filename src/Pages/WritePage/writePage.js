import React, { useState } from 'react';
import Button from '../../Components/Button/Button';
import MapleHeader from '../../Components/MapleHeader';
import MapleFooter from '../../Components/MapleFooter';
import { usePostWriteMutation } from '../../Hooks/post/usePostWriteMutation';

export default function WritePage() {
  const [title, setTitle] = useState('');
  const [content, setContent] = useState('');
  const [author, setAuthor] = useState('');
  const { mutate } = usePostWriteMutation();

  // 오늘 날짜 format
  const formatDate = (date) => {
    return date.toISOString().split('T')[0]; // YYYY-MM-DD 형식
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    const post = {
      id: 10000000000, //중복X,자동 생성 & 증가되도록 변경해야됨
      title,
      author,
      write_time: formatDate(new Date()),
      view: 0,
      content,
    };
    mutate(
      { post },
      {
        onSuccess: () => {
          alert('✅게시물 등록 완료');
        },
        onError: (error) => {
          console.error('게시물 등록 중 오류 발생:', error);
          alert('❎게시물 등록에 실패했습니다.');
        },
      },
    );
  };
  return (
    <>
      <MapleHeader />
      <form
        className=" flex flex-col items-center h-[80%] w-full"
        onSubmit={handleSubmit}
      >
        <div className="flex">
          <h1 className="text-center text-xl font-bold py-6">게시판 글쓰기</h1>
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">제목</h3>
          <input
            className="w-[55svw] h-[5svh] rounded-[10px] border-[2px] border-[#A57865]"
            value={title}
            onChange={(e) => setTitle(e.target.value)}
          />
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">내용</h3>
          <textarea
            className="w-[55svw] h-[50svh]  rounded-[10px] border-[2px] border-[#A57865]"
            value={content}
            onChange={(e) => setContent(e.target.value)}
          ></textarea>
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">작성자</h3>
          <input
            className="w-[55svw] h-[5svh] rounded-[10px] border-[2px] border-[#A57865]"
            value={author}
            onChange={(e) => setAuthor(e.target.value)}
          />
        </div>
        <div className="flex justify-start w-[55svw]">
          <button
            type="submit"
            className="hover:brightness-150 duration-[0.5s] ease-in-out"
          >
            <Button width="90px" height="40px">
              등록하기
            </Button>
          </button>
        </div>
      </form>
      <MapleFooter />
    </>
  );
}
