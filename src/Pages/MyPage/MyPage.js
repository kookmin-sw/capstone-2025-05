import React, { useState, useEffect } from 'react';
import axios from 'axios';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Profile from '../../Assets/MyPage/profile.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Music from '../../Assets/MyPage/Vector.svg';
import Bookmark from '../../Assets/MyPage/filledBookmark.svg';
import Heart from '../../Assets/MyPage/filledHeart.svg';
import Write from '../../Assets/MyPage/wirte.svg';
import ImprovementChart from '../../Components/Chart/ImprovementChart.js';
import { Link } from 'react-router-dom';

export default function MyPage() {
  const [records, setRecords] = useState([]);
  const [userInfo, setUserInfo] = useState({ nickname: '', email: '' });
  const [loading, setLoading] = useState(true);
  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;
  const uid = localStorage.getItem("uid") || "cLZMFP4802a7dwMo0j4qmcxpnY63";

  useEffect(() => {
    const fetchRecentRecords = async () => {
      try {
        const response = await axios.get(`${BACKEND_URL}/recent-4-record`, {
          params: { uid },
        });
        console.log("최근 기록 응답:", response.data);
        setRecords(response.data.recent_uploads || []);
      } catch (error) {
        console.error("연습 기록 가져오기 실패:", error);
      } finally {
        setLoading(false);
      }
    };

    const fetchUserInfo = async () => {
      try {
        const response = await axios.get(`${BACKEND_URL}/get-user-info`, {
          params: { uid },
        });
        console.log("유저 정보 응답:", response.data["user information"]);
        const { nickname, email, profile_image } = response.data["user information"] || {};
        setUserInfo({ nickname, email, profile_image });
      } catch (error) {
        console.error("유저 정보 가져오기 실패:", error);
      }
    };

    fetchRecentRecords();
    fetchUserInfo();
  }, [BACKEND_URL, uid]);

  return (
    <div className="min-h-screen">
      <Header />
      <div className="flex">
        {/* Sidebar */}
        <div className="w-[12%] bg-[#463936] text-white p-4 flex flex-col justify-between">
          <div>
            <h2 className="text-md font-bold">MAPLE</h2>
            <ul className="mt-4 space-y-2">
              <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                <img src={Information} alt="내 정보 아이콘" className="w-4 h-4" />
                <Link to="/mypage" className="text-white">내 정보</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                <Link to="/playedmusic">연주한 곡</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting">관리</Link>
              </li>
            </ul>
          </div>
          <div>
            <p className="font-semibold">{userInfo.nickname || "사용자"}</p>
          </div>
        </div>

        {/* Main Content */}
        <div className="w-[88%] overflow-y-auto p-16 ml-5">
          {/* 실력 향상 그래프 + 프로필 */}
          <div className="flex gap-16 h-[50vh]">
            <Box width="70%" height="100%">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[20px] block">실력 향상 그래프</span>
              </div>
              <div className="flex justify-center h-[85%]">
                <ImprovementChart alt="실력 향상 그래프" />
              </div>
            </Box>

            <div className="w-[25%] flex justify-center">
              <Link to="/setting" className="w-full h-full">
                <Box width="100%" height="100%" backgroundColor="#463936">
                  <div className="p-8 text-white w-full h-full">
                    <img src={Profile} alt="프로필" className="w-40 h-40 rounded-full mx-auto cursor-pointer" />
                    <h3 className="text-center text-xl font-bold mt-6">{userInfo.nickname || "홍길동"}</h3>
                    <div className="mt-16 ml-4">
                      <p className="text-xl">닉네임</p>
                      <p className="font-semibold text-xl">{userInfo.nickname}</p>
                    </div>
                    <div className="mt-16 ml-4">
                      <p className="text-xl">이메일</p>
                      <p className="font-semibold text-xl">{userInfo.email}</p>
                    </div>
                  </div>
                </Box>
              </Link>
            </div>
          </div>

          {/* 최근 연주한 곡 + 나의 활동 */}
          <div className="flex gap-16 h-[50vh] mt-16">
            <Box width="70%" height="100%">
              <div className="flex items-center ml-4 mt-5">
                <span className="font-bold text-[20px]">최근 연주한 곡</span>
                <Link to="/playedmusic" className="ml-2 text-lg text-gray-500 hover:text-gray-700">&gt;</Link>
              </div>
              <ul className="mt-8 space-y-5 ml-8">
                {loading ? (
                  <li>Loading...</li>
                ) : (
                  records.map((song, index) => (
                    <li key={index} className="flex items-center justify-between pb-2 mb-2">
                      <img src={song.cover_url} alt="Album" className="w-16 h-16 mr-8 mt-7" />
                      <div className="flex flex-col justify-center flex-grow">
                        <p className="font-semibold text-[20px] mt-7 truncate">{song.song_name}</p>
                        <p className="text-[15px] text-gray-600 mt-0">Artist Name</p>
                      </div>
                    </li>
                  ))
                )}
              </ul>
            </Box>

            <Box width="25%" height="100%">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[20px]">나의 활동</span>
                <Link to="/myactivity" className="ml-2 text-lg text-gray-500 hover:text-gray-700">&gt;</Link>
              </div>
              <div className="ml-5 mt-10 space-y-4 flex-grow">
                <div className="rounded-lg p-7 flex items-center gap-8">
                  <img src={Bookmark} alt="북마크" className="w-10 h-10" />
                  <Link to="/myactivity" className="text-black text-[16px] font-semibold hover:underline">북마크</Link>
                </div>
                <div className="rounded-lg p-7 flex items-center gap-8">
                  <img src={Heart} alt="좋아요" className="w-10 h-10" />
                  <Link to="/myactivity" className="text-black text-[16px] font-semibold hover:underline">좋아요</Link>
                </div>
                <div className="rounded-lg p-7 flex items-center gap-8">
                  <img src={Write} alt="내가 쓴 글" className="w-10 h-10" />
                  <Link to="/myactivity" className="text-black text-[16px] font-semibold hover:underline">내가 쓴 글</Link>
                </div>
                <div className="rounded-lg p-5 flex items-center gap-8">
                  <img src={Profile} alt="프로필" className="w-12 h-12 " />
                  <Link to="/setting" className="text-black text-[16px] font-semibold hover:underline">{userInfo.nickname || "길동1234"}</Link>
                </div>
              </div>
            </Box>
          </div>
        </div>
      </div>
    </div>
  );
}
