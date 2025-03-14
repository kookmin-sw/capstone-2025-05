import React, { useEffect, useState } from 'react';
import fakeData from '../../Data/fake_notice_data.json';
import PageButton from '../../Components/PageButton/PageButton';
import PagePrevButton from '../../Components/PagePrevButton.js/PagePrevButton';
import PageNextButton from '../../Components/PageNextButton/PageNextButton';
import SearchBox from '../../Components/SearchBox/searchBox';
import Button from '../../Components/Button/Button';
import MapleHeader from '../../Components/MapleHeader';
import MapleFooter from '../../Components/MapleFooter';
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

  //화면에 보여줄 페이지들 중 첫번째 페이지를 계산하는 함수
  const calcStartPage = () => {
    if (currentPage % pages == 0) {
      setStartPage((Math.floor(currentPage / 10) - 1) * 10 + 1);
    } else {
      setStartPage(Math.floor(currentPage / 10) * 10 + 1);
    }
  };

  //화면에 보여줄 페이지들을 계산하는 함수
  const calcPageNumbers = () => {
    let pageNumbers = []; //페이지들 초기화
    for (let i = startPage; i < Math.min(startPage + pages, totalPage); i++) {
      pageNumbers.push(i);
    }
    setPageNumbers(pageNumbers);
  };

  const calcCurrentData = () => {
    const startIndex = (currentPage - 1) * contents;
    setCurrentData(fakeData.slice(startIndex, startIndex + contents));
  };

  const calcPagiNation = () => {
    calcCurrentData();
    calcStartPage();
    calcPageNumbers();
  };

  useEffect(() => {
    calcPagiNation();
  }, [currentPage, startPage]);

  return (
    <>
      <MapleHeader />
      <div className="flex flex-col items-center h-[100svh]">
        <table id="table" className="w-[80%] h-[80%] m-auto">
          <thead>
            <div className="mb-[10px]">
              <th
                scope="col"
                className="text-2xl font-bold h-[30px] w-auto whitespace-nowrap"
              >
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
              <tr key={item?.id} className="border-b-[1px] border-[#A57865]">
                <td className="text-center">{item?.id}</td>
                <td className="text-center">{item?.title}</td>
                <td className="text-center">{item?.writer}</td>
                <td className="text-center">{item?.write_time}</td>
                <td className="text-center">{item?.view}</td>
              </tr>
            ))}
          </tbody>
        </table>
        <div id="searchBar" className="flex justify-between w-[80%]">
          <div></div>
          <SearchBox width={'300px'} height={'40px'} />
          <div className="hover:brightness-150 duration-[0.5s] ease-in-out">
            <Button width={'80px'} height={'40px'}>
              글쓰기
            </Button>
          </div>
        </div>
        <div id="pagination" className="flex my-4">
          {startPage != 1 && (
            <PagePrevButton
              width={'50px'}
              height={'50px'}
              onClick={() => setCurrentPage(startPage - 10)}
            />
          )}
          {pageNumbers.map((page) => (
            <PageButton
              content={page}
              width={'50px'}
              height={'50px'}
              onClick={() => setCurrentPage(page)}
              active={page === currentPage ? true : false}
            />
          ))}
          {startPage < totalPage && (
            <PageNextButton
              width={'50px'}
              height={'50px'}
              onClick={() => setCurrentPage(startPage + 10)}
            />
          )}
        </div>
      </div>
      <MapleFooter />
    </>
  );
}
