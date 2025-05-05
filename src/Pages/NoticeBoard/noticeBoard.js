import React, { useEffect, useState, useMemo } from 'react';
import PageButton from '../../Components/PageButton/PageButton';
import PagePrevButton from '../../Components/PagePrevButton.js/PagePrevButton';
import PageNextButton from '../../Components/PageNextButton/PageNextButton';
import SearchBox from '../../Components/SearchBox/searchBox';
import MapleFooter from '../../Components/MapleFooter';
import { Link, useLocation, useNavigate } from 'react-router-dom';
import { usePostInfoQuery } from '../../Hooks/get/usePostInfoQuery';
import { useViewPutMutation } from '../../Hooks/put/viewPutMutation';
import { useAutoCompleteQuery } from '../../Hooks/get/autoComplete/autoCompleteQuery';
import guitaricon from '../../Assets/electric-guitar.svg';
import musicIcon from '../../../src/Assets/music-note.svg';
import AutoCompleteDropdown from '../../Components/Dropdown/AutoCompleteDropdown';

export default function NoticeBoard() {
  const contentsPerPage = 10; // 한 페이지에 표시될 데이터 수
  const pagesPerBlock = 5; // 한 블록에 표시될 페이지 수
  const { mutate: increaseView } = useViewPutMutation();
  const { data: postInfo } = usePostInfoQuery();

  const [currentPage, setCurrentPage] = useState(() => {
    // 히스토리 복원 시 유지되도록
    return sessionStorage.getItem('lastPage')
      ? parseInt(sessionStorage.getItem('lastPage'))
      : 1;
  });

  const [startPage, setStartPage] = useState(1);
  const [currentData, setCurrentData] = useState([]);
  const [pageNumbers, setPageNumbers] = useState([]);
  const [searchKeyword, setSearchKeyword] = useState('');
  const [showSuggestions, setShowSuggestions] = useState(false);
  const [totalPage, setTotalPage] = useState(
    postInfo ? Math.ceil(postInfo.length / contentsPerPage) : 1,
  );

  const { data: autoCompleteSuggestions } = useAutoCompleteQuery(searchKeyword);

  const navigate = useNavigate();

  // 필터링된 결과
  const filteredData = useMemo(() => {
    let data = postInfo || [];
    if (searchKeyword.trim()) {
      data = data.filter((post) =>
        post.title?.toLowerCase().includes(searchKeyword.toLowerCase()),
      );
    }
    return data.sort((a, b) => parseInt(a.id) - parseInt(b.id));
  }, [searchKeyword, postInfo]);

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

  // 현재 페이지가 바뀔때마다 session storage에 저장
  useEffect(() => {
    sessionStorage.setItem('lastPage', currentPage);
  }, [currentPage]);

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

    //현재 페이지가 범위를 벗어나면 보정
    if (currentPage > pageCount) {
      //총페이지가 3, 현재 페이지가 3일때 다음을 누르면 마지막 페이지 유지
      setCurrentPage(pageCount || 1);
    } else if (currentPage < 1) {
      //1페이진데 이전을 누를경우 1페이지로 setting
      setCurrentPage(1);
    }
  }, [filteredData, currentPage]);

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
          <thead className="text-xs text-black uppercase border-b border-[#d4c2b5] h-[20%]">
            <div className="py-6">
              <th
                scope="col"
                className="text-2xl font-bold h-[10%] w-auto whitespace-nowrap"
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
            <tr id="header" className="border-y-[2px] border-[#A57865] h-[50%]">
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
                className="font-bold w-[50%] text-xl text-lg"
              >
                제목
              </th>
              <th scope="col" className="w-[10%] font-bold text-lg">
                글쓴이
              </th>
              <th scope="col" className="w-[10%] font-bold text-lg">
                작성시간
              </th>
              <th scope="col" className="w-[10%] font-bold text-lg">
                조회수
              </th>
            </tr>
          </thead>
          <tbody>
            {currentData.map((post) => (
              <tr
                key={post.id}
                className="border-b border-[#e1d4c7] truncate hover:bg-[#fdfaf6] transition duration-200 text-center"
              >
                <td className="text-center max-w-[200px] truncate">
                  {post.id}
                </td>
                <td className="text-center hover:text-[#A57865] max-w-[100px] truncate hover:cursor-pointer hover:underline ">
                  <a
                    className="block"
                    onClick={() => {
                      handleClick(post.id);
                      navigate(`/noticeDetail/${post.id}`, {
                        state: {
                          id: post.id,
                          title: post.제목,
                          uid: post.uid,
                          writer: post.작성자,
                          write_time: post.created_at,
                          view: post.조회수,
                          content: post.내용,
                          likes: post.좋아요수,
                          audio_url: post.audio_url,
                          image_url: post.image_url,
                        },
                      });
                    }}
                  >
                    {post.제목}
                  </a>
                </td>
                <td className="text-center max-w-[200px] truncate">
                  {post.작성자}
                </td>
                <td className="text-center max-w-[120px] truncate">
                  {formatDate(Date(post.created_at))}
                </td>
                <td className="text-center">{post.조회수}</td>
              </tr>
            ))}
          </tbody>
        </table>
        {/* 검색창 */}
        <div id="searchBar" className="flex justify-center w-[80%] mt-6">
          <div className="relative w-[25%]">
            <SearchBox
              value={searchKeyword}
              onChange={(e) => {
                setSearchKeyword(e.target.value);
                setShowSuggestions(true);
              }}
              onSearch={(keyword) => {
                setSearchKeyword(keyword);
                setShowSuggestions(false);
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
        <div className="fixed bottom-10 right-20">
          <button
            onClick={() => navigate('/write')}
            className="flex items-center gap-2 px-5 py-3 bg-gradient-to-r from-[#d5b6a2] to-[#A57865] text-white font-semibold rounded-full shadow-md hover:scale-105 transition-transform duration-300"
          >
            <img src={guitaricon} alt="글쓰기버튼" className="h-[50px]" />{' '}
            글쓰기
          </button>
        </div>
        {/* 페이지 네이션 구현 */}
        <div id="pagination" className="flex my-4">
          {startPage != 1 && (
            <PagePrevButton
              width={'50px'}
              height={'50px'}
              onClick={() => setCurrentPage(startPage - pagesPerBlock)}
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
              onClick={() => setCurrentPage(startPage + pagesPerBlock)}
            />
          )}
        </div>
      </div>
      <MapleFooter email={'maple@gmail.com'} />
    </>
  );
}
