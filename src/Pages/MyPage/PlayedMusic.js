import React from 'react';
import { Link } from 'react-router-dom';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Cover_1 from '../../Assets/MyPage/앨범커버1.svg';
import Cover_2 from '../../Assets/MyPage/앨범커버2.svg';
import Cover_3 from '../../Assets/MyPage/앨범커버3.svg';
import Cover_4 from '../../Assets/MyPage/앨범커버4.svg';

const recentSongs = [
  { title: 'REBEL HEART', artist: 'IVE', image: Cover_1 },
  { title: '오늘만 I LOVE YOU', artist: 'BOYNEXTDOOR', image: Cover_2 },
  { title: 'COMBO', artist: 'RIZE', image: Cover_3 },
  { title: 'HOME SWEET HOME', artist: 'G-DRAGON', image: Cover_4 }
];

const practiceRecords = [
  { date: '2025-02-01', song: 'Canon Rock', tempo: 85, rhythm: 90, pitch: 80 },
  { date: '2025-01-28', song: 'Hotel California', tempo: 80, rhythm: 85, pitch: 78 },
  { date: '2025-01-27', song: 'Stand By Me', tempo: 88, rhythm: 87, pitch: 82 },
  { date: '2025-01-26', song: 'Wonderwall', tempo: 75, rhythm: 80, pitch: 76 },
  { date: '2025-01-25', song: 'Hey Jude', tempo: 92, rhythm: 89, pitch: 85 },
  { date: '2025-01-24', song: 'Canon Rock', tempo: 85, rhythm: 84, pitch: 83 }
];

export default function MyPage() {
  return (
    <div className="flex flex-col">
      <Header />
      <div className="flex">
        <div className="w-[220px] h-[770px] bg-[#463936] text-white p-4 flex flex-col justify-between">
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
            <button className="mt-2 text-sm text-red-400 hover:underline">탈퇴하기</button>
          </div>
        </div>

        <div className="w-full mt-5 ml-[20px]"> 
          <h2 className="text-xl font-bold mb-6">최근 연주한 곡</h2>
          <div className="flex flex-wrap gap-12 mt-4 ml-4">
          {recentSongs.map((song, index) => (
            <Box key={index} width="220px" height="288px">
              <div className="flex justify-center items-center mt-4">
                <img
                  src={song.image}
                  alt="Album Cover"
                  className="w-[200px] h-[200px] object-cover"
                />
              </div>
              <div className="flex items-center justify-between px-4 mt-2">
                <div className="flex flex-col w-[140px]">
                  <span className="text-lg font-semibold truncate">{song.title}</span>
                  <span className="text-lg mt-[-4px] truncate">{song.artist}</span>
                </div>
                <button className="w-10 h-10 rounded-full bg-[#A57865] flex items-center justify-center shadow-none hover:bg-white hover:shadow-xl transition-colors group">
                  <div className="w-0 h-0 border-l-[8px] border-l-white border-t-[4px] border-b-[4px] border-t-transparent border-b-transparent group-hover:border-l-[#A57865] transition-colors"></div>
                </button>
              </div>

              </Box>
            ))}
          </div>
            
          
          <h2 className="text-xl font-bold mt-8 mb-6">연습 기록</h2>
          <div className="w-full justify-center items-centermt-12 ml-[25px] mb-11">
            <Box width="1010px" height="280px" className="bg-white p-4 rounded-lg mt-4">
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
                  {practiceRecords.map((record, index) => (
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
                  ))}
                </tbody>
              </table>
            </Box>
          </div>
        </div>
      </div>
      </div>
  );
}
