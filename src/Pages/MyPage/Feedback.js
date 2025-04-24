import React, { useState, useEffect } from 'react';
import axios from 'axios';
import { Link } from 'react-router-dom';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Album from '../../Assets/Main/album/bndCover.svg';
import PerformanceChart from '../../Components/Chart/PerformanceChart.js';

export default function Feedback() {

  const [userInfo, setUserInfo] = useState({ nickname: '', email: '' });
  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;
  const uid = localStorage.getItem("uid") || "cLZMFP4802a7dwMo0j4qmcxpnY63";

  const record = {
      tempo: 92,
      rhythm: 89,
      pitch: 85,
    };

  const recentSongs = [
    { title: "오늘만 I LOVE YOU", artist: "BOYNEXTDOOR", image: Album },
  ];

  const chartData = Array.from({ length: 901 }, (_, i) => {
    const second = parseFloat((i * 0.1).toFixed(1));
    const base = 740 + Math.sin(i / 15) * 25; // original: 자연스러운 곡선
    const played = base + (Math.random() - 0.5) * 12; // played: ±6 오차
  
    return {
      second,
      original: Math.round(base),
      played: Math.round(played),
    };
  });

  useEffect(() => {
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
      fetchUserInfo();
    }, [BACKEND_URL, uid]);
  
  

  return (
    <div className="flex flex-col h-screen">
      <Header />
      <div className="flex flex-1">
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

        {/* 메인 컨텐츠 */}
        <div className="w-full mt-12 ml-[50px]"> 
          
          {/* 곡 박스 & 실력 향상 그래프 박스 나란히 배치 */}
        <div className="flex flex-row mt-2 ml-2 gap-12 items-start">
          <div className="mt-3">
            {recentSongs.map((song, index) => (
              <Box key={index} width="220px" height="288px">
                <div className="flex justify-center items-center mt-3">
                  <img
                    src={song.image}
                    alt="Album Cover"
                    className="w-[200px] h-[200px] mt-4 object-cover"
                  />
                </div>
                <div className="flex items-center justify-between px-4 mt-2">
                  <div className="flex flex-col w-[140px]">
                    <span className="text-lg font-semibold truncate">{song.title}</span>
                    <span className="text-lg mt-[-4px] truncate">{song.artist}</span>
                  </div>
                </div>
              </Box>
            ))}
           </div>

            <Box width="750px" height="340px" className="mt-[-10px]">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">연주 분석 그래프</span>
              </div>
              <PerformanceChart data={chartData} alt="연주 분석 그래프" className="mt-6 ml-12 w-[600px]" />
            </Box>
          </div>

  
          <div className="flex flex-row mt-10 gap-10">
            {/* 분석 박스 */}
            <Box width="235px" height="280px">
              <div className="ml-4 mt-7">
                <p className="text-lg font-semibold">{recentSongs[0].title}</p>
                <p className="text-sm text-gray-500">{recentSongs[0].artist}</p>
                <div className="ml-0 mt-5">
                <p className="text-blue-500">템포: {record.tempo}%</p>
                <div className="bg-blue-500 h-3 rounded-full" style={{ width: `${record.tempo}%` }}></div>
                <p className="text-green-500 mt-5">박자: {record.rhythm}%</p>
                <div className="bg-green-500 h-3 rounded-full" style={{ width: `${record.rhythm}%` }}></div>
                <p className="text-red-500 mt-5">음정: {record.pitch}%</p>
                <div className="bg-red-500 h-3 rounded-full" style={{ width: `${record.pitch}%` }}></div>
              </div>
              </div>
            </Box>

            {/* 피드백 박스 */}
            <Box width="750px" height="280px">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">Feedback</span>
                <ul className="list-disc pl-5 mt-5">
                <li>박자가 빨라요</li>
                <li>음정이 맞지 않아요 - 튜닝이 필요해요!</li>
                </ul>
              </div>
            </Box>
          </div>
        </div>
      </div>
    </div>
  );
}
