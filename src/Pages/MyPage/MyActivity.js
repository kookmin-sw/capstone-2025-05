import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { Link } from 'react-router-dom';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Music from '../../Assets/MyPage/Vector.svg';
import Bookmark from '../../Assets/MyPage/filledBookmark.svg';
import Heart from '../../Assets/MyPage/filledHeart.svg';
import Write from '../../Assets/MyPage/wirte.svg';

export default function MyActivity() {
  const [bookmarks, setBookmarks] = useState([]);
  const [likes, setLikes] = useState([]);
  const [myPosts, setMyPosts] = useState([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const uid = "cLZMFP4802a7dwMo0j4qmcxpnY63";
  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

  useEffect(() => {
    if (!uid) return;

    const fetchData = async () => {
      setLoading(true);
      try {
        const [bookmarkRes, likeRes, myPostRes] = await Promise.all([
          axios.get(`${BACKEND_URL}/mypage/my-scraps`, { params: { uid } }),
          axios.get(`${BACKEND_URL}/mypage/my-likes`, { params: { uid } }),
          axios.get(`${BACKEND_URL}/mypage/my-posts`, { params: { uid } }),
        ]);

        const parseData = (data) =>
          Array.isArray(data) ? data.map(item => ({
            title: item.title || "제목 없음",
            content: item.content || "내용 없음"
          })) : [];

        setBookmarks(parseData(bookmarkRes.data));
        setLikes(parseData(likeRes.data));
        setMyPosts(parseData(myPostRes.data));
        setError(null);
      } catch (err) {
        console.error("데이터 가져오는 중 오류:", err);
        setError("데이터를 가져오는 데 실패했습니다.");
      } finally {
        setLoading(false);
      }
    };

    fetchData();
  }, [uid, BACKEND_URL]);

  const renderList = (data) =>
    data.map((item, idx) => (
      <div key={idx} className="p-2 border rounded bg-gray-50 shadow-sm">
        <p className="font-semibold truncate">{item.title}</p>
        <p className="text-sm text-gray-600 truncate">{item.content}</p>
      </div>
    ));

  return (
    <div className="flex flex-col">
      <Header />
      <div className="flex">
        <div className="w-[12%] h-[790px] bg-[#463936] text-white p-4 flex flex-col justify-between">
          <div>
            <h2 className="text-md font-bold">MAPLE</h2>
            <ul className="mt-4 space-y-2">
              <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                <img src={Information} alt="내 정보 아이콘" className="w-4 h-4" />
                <Link to="/mypage" className="text-white">내 정보</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                <Link to="/playedmusic" className="text-white">연주한 곡</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting" className="text-white">관리</Link>
              </li>
            </ul>
          </div>
        </div>

        <div className="flex-grow p-6 bg-[#F5F1EC] min-h-screen">
          <div className="flex gap-12 mt-5 ml-7">
            {/* 북마크 */}
            <Box width="300px" height="530px">
              <div className="p-3 flex items-center gap-3 border-b">
                <img src={Bookmark} alt="북마크" className="w-8 h-8" />
                <span className="font-bold text-[18px]">북마크</span>
              </div>
              <div className="h-[470px] overflow-y-auto p-3 space-y-2">
                {loading ? <p>로딩 중...</p> :
                  bookmarks.length === 0 ? <p className="text-gray-500">북마크한 게시글이 없습니다.</p> :
                    renderList(bookmarks)}
              </div>
            </Box>

            {/* 좋아요 */}
            <Box width="300px" height="530px">
              <div className="p-3 flex items-center gap-3 border-b">
                <img src={Heart} alt="좋아요" className="w-8 h-8" />
                <span className="font-bold text-[18px]">좋아요</span>
              </div>
              <div className="h-[470px] overflow-y-auto p-3 space-y-2">
                {loading ? <p>로딩 중...</p> :
                  likes.length === 0 ? <p className="text-gray-500">좋아요한 게시글이 없습니다.</p> :
                    renderList(likes)}
              </div>
            </Box>

            {/* 내가 쓴 글 */}
            <Box width="300px" height="530px">
              <div className="p-3 flex items-center gap-3 border-b">
                <img src={Write} alt="내 글" className="w-8 h-8" />
                <span className="font-bold text-[18px]">내가 쓴 글</span>
              </div>
              <div className="h-[470px] overflow-y-auto p-3 space-y-2">
                {loading ? <p>로딩 중...</p> :
                  myPosts.length === 0 ? <p className="text-gray-500">작성한 게시글이 없습니다.</p> :
                    renderList(myPosts)}
              </div>
            </Box>
          </div>

          {error && (
            <div className="mt-6 ml-7 text-red-600 font-semibold">
              ⚠ {error}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
