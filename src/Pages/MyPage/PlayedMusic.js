import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { Link } from 'react-router-dom';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Cover_1 from '../../Assets/Main/album/iveCover.svg';
import Cover_2 from '../../Assets/Main/album/bndCover.svg';
import Cover_3 from '../../Assets/Main/album/riizeCover.svg';
import Cover_4 from '../../Assets/Main/album/gdCover.svg';

const recentSongs = [
  { title: 'REBEL HEART', artist: 'IVE', image: Cover_1 },
  { title: '오늘만 I LOVE YOU', artist: 'BOYNEXTDOOR', image: Cover_2 },
  { title: 'COMBO', artist: 'RIZE', image: Cover_3 },
  { title: 'HOME SWEET HOME', artist: 'G-DRAGON', image: Cover_4 },
];

export default function PlayedMusic() {
  const [records, setRecords] = useState([]);
  const [loading, setLoading] = useState(true);
  const uid = localStorage.getItem("uid");

  useEffect(() => {
    const fetchRecords = async () => {
      try {
        const response = await axios.get("/mypage/records/all", {
          params: { uid },
        });
        setRecords(response.data);
      } catch (error) {
        console.error("연습 기록 가져오기 실패:", error);
      } finally {
        setLoading(false);
      }
    };

    if (uid) {
      fetchRecords();
    } else {
      setLoading(false);
      console.warn("uid가 없습니다. 로그인 상태를 확인하세요.");
    }
  }, [uid]);
  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      {/* 헤더 아래 콘텐츠 */}
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
            <p className="font-semibold">Kildong Hong</p>
          </div>
        </div>

        {/* 본문 */}
        <div className="w-[100%] overflow-y-auto p-10">
          <h2 className="text-xl font-bold ml-8 mb-6">최근 연주한 곡</h2>
          <div className="flex flex-wrap gap-44 ml-36 justify-start">
            {recentSongs.map((song, index) => (
              <Box key={index} className="w-[40%] sm:w-[55%] md:w-[30%] mx-auto" overwrite={"h-[50%] p-4 flex flex-col justify-between"}>
                <div className="flex justify-center items-center mt-4">
                  <Link to="/feedback">
                    <img
                      src={song.image}
                      alt="Album Cover"
                      className={`object-cover ${
                        song.title === 'HOME SWEET HOME' ? 'w-[240px] h-[240px]' : 'w-full h-auto max-w-[100%]'
                      }`}
                    />
                  </Link>
                </div>
                <div className="flex items-center justify-between px-4 mt-2">
                  <div className="flex flex-col w-[140px]">
                    <span className="text-lg font-semibold truncate">{song.title}</span>
                    <span className="text-sm mt-[-4px] truncate">{song.artist}</span>
                  </div>
                </div>
              </Box>
            ))}
          </div>

          {/* 연습 기록 */}
          <h2 className="text-xl font-bold mt-10 ml-8 mb-6">연습 기록</h2>
          <div className="w-full justify-centermt-4 ml-9">
          <Box width="96.5%" height="600px" backgroundColor="white" overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto">
              <div className="flex flex-col overflow-y-auto max-h-[100%]">
                <table className="w-full text-left text-sm">
                  <thead>
                    <tr>
                      <th className="py-4 text-sm">날짜</th>
                      <th className="py-4 text-sm">곡명</th>
                      <th className="py-4 text-sm">템포</th>
                      <th className="py-4 text-sm">박자</th>
                      <th className="py-4 text-sm">음정</th>
                    </tr>
                  </thead>
                  <tbody>
                    {loading ? (
                      <tr>
                        <td colSpan="5" className="text-center py-8">로딩 중...</td>
                      </tr>
                    ) : records.length > 0 ? (
                      records.map((record, index) => (
                        <tr key={index} className="border-t">
                          <td className="py-2 text-sm">{record.date}</td>
                          <td className="py-2 text-sm">{record.song}</td>
                          <td className="py-2 text-sm">
                            <div className="w-full bg-gray-200 rounded-full h-3">
                              <div className="bg-blue-500 h-3 rounded-full" style={{ width: `${record.tempo}%` }}></div>
                            </div>
                          </td>
                          <td className="py-1 text-sm">
                            <div className="w-full bg-gray-200 rounded-full h-3">
                              <div className="bg-green-500 h-3 rounded-full" style={{ width: `${record.rhythm}%` }}></div>
                            </div>
                          </td>
                          <td className="py-1 text-sm">
                            <div className="w-full bg-gray-200 rounded-full h-3">
                              <div className="bg-red-500 h-3 rounded-full" style={{ width: `${record.pitch}%` }}></div>
                            </div>
                          </td>
                        </tr>
                      ))
                    ) : (
                      <tr>
                        <td colSpan="5" className="text-center py-8">연습 기록이 없습니다.</td>
                      </tr>
                    )}
                  </tbody>
                </table>
              </div>
            </Box>
          </div>
        </div>
      </div>
    </div>
  );
}
