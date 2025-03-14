import React from 'react';
import { Link } from 'react-router-dom'; 
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Improvement from '../../Assets/MyPage/실력 향상 그래프.svg';
import Profile from '../../Assets/MyPage/profile.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Music from '../../Assets/MyPage/Vector.svg';
import Album from '../../Assets/MyPage/album.svg';
import Volume from '../../Assets/MyPage/volume.svg';


export default function MyPage() {
  return (
    <div className="flex flex-col">
      <Header />
      <div className="flex">
        <div className="w-[190px] h-[770px] bg-[#463936] text-white p-4 flex flex-col justify-between">
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
                <Link to="/admin">관리</Link>
              </li>
            </ul>
          </div>
          <div>
            <p className="font-semibold">Kildong Hong</p>
            <button className="mt-2 text-sm text-red-400 hover:underline">
              탈퇴하기
            </button>
          </div>
        </div>

        <div className="flex flex-col flex-grow p-6 gap-8 ml-3">
          <div className="flex gap-4">
            <Box width="750px" height="350px">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">실력 향상 그래프</span>
              </div>
              <img src={Improvement} alt="실력 향상 그래프" className="mt-6 ml-12 w-[480px]" />
            </Box>

            <Box width="250px" height="350px" backgroundColor="#463936">
              <div className="p-4 text-white">
                <img src={Profile} alt="프로필" className="w-14 h-14 rounded-full mx-auto" />
                <h3 className="text-center text-xs font-bold mt-4">홍길동</h3>
                <p className="text-center text-[10px]">gildong1234@gmail.com</p>
                <div className="mt-9">
                  <p className="text-xs">닉네임</p>
                  <p className="font-semibold text-xs">길동1234</p>
                </div>
                <div className="mt-9">
                  <p className="text-xs">이메일</p>
                  <p className="font-semibold text-xs">gildong1234@gmail.com</p>
                </div>
              </div>
            </Box>
          </div>

          <div className="flex gap-4">
            <Box width="750px" height="350px">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">최근 연주한 곡</span>
              </div>
              <ul className="mt-8 space-y-5 ml-4">
                {[
                  { title: "Sweet Home Alabama", artist: "Lynyrd Skynyrd" },
                  { title: "Zombie", artist: "The Cranberries" },
                  { title: "Esperanza", artist: "Listening to Young Hug" },
                  { title: "Creep", artist: "Radiohead" },
                ].map((song, index) => (
                  <li key={index} className={`flex items-center justify-between pb-2 mb-2 ${index === 3 ? '' : 'border-b border-gray-300'}`}>
                    <img src={Album} alt="Album" className="w-6 h-6 mr-2" />
                    <div className="flex flex-col justify-center flex-grow">
                      <p className="font-semibold text-[10px] mt-2 truncate">{song.title}</p>
                      <p className="text-[9px] text-gray-600 mt-0">{song.artist}</p>
                    </div>
                    <img src={Volume} alt="Boulum" className="w-5 h-5 mr-4 object-contain" />
                  </li>
                ))}
              </ul>
            </Box>

            <Box width="250px" height="350px">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">나의 활동</span>
              </div>
              <ul className="mt-4 ml-4">
                {Array(4).fill("가입 인사 드립니다.").map((text, index) => (
                  <li key={index} className={`flex items-center gap-3 pb-5 mb-3 ${index === 3 ? '' : 'border-b border-gray-300'}`}>
                    <img src={Profile} alt="프로필" className="w-6 h-6 rounded-full" />
                    <div>
                      <p className="font-semibold text-[10px]">{text}</p>
                      <p className="text-[8px] text-gray-600">2025년 02월 01일</p>
                    </div>
                  </li>
                ))}
              </ul>
            </Box>
          </div>
        </div>
      </div>
    </div>
  );
}
