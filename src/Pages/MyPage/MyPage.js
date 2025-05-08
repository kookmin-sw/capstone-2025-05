import React from 'react';
import Box from '../../Components/Box/Box.js';
import Profile from '../../Assets/MyPage/profile.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Music from '../../Assets/MyPage/Vector.svg';
import Bookmark from '../../Assets/MyPage/filledBookmark.svg';
import Heart from '../../Assets/MyPage/filledHeart.svg';
import Write from '../../Assets/MyPage/wirte.svg';
import ImprovementChart from '../../Components/Chart/ImprovementChart.js';
import Cover1 from '../../Assets/Main/album/bndCover.svg';
import Cover2 from '../../Assets/Main/album/gdCover.svg';
import Cover3 from '../../Assets/Main/album/iveCover.svg';
import Cover4 from '../../Assets/Main/album/riizeCover.svg';
import { Link } from 'react-router-dom';
import { useUserQuery } from '../../Hooks/MyPage/PlayedMusic/useUserInfoQuery.js';
import { useRecentRecordsQuery } from '../../Hooks/MyPage/PlayedMusic/useRecentRecordQuery.js';
import { useAuth } from '../../Context/AuthContext.js';

export default function MyPage() {
  const { uid } = useAuth();
  const { data: userInfo } = useUserQuery(uid);
  const { data: records, isLoading } = useRecentRecordsQuery(uid);

  const recommendedSongs = [
    { cover: Cover1, title: '오늘만 I LOVE YOU', artist: 'BOYNEXTDOOR' },
    { cover: Cover2, title: 'HOME SWEET HOME', artist: 'G-Dragon' },
    { cover: Cover3, title: 'REVEL HEART', artist: 'IVE' },
    { cover: Cover4, title: 'COMBO', artist: 'RIZE' },
  ];

  return (
    <div className="min-h-screen">
      <div className="flex">
        {/* Sidebar */}
        <div className="w-[12%] bg-[#463936] text-white p-4 flex flex-col justify-between">
          <div>
            <h2 className="text-md font-bold">MAPLE</h2>
            <ul className="mt-4 space-y-2">
              <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                <img
                  src={Information}
                  alt="내 정보 아이콘"
                  className="w-4 h-4"
                />
                <Link to="/mypage" className="text-white truncate">
                  내 정보
                </Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                <Link to="/playedmusic" className="truncate">
                  연주한 곡
                </Link>
              </li>

              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting" className="truncate">
                  관리
                </Link>
              </li>
            </ul>
          </div>
          <div>
            <p className="font-semibold truncate">
              {userInfo?.nickname || '사용자'}
            </p>
          </div>
        </div>

        {/* Main Content */}
        <div className="w-[88%] overflow-y-auto p-16 ml-5">
          <div className="flex gap-16 h-[50vh]">
            <Box width="73%" height="100%">
              <div className="ml-5 mt-4">
                <span className="font-bold text-[20px] block">
                  실력 향상 그래프
                </span>
              </div>
              <div className="flex justify-center h-[100%]">
                <ImprovementChart />
              </div>
            </Box>

            <div className="w-[25%] flex justify-center">
              <Link to="/setting" className="w-full h-full">
                <Box width="100%" height="100%" backgroundColor="#463936">
                  <div className="flex flex-col p-8 text-white w-full h-full overflow-y-auto">
                    <img
                      src={userInfo?.profile_image_url || Profile}
                      alt="프로필"
                      className="w-32 h-32 rounded-full mx-auto cursor-pointer"
                    />
                    <h3 className="text-center text-lg font-bold truncate">
                      {userInfo?.nickname || '사용자'}
                    </h3>
                    <div className="mt-12 ml-4">
                      <p className="text-lg">닉네임</p>
                      <p className="font-medium text-base truncate">
                        {userInfo?.nickname}
                      </p>
                    </div>
                    <div className="mt-12 ml-4">
                      <p className="text-lg">이메일</p>
                      <p className="font-medium text-base truncate">
                        {userInfo?.email}
                      </p>
                    </div>
                  </div>
                </Box>
              </Link>
            </div>
          </div>

          <div className="flex gap-16 h-[50vh] mt-16">
            <Box
              width="43%"
              height="105%"
              overwrite="flex flex-col overflow-y-auto"
            >
              <div className="flex items-center ml-2 mt-2">
                <Link
                  to="/playedmusic"
                  className="ml-4 text-xl font-bold duration-300 ease-in-out "
                >
                  <div className="mt-5 flex items-center hover:scale-[105%] duration-300 ease-in-out">
                    <span className="font-bold text-[20px]">
                      최근 연주한 곡
                    </span>
                    &gt;
                  </div>
                </Link>
              </div>
              <ul className="mt-8 space-y-5 ml-8">
                {isLoading ? (
                  <li>Loading...</li>
                ) : records?.length > 0 ? (
                  records.map((song, index) => (
                    <li
                      key={index}
                      className="flex items-center justify-between pb-2 mb-2 min-w-0"
                    >
                      <img
                        src={song.cover_url}
                        alt="Album"
                        className="w-16 h-16 mr-8 mt-7"
                      />
                      <div className="flex flex-col justify-center flex-grow min-w-0">
                        <p className="font-semibold text-[20px] mt-7 truncate overflow-hidden whitespace-nowrap text-ellipsis">
                          {song.song_name}
                        </p>
                        <p className="text-[15px] text-gray-600 mt-0 truncate">
                          {song.artist || '아티스트 미정'}
                        </p>
                      </div>
                    </li>
                  ))
                ) : (
                  <h1 className="font-bold text-xl text-gray-600 mt-7 truncate">
                    최근 연주한 곡이 없습니다...
                  </h1>
                )}
              </ul>
            </Box>

            <Box
              width="25%"
              height="105%"
              overwrite="flex flex-col overflow-y-auto"
            >
              <div className="ml-4 mt-5">
                <span className="font-bold text-[20px]">선호 장르 추천곡</span>
                <p className="font-semibold text-gray-600 mt-7 truncate">
                  어쿠스틱 팝(Acoustic Pop)
                </p>
              </div>

              <ul className="items-center mt-4 ml-8 space-y-5">
                {recommendedSongs.map((song, index) => (
                  <li
                    key={index}
                    className="flex items-center justify-between min-w-0"
                  >
                    <img
                      src={song.cover}
                      alt="Album"
                      className="w-16 h-16 mr-8 mt-4 shrink-0"
                    />
                    <div className="flex flex-col justify-center flex-grow min-w-0">
                      <p className="font-semibold text-[20px] mt-7 truncate">
                        {song.title}
                      </p>
                      <p className="text-[15px] text-gray-600 truncate">
                        {song.artist}
                      </p>
                    </div>
                  </li>
                ))}
              </ul>
            </Box>

            <Box
              width="25%"
              height="105%"
              overwrite="flex flex-col overflow-y-auto"
            >
              <Link
                to="/myactivity"
                className="ml-2 text-xl font-bold duration-300 ease-in-out"
              >
                <div className="ml-4 mt-5 flex items-center hover:scale-[105%] duration-300 ease-in-out">
                  <span>나의 활동</span>
                  &gt;
                </div>
              </Link>
              <div className="ml-5 mt-10 space-y-4 flex-grow">
                {[
                  { icon: Bookmark, label: '북마크' },
                  { icon: Heart, label: '좋아요' },
                  { icon: Write, label: '내가 쓴 글' },
                ].map(({ icon, label }, i) => (
                  <div
                    key={i}
                    className="rounded-lg p-7 flex items-center gap-8"
                  >
                    <img src={icon} alt={label} className="w-10 h-10" />
                    <Link
                      to="/myactivity"
                      className="text-black text-[16px] font-semibold hover:underline truncate"
                    >
                      {label}
                    </Link>
                  </div>
                ))}
                <div className="rounded-lg p-5 flex items-center gap-8">
                  <img src={Profile} alt="프로필" className="w-12 h-12" />
                  <Link
                    to="/setting"
                    className="text-black text-[16px] font-semibold hover:underline truncate"
                  >
                    {userInfo?.nickname || '사용자'}
                  </Link>
                </div>
              </div>
            </Box>
          </div>
        </div>
      </div>
    </div>
  );
}
