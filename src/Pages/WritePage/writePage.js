import React, { useState } from 'react';
import MapleHeader from '../../Components/MapleHeader';
import MapleFooter from '../../Components/MapleFooter';
import { usePostWriteMutation } from '../../Hooks/post/usePostWriteMutation';

export default function WritePage() {
  const [title, setTitle] = useState('');
  const [content, setContent] = useState('');
  const [author, setAuthor] = useState('');
  const { mutate: writePost } = usePostWriteMutation();

  // 오늘 날짜 format
  const formatDate = (date) => {
    return date.toISOString().split('T')[0]; // YYYY-MM-DD 형식
  };

  const handleSubmit = (e) => {
    e.preventDefault();
    const post = {
      uid: 'dogyeong', //중복X,자동 생성 & 증가되도록 변경해야됨
      title,
      author,
      write_time: formatDate(new Date()),
      view: 0,
      content,
    };
    if (!title) {
      alert('제목을 입력해주세요');
      return;
    }
    if (!content) {
      alert('내용을 입력해주세요');
      return;
    }
    if (!author) {
      alert('작성자를 입력해주세요');
      return;
    }
    if (content.length < 10) {
      alert('내용을 10자 이상 입력해주세요');
      return;
    }

    writePost(
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
      {/* <form
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
            placeholder=" 제목을 입력해주세요"
            onChange={(e) => setTitle(e.target.value)}
          />
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">내용</h3>
          <textarea
            className="w-[55svw] h-[50svh]  rounded-[10px] border-[2px] border-[#A57865]"
            value={content}
            placeholder=" 10자 이상 입력해주세요"
            onChange={(e) => setContent(e.target.value)}
          ></textarea>
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">작성자</h3>
          <input
            className="w-[55svw] h-[5svh] rounded-[10px] border-[2px] border-[#A57865]"
            value={author}
            placeholder=" 작성자를 입력해주세요"
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
      </form> */}
      <div className="min-h-screen bg-[F0EFE6] flex justify-center items-center px-4 py-12">
        <form
          onSubmit={handleSubmit}
          className="bg-white w-full max-w-2xl rounded-2xl shadow-lg p-10 space-y-6 transition-all duration-300"
        >
          <h1 className="text-3xl font-bold text-center text-[#5f4532] tracking-wide">
            게시판 글쓰기
          </h1>

          {/* 제목 */}
          <div>
            <label className="block mb-2 text-sm font-medium text-[#5f4532]">
              제목
            </label>
            <input
              className="w-full px-4 py-3 border border-[#A57865] rounded-xl shadow-sm focus:ring-2 focus:ring-[#a57865] focus:outline-none bg-[#fdfaf6] placeholder:text-[#b28c74]"
              value={title}
              onChange={(e) => setTitle(e.target.value)}
              placeholder="제목을 입력해주세요"
            />
          </div>

          {/* 내용 */}
          <div>
            <label className="block mb-2 text-sm font-medium text-[#5f4532]">
              내용
            </label>
            <textarea
              className="w-full h-48 px-4 py-3 border border-[#A57865] rounded-xl shadow-sm resize-none focus:ring-2 focus:ring-[#a57865] focus:outline-none bg-[#fdfaf6] placeholder:text-[#b28c74]"
              value={content}
              onChange={(e) => setContent(e.target.value)}
              placeholder="10자 이상 입력해주세요"
            />
          </div>

          {/* 작성자 */}
          <div>
            <label className="block mb-2 text-sm font-medium text-[#5f4532]">
              작성자
            </label>
            <input
              className="w-full px-4 py-3 border border-[#A57865] rounded-xl shadow-sm focus:ring-2 focus:ring-[#a57865] focus:outline-none bg-[#fdfaf6] placeholder:text-[#b28c74]"
              value={author}
              onChange={(e) => setAuthor(e.target.value)}
              placeholder="작성자를 입력해주세요"
            />
          </div>

          {/* 버튼 */}
          <button
            type="submit"
            className="w-full py-3 bg-gradient-to-r from-[#A57865] to-[#d5b6a2] text-white font-semibold rounded-full shadow-md hover:scale-105 transition-transform duration-300"
          >
            등록하기
          </button>
        </form>
      </div>

      <MapleFooter />
    </>
  );
}
