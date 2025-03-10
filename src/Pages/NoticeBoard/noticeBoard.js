import React, { useEffect, useState } from 'react';
import fakeData from '../../Data/fake_notice_data.json';
import PageButton from '../../Components/PageButton/PageButton';

export default function NoticeBoard() {
  const contents = 10; //게시판에 표시될 데이터 갯수
  const pages = 10; //게시팜에 표시될 페이지 갯수
  const totalPage = Math.ceil(fakeData.length / contents);
  const [currentPage, setCurrentPage] = useState(1);
  const [startPage, setStartPage] = useState(1);
  const [currentData, setCurrentData] = useState(fakeData.slice(0, 10));
  const [pageNumbers, setPageNumbers] = useState(
    [...Array(10).keys()].map((i) => i + 1),
  );

  const calcStartPage = () => {
    if (currentPage % pages == 0) {
      setStartPage((Math.floor(currentPage / 10) - 1) * 10 + 1);
    }
    setStartPage(Math.floor(currentPage / 10) * 10 + 1);
  };

  //화면에 보여줄 페이지들을 계산하는 함수
  const calcPageNumbers = () => {
    let pageNumbers = []; //페이지들 초기화
    for (let i = startPage; i < startPage + pages; i++) {
      pageNumbers.push(i);
    }
    setPageNumbers(pageNumbers);
  };

  const calcCurrentData = () => {
    const startIndex = (currentPage - 1) * 10;
    setCurrentData(fakeData.slice(startIndex, startIndex + 10));
  };

  console.log(currentPage);
  console.log(currentData);
  useEffect(() => {
    calcStartPage();
    calcPageNumbers();
    calcCurrentData();
  }, [currentPage]);
  return (
    <div className="flex flex-col items-center  h-[540px]">
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
          {currentData.map((item, index) => (
            <tr key={item?.id}>
              <td>{item?.id}</td>
              <td>{item?.title}</td>
              <td>{item?.view}</td>
              <td>{item?.write_time}</td>
              <td>{item?.writer}</td>
            </tr>
          ))}
        </tbody>
      </table>
      <div id="pagination" className="flex">
        {pageNumbers.map((page) => (
          <PageButton
            content={page}
            width={'50px'}
            height={'50px'}
            onClick={() => setCurrentPage(page)}
          />
        ))}
      </div>
    </div>
  );
}
