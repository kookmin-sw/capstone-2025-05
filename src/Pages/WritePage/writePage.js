import React from 'react';
import Button from '../../Components/Button/Button';
import MapleHeader from '../../Components/MapleHeader';
import MapleFooter from '../../Components/MapleFooter';

const writePage = () => {
  return (
    <>
      <MapleHeader />
      <form className=" flex flex-col items-center h-[80%] w-full">
        <div className="flex">
          <h1 className="text-center text-xl font-bold py-6">게시판 글쓰기</h1>
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">제목</h3>
          <input className="w-[55svw] h-[5svh] rounded-[10px] border-[2px] border-[#A57865]" />
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">내용</h3>
          <textarea className="w-[55svw] h-[50svh]  rounded-[10px] border-[2px] border-[#A57865]"></textarea>
        </div>
        <div className="flex flex-col mb-4">
          <h3 className="text-left font-bold mb-2 pl-2">작성자</h3>
          <input className="w-[55svw] h-[5svh] rounded-[10px] border-[2px] border-[#A57865]" />
        </div>
        <div className="flex justify-start w-[55svw]">
          <div className="hover:brightness-150 duration-[0.5s] ease-in-out">
            <Button width="90px" height="40px" children="등록하기" />
          </div>
        </div>
      </form>
      <MapleFooter />
    </>
  );
};

export default writePage;
