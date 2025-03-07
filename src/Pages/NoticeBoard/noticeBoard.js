import React from 'react';
import fakeData from '../../Data/fake_notice_data.json';

function noticeBoard() {
  // noticeBoard
  console.log(fakeData);
  return (
    <div className="flex justify-center  h-[540px]">
      <table id="table" className="w-[80%] h-[80%] m-auto">
        <thead>
          <div className="mb-[10%]">
            <th scope="col" className="text-2xl font-bold h-[30px] w-[15%]">
              자유 게시판
            </th>
          </div>
          <tr id="header" className="border-y-[2px] border-[#A57865] h-[10%]">
            <th
              scope="col"
              id="number"
              className="font-bold w-[10%] text-xl text-center"
            >
              No.
            </th>
            <th
              scope="col"
              id="title"
              className="font-bold w-[50%] text-xl text-center"
            >
              제목
            </th>
            <th scope="col" className="w-[10%] font-bold text-center">
              글쓴이
            </th>
            <th scope="col" className="w-[10%] font-bold text-center">
              작성시간
            </th>
            <th scope="col" className="w-[10%] font-bold text-center">
              조회수
            </th>
          </tr>
        </thead>
        <tbody>
          {fakeData.map((item, index) => {
            <th>{item[0].id}</th>;
          })}
        </tbody>
      </table>
    </div>
  );
}

export default noticeBoard;
