import React, { useEffect, useState, useMemo } from 'react';
import PageButton from '../../Components/PageButton/PageButton';
import PagePrevButton from '../../Components/PagePrevButton.js/PagePrevButton';
import PageNextButton from '../../Components/PageNextButton/PageNextButton';
import SearchBox from '../../Components/SearchBox/searchBox';
import MapleFooter from '../../Components/MapleFooter';
import { Link, useNavigate } from 'react-router-dom';
import { usePostInfoQuery } from '../../Hooks/get/usePostInfoQuery';
import { useViewPutMutation } from '../../Hooks/put/viewPutMutation';
import { useAutoCompleteQuery } from '../../Hooks/get/autoComplete/autoCompleteQuery';
import guitaricon from '../../Assets/electric-guitar.svg';
import musicIcon from '../../../src/Assets/music-note.svg';
import AutoCompleteDropdown from '../../Components/Dropdown/AutoCompleteDropdown';

export default function NoticeBoard() {
  const contentsPerPage = 10; // 한 페이지에 표시될 데이터 수
  const pagesPerBlock = 10; // 한 블록에 표시될 페이지 수
  const { mutate: increaseView } = useViewPutMutation();

  console.log(increaseView, '조회수');

  const { data: postInfo } = usePostInfoQuery();
  console.log(postInfo, 'postinfo');

  const [currentPage, setCurrentPage] = useState(1);
  const [startPage, setStartPage] = useState(1);
  const [currentData, setCurrentData] = useState([]);
  const [pageNumbers, setPageNumbers] = useState([]);
  const [searchKeyword, setSearchKeyword] = useState('');
  const [showSuggestions, setShowSuggestions] = useState(false);
  const [totalPage, setTotalPage] = useState(
    postInfo ? Math.ceil(postInfo.length / contentsPerPage) : 1,
  );

  const navigate = useNavigate();

  const { data: autoCompleteSuggestions } = useAutoCompleteQuery(searchKeyword);

  // 필터링된 결과
  const filteredData = useMemo(() => {
    if (!searchKeyword.trim()) return postInfo || [];
    return postInfo?.filter((post) =>
      post.title?.toLowerCase().includes(searchKeyword.toLowerCase()),
    );
  }, [searchKeyword, postInfo]);

  console.log(filteredData, '필터링데이터');
  console.log(searchKeyword, '← 검색어');
  console.log(autoCompleteSuggestions, '← 자동완성 결과');

  const handleClick = (post_id) => {
    increaseView({ post_id: post_id });
  };

  //날짜 format//
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

  useEffect(() => {
    const length = filteredData?.length || 0;
    const pageCount = Math.ceil(length / contentsPerPage);
    setTotalPage(pageCount || 1);
  }, [filteredData]);

  useEffect(() => {
    const startIndex = (currentPage - 1) * contentsPerPage;

    const targetData = filteredData || [];
    const pageData = targetData.slice(startIndex, startIndex + contentsPerPage);

    setCurrentData(pageData);
  }, [filteredData, currentPage]);
  return (
    <>
      <div className="flex flex-col items-center h-[100svh]">
        <table id="table" className="w-[80%] h-[80%] m-auto">
          <thead className="text-xs text-black uppercase border-b border-[#d4c2b5]">
            <div className="my-6">
              <th
                scope="col"
                className="text-2xl font-bold h-[30px] w-auto whitespace-nowrap"
              >
                <div className="flex items-center">
                  <img
                    src={musicIcon}
                    alt="음악아이콘"
                    width={30}
                    className="mr-4"
                  />
                  자유 게시판
                </div>
              </th>
            </div>
            <tr id="header" className="border-y-[2px] border-[#A57865] h-[6%]">
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
            {currentData.map((post, index) => (
              <tr
                key={post.index}
                className="border-b border-[#e1d4c7] hover:bg-[#fdfaf6] transition duration-200 text-center"
              >
                <td className="text-center">{post.id}</td>
                <td className="text-center hover:text-[#A57865] hover:cursor-pointer hover:underline">
                  <Link
                    to={`/noticeDetail/${post.id}`}
                    state={{
                      //추후에 백엔드 필드명으로 변경
                      id: post.id,
                      title: post.title,
                      uid: post.uid,
                      writer: post.작성자,
                      write_time: formatDate(post.date),
                      view: post.조회수,
                      content: post.내용,
                      likes: post.좋아요수,
                      audio_url: post.audio_url,
                      image_url: post.image_url,
                    }}
                    onClick={() => handleClick(post.id)}
                  >
                    {post.title}
                  </Link>
                </td>
                <td className="text-center">{post.작성자}</td>
                <td className="text-center">{formatDate(post.date)}</td>
                <td className="text-center">{post.조회수}</td>
              </tr>
            ))}
          </tbody>
        </table>

        <div id="searchBar" className="flex justify-center w-[80%] mt-6">
          <div className="relative w-[300px]">
            <SearchBox
              width={'300px'}
              height={'40px'}
              value={searchKeyword}
              onChange={(e) => {
                setSearchKeyword(e.target.value);
                setShowSuggestions(true);
              }}
              onSearch={(keyword) => {
                setSearchKeyword(keyword);
                setShowSuggestions(false);
                // 여기에 필터 적용 로직 또는 navigate 등 추가
              }}
            />
            <AutoCompleteDropdown
              suggestions={autoCompleteSuggestions}
              keyword={searchKeyword}
              onSelect={(s) => {
                setSearchKeyword(s);
                setShowSuggestions(false);
                setCurrentPage(1);
              }}
            />
          </div>
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
      <MapleFooter email={'maple@gmail.com'} />
    </>
  );
}
