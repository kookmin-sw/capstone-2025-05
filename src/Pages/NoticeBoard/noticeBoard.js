import React, { useEffect, useState } from 'react';
import PageButton from '../../Components/PageButton/PageButton';
import PagePrevButton from '../../Components/PagePrevButton.js/PagePrevButton';
import PageNextButton from '../../Components/PageNextButton/PageNextButton';
import SearchBox from '../../Components/SearchBox/searchBox';
import Button from '../../Components/Button/Button';
import MapleHeader from '../../Components/MapleHeader';
import MapleFooter from '../../Components/MapleFooter';
import { Link, useNavigate } from 'react-router-dom';
import { usePostInfoQuery } from '../../Hooks/get/usePostInfoQuery';
import { useViewPutMutation } from '../../Hooks/put/viewPutMutation';
import { useTopViewedPostsQuery } from '../../Hooks/get/useTopViewedPosts';
import { FaFilter } from 'react-icons/fa6';
import guitaricon from '../../Assets/electric-guitar.svg';
export default function NoticeBoard() {
  const contentsPerPage = 10; // 한 페이지에 표시될 데이터 수
  const pagesPerBlock = 10; // 한 블록에 표시될 페이지 수
  const { mutate: increaseView } = useViewPutMutation();

  const { data: postInfo } = usePostInfoQuery();
  const totalPage = postInfo ? Math.ceil(postInfo.length / contentsPerPage) : 1;

  const [currentPage, setCurrentPage] = useState(1);
  const [startPage, setStartPage] = useState(1);
  const [currentData, setCurrentData] = useState([]);
  const [pageNumbers, setPageNumbers] = useState([]);

  const navigate = useNavigate();

  const handleClick = (post_id) => {
    increaseView({ post_id: post_id });
  };
  function formatDate(dateString) {
    if (!dateString) return '-';
    const parsed = new Date(dateString);
    if (isNaN(parsed.getTime())) return '-'; // invalid date
    return parsed.toISOString().slice(0, 10).replace(/-/g, '.');
  }

  // 현재 페이지 기준으로 시작 페이지 계산
  useEffect(() => {
    const newStartPage =
      Math.floor((currentPage - 1) / pagesPerBlock) * pagesPerBlock + 1;
    setStartPage(newStartPage);
  }, [currentPage]);

  // 페이지 버튼 리스트 계산
  useEffect(() => {
    const endPage = Math.min(startPage + pagesPerBlock - 1, totalPage);
    setPageNumbers(
      Array.from({ length: endPage - startPage + 1 }, (_, i) => startPage + i),
    );
  }, [startPage, totalPage]);

  // 현재 페이지에 해당하는 데이터 추출
  useEffect(() => {
    if (postInfo && Array.isArray(postInfo)) {
      const startIndex = (currentPage - 1) * contentsPerPage;
      setCurrentData(postInfo.slice(startIndex, startIndex + contentsPerPage));
    } else {
      setCurrentData([]); // 데이터가 없을 때 빈 배열 유지
    }
  }, [currentPage, postInfo]);

  return (
    <>
      <MapleHeader />
      <div className="flex flex-col items-center h-[100svh]">
        {/* <table id="table" className="w-[80%] h-[80%] m-auto">
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
            {currentData?.map((item, index) => (
              <tr key={item?.id} className="border-b-[1px] border-">
                <td className="text-center">{item?.id}</td>
                <td className="text-center hover:text-[#A57865] hover:cursor-pointer hover:underline">
                  <Link
                    to={`/noticeDetail/${item?.id}`}
                    state={{
                      //추후에 백엔드 필드명으로 변경
                      id: item?.id,
                      title: item?.제목,
                      writer: item?.작성자,
                      write_time: item?.작성일시,
                      view: item?.조회수,
                      content: item?.내용,
                      likes: item?.좋아요수,
                    }}
                    onClick={() => handleClick(item?.id)}
                  >
                    {item?.제목}
                  </Link>
                </td>
                <td className="text-center">{item?.작성자}</td>
                <td className="text-center">{item?.작성일시}</td>
                <td className="text-center">{item?.조회수}</td>
              </tr>
            ))}
          </tbody>
        </table> */}
        <table className="w-full text-sm text-left text-gray-600">
          <thead className="text-xs text-[#5f4532] uppercase bg-[#f4ece2] border-b border-[#d4c2b5]">
            <tr>
              <th scope="col" className="px-6 py-4 text-center w-[10%]">
                No.
              </th>
              <th
                scope="col"
                className="px-6 py-4 text-center font-bold w-[50%] text-center"
              >
                제목
              </th>
              <th scope="col" className="px-6 py-4 text-center w-[10%]">
                글쓴이
              </th>
              <th scope="col" className="px-6 py-4 text-center w-[10%]">
                작성일
              </th>
              <th scope="col" className="px-6 py-4 text-center w-[10%]">
                조회수
              </th>
            </tr>
          </thead>
          <tbody>
            {currentData.map((post, idx) => (
              <tr
                key={post.id}
                className="border-b border-[#e1d4c7] hover:bg-[#fdfaf6] transition duration-200 text-center"
              >
                <td className="px-6 py-4 font-medium text-center">{post.id}</td>
                <td className="px-6 py-4 text-[#5f4532] hover:underline cursor-pointer text-center">
                  <Link
                    to={`/noticeDetail/${post.id}`}
                    state={{
                      //추후에 백엔드 필드명으로 변경
                      id: post.id,
                      title: post.제목,
                      writer: post.작성자,
                      write_time: post.작성일시,
                      view: post.조회수,
                      content: post.내용,
                      likes: post.좋아요수,
                    }}
                    onClick={() => handleClick(post.id)}
                  >
                    {post.제목}
                  </Link>
                </td>
                <td className="px-6 py-4 text-center">{post.작성자}</td>
                <td className="px-6 py-4 text-center">
                  {formatDate(post.작성일시)}
                </td>
                <td className="px-6 py-4 text-center">{post.조회수}</td>
              </tr>
            ))}
          </tbody>
        </table>

        <div id="searchBar" className="flex justify-center w-[80%] mt-6">
          <SearchBox width={'300px'} height={'40px'} />
        </div>
        <div className="fixed bottom-10 right-10">
          <button
            onClick={() => navigate('/write')}
            className="flex items-center gap-2 px-5 py-3 bg-gradient-to-r from-[#d5b6a2] to-[#A57865] text-white font-semibold rounded-full shadow-md hover:scale-105 transition-transform duration-300"
          >
            <img src={guitaricon} alt="글쓰기버튼" className="h-[30px]" />{' '}
            글쓰기
          </button>
        </div>
        {/* 페이지 네이션 구현 */}
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
