import React from 'react';
import { Link } from 'react-router-dom';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Profile from '../../Assets/MyPage/profile.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Music from '../../Assets/MyPage/Vector.svg';
import Album from '../../Assets/MyPage/album.svg';
import Volume from '../../Assets/MyPage/volume.svg';
import Bookmark from '../../Assets/MyPage/filledBookmark.svg';
import Heart from '../../Assets/MyPage/filledHeart.svg';
import Write from '../../Assets/MyPage/wirte.svg';
import ImprovementChart from '../../Components/Chart/ImprovementChart.js';

export default function MyPage() {
  return (
    <div className="min-h-screen">
        <Header />
      {/* 헤더 아래 콘텐츠 */}
      <div className="flex ">
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
            <p className="font-semibold">Kildong Hong</p>
          </div>
        </div>

        {/* Main Content */}
        <div className="w-[88%] overflow-y-auto p-16 ml-5">
          {/* Chart + Profile */}
          <div className="flex gap-16 h-[50vh] ">
            <Box width="70%" height="100%">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">실력 향상 그래프</span>
              </div>
              <div className="flex justify-center h-[85%]">
                <ImprovementChart alt="실력 향상 그래프" />
              </div>
            </Box>

            <div className="w-[25%] flex justify-center ">
              <Link to="/setting" className="w-full h-full">
                <Box width="100%" height="100%" backgroundColor="#463936">
                  <div className="p-4 text-white w-full h-full">
                    <img src={Profile} alt="프로필" className="w-15 h-15 rounded-full mx-auto cursor-pointer" />
                    <h3 className="text-center text-s font-bold mt-4">홍길동</h3>
                    <div className="mt-8 ml-4">
                      <p className="text-s">닉네임</p>
                      <p className="font-semibold text-xs">길동1234</p>
                    </div>
                    <div className="mt-8 ml-4">
                      <p className="text-s">이메일</p>
                      <p className="font-semibold text-xs">gildong1234@gmail.com</p>
                    </div>
                  </div>
                </Box>
              </Link>
            </div>
          </div>

          {/* Played Music + My Activity */}
          <div className="flex gap-16 h-[50vh] mt-16">
            <Box width="70%" height="100%">
              <div className="flex items-center ml-4 mt-5">
                <span className="font-bold text-[16px]">최근 연주한 곡</span>
                <Link to="/playedmusic" className="ml-2 text-lg text-gray-500 hover:text-gray-700">&gt;</Link>
              </div>
              <ul className="mt-8 space-y-5 ml-4">
                {[
                  { title: "Sweet Home Alabama", artist: "Lynyrd Skynyrd" },
                  { title: "Zombie", artist: "The Cranberries" },
                  { title: "Esperanza", artist: "Listening to Young Hug" },
                  { title: "Creep", artist: "Radiohead" },
                ].map((song, index) => (
                  <li key={index} className={"flex items-center justify-between pb-2 mb-2 ${index === 3 ? '' : 'border-b border-gray-300'"}>
                    <img src={Album} alt="Album" className="w-6 h-6 mr-2" />
                    <div className="flex flex-col justify-center flex-grow">
                      <p className="font-semibold text-[10px] mt-2 truncate">{song.title}</p>
                      <p className="text-[9px] text-gray-600 mt-0">{song.artist}</p>
                    </div>
                    <img src={Volume} alt="Volume" className="w-5 h-5 mr-4 object-contain" />
                  </li>
                ))}
              </ul>
            </Box>

            <Box width="25%" height="100%">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px]">나의 활동</span>
                <Link to="/myactivity" className="ml-2 text-lg text-gray-500 hover:text-gray-700">&gt;</Link>
              </div>
              <div className="ml-5 mt-8 space-y-4 flex-grow">
                <div className="rounded-lg p-3 flex items-center gap-3">
                  <img src={Bookmark} alt="북마크" className="w-6 h-6" />
                  <Link to="/my-scraps" className="text-black text-[16px] font-semibold hover:underline">북마크</Link>
                </div>
                <div className="rounded-lg p-3 flex items-center gap-3">
                  <img src={Heart} alt="좋아요" className="w-6 h-6" />
                  <Link to="/my-likes" className="text-black text-[16px] font-semibold hover:underline">좋아요</Link>
                </div>
                <div className="rounded-lg p-3 flex items-center gap-3">
                  <img src={Write} alt="내가 쓴 글" className="w-6 h-6" />
                  <Link to="/my-posts" className="text-black text-[16px] font-semibold hover:underline">내가 쓴 글</Link>
                </div>
                <div className="flex items-center gap-4">
                  <img src={Profile} alt="프로필" className="w-12 h-12 " />
                  <Link to="/setting" className="text-black text-[16px] font-semibold hover:underline">길동1234</Link>
                </div>
              </div>
            </Box>
          </div>
        </div>
      </div>
    </div>
  );
}