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
import Bookmark from '../../Assets/MyPage/bookmark.svg';
import Heart from '../../Assets/MyPage/heart.svg';

export default function MyPage() {
  return (
    <div className="flex flex-col">
      <Header />
      <div className="flex">
        <div className="w-[180px] h-[790px] bg-[#463936] text-white p-4 flex flex-col justify-between">
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

        <div className="flex flex-col flex-grow p-6 gap-8 mt-2 ml-3">
          <div className="flex gap-9">
            <Box width="750px" height="350px">
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block">실력 향상 그래프</span>
              </div>
              <img src={Improvement} alt="실력 향상 그래프" className="mt-6 ml-12 w-[650px]" />
            </Box>
            <Link to="/admin">
            <Box width="250px" height="350px" backgroundColor="#463936">
              <div className="p-4 text-white">
                  <img src={Profile} alt="프로필" className="w-15 h-15 rounded-full mx-auto cursor-pointer" />
                <h3 className="text-center text-s font-bold mt-4">홍길동</h3>
                <div className="mt-8">
                  <p className="text-s">닉네임</p>
                  <p className="font-semibold text-xs">길동1234</p>
                </div>
                <div className="mt-8">
                  <p className="text-s">이메일</p>
                  <p className="font-semibold text-xs">gildong1234@gmail.com</p>
                </div>
              </div>
            </Box>
            </Link>
          </div>

          <div className="flex gap-9">
            <Box width="750px" height="350px">
              <div className="flex items-center ml-4 mt-5">
                <span className="font-bold text-[16px]">최근 연주한 곡</span>
                <Link to="/playedmusic" className="ml-2 text-lg text-gray-500 hover:text-gray-700">
                  &gt;
                </Link>
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
                    <img src={Volume} alt="Volume" className="w-5 h-5 mr-4 object-contain" />
                  </li>
                ))}
              </ul>
            </Box>

            <Box width="250px" height="350px">
              <div className="ml-4 mt-5">
               <span className="font-bold text-[16px] block">나의 활동</span>
              </div>
              <div className="ml-5 mt-10 space-y-4 flex-grow">
                <div className="rounded-lg p-3 flex items-center gap-3">
                  <img src={Bookmark} alt="북마크" className="w-6 h-6" />
                  <Link to="/mypage" className="text-black text-[16px] font-semibold hover:underline">북마크</Link>
              </div>
              <div className="rounded-lg p-3 flex items-center gap-3">
                  <img src={Heart} alt="좋아요" className="w-6 h-6" />
                  <Link to="/mypage" className="text-black text-[16px] font-semibold hover:underline">좋아요</Link>
              </div>
              <div className="rounded-lg p-3 flex items-center gap-3">
                  <img src={Bookmark} alt="내가 쓴 글" className="w-6 h-6" />
                  <Link to="/mypage" className="text-black text-[16px] font-semibold hover:underline">내가 쓴 글</Link>
              </div>
              <div className="flex items-center gap-4">
                  <img src={Profile} alt="프로필" className="w-12 h-12 " />
                  <Link to="/admin" className="text-black text-[16px] font-semibold hover:underline">길동1234</Link>
              </div>
             </div>
          </Box>
          </div>
         </div>
        </div>
      </div>
  );
}
