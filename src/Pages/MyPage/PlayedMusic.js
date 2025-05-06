import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import Box from '../../Components/Box/Box.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Cover_1 from '../../Assets/Main/album/iveCover.svg';
import { useRecordQuery } from '../../Hooks/MyPage/PlayedMusic/useRecordQuery.js';
import { useUserQuery } from '../../Hooks/MyPage/PlayedMusic/useUserInfoQuery.js';
import { useRecentRecordsQuery } from '../../Hooks/MyPage/PlayedMusic/useRecentRecordQuery.js';
import swal from 'sweetalert';
import { useAuth } from '../../Context/AuthContext.js';

export default function PlayedMusic() {
  const [records, setRecords] = useState([]);
  const { uid } = useAuth();

  const { data: recordData } = useRecordQuery(uid);
  const {
    data: recentRecords,
    isLoading: recentLoading,
    isError: recentError,
  } = useRecentRecordsQuery(uid);
  const { data: userInfo } = useUserQuery(uid);

  useEffect(() => {
    if (recordData) {
      try {
        const allRecords = [];
        for (const [songName, recordsArray] of Object.entries(recordData)) {
          recordsArray.forEach((record) => {
            allRecords.push({ ...record, song: songName });
          });
        }
        setRecords(allRecords || []);
      } catch (error) {
        swal({
          text: `연습 기록 가져오기 실패: ${error}`,
          icon: 'error',
          buttons: {
            confirm: {
              text: '확인',
              className: 'custom-confirm-button',
            },
          },
        });
      }
    }
  }, [recordData]);

  return (
    <div className="min-h-screen">
      <div className="flex">
        {/* Sidebar */}
        <div className="w-[12%] bg-[#463936] text-white p-4 flex flex-col justify-between">
          <div>
            <h2 className="text-md font-bold">MAPLE</h2>
            <ul className="mt-4 space-y-2">
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img
                  src={Information}
                  alt="내 정보 아이콘"
                  className="w-4 h-4"
                />
                <Link to="/mypage" className="text-white">
                  내 정보
                </Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                <Link to="/playedmusic" className="text-white">
                  연주한 곡
                </Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting" className="text-white">
                  관리
                </Link>
              </li>
            </ul>
          </div>
          <div>
            <p className="font-semibold">{userInfo?.nickname || '사용자'}</p>
          </div>
        </div>

        {/* Main Content */}
        <div className="w-[88%] overflow-y-auto p-8">
          {/* 최근 연주한 곡 */}
          <h2 className="text-xl font-bold ml-4 mb-4">최근 연주한 곡</h2>
          <div
            className="grid gap-8 ml-2 justify-items-center"
            style={{
              gridTemplateColumns: 'repeat(auto-fit, minmax(180px, 1fr))',
            }}
          >
            {recentRecords && recentRecords.length > 0 ? (
              recentRecords.map((song, index) => (
                <Box
                  key={song.song_id || index}
                  overwrite="p-4 flex flex-col justify-between w-full"
                >
                  <div className="flex justify-center items-center mt-4">
                    <Link to="/feedback">
                      <img
                        src={song.cover_url || Cover_1}
                        alt="Album Cover"
                        className="object-cover w-full h-auto max-w-[100%]"
                      />
                    </Link>
                  </div>
                  <div className="flex items-center justify-between px-2 mt-2">
                    <div className="flex flex-col w-full">
                      <span className="text-lg font-semibold truncate">
                        {song.song_name}
                      </span>
                      <span className="text-sm mt-[-2px] truncate">
                        {song.artist || 'Unknown Artist'}
                      </span>
                    </div>
                  </div>
                </Box>
              ))
            ) : (
              <p className="ml-8">로딩 중...</p>
            )}
          </div>

          {/* 연습 기록 */}
          <h2 className="text-xl font-bold mt-12 ml-4 mb-4">연습 기록</h2>
          <div className="w-full justify-center mt-4">
            <Box
              backgroundColor="white"
              height="400px"
              overwrite="p-4 overflow-y-auto w-[99%] mx-auto"
            >
              <div className="flex flex-col overflow-y-auto max-h-[100%]">
                <table className="w-full text-left text-sm">
                  <thead>
                    <tr>
                      <th className="py-4 text-sm">날짜</th>
                      <th className="py-4 text-sm">곡명</th>
                      <th className="py-4 text-sm">테크닉</th>
                      <th className="py-4 text-sm">박자</th>
                      <th className="py-4 text-sm">음정</th>
                    </tr>
                  </thead>
                  <tbody>
                    {records.map((record, index) => (
                      <tr key={index} className="border-t">
                        <td className="py-2 text-sm">{record.date}</td>
                        <td className="py-2 text-sm">{record.song}</td>
                        <td className="py-2 text-sm">
                          <div className="w-full bg-gray-200 rounded-full h-3">
                            <div
                              className="bg-blue-500 h-3 rounded-full"
                              style={{ width: `${record.onset}%` }}
                            ></div>
                          </div>
                        </td>
                        <td className="py-1 text-sm">
                          <div className="w-full bg-gray-200 rounded-full h-3">
                            <div
                              className="bg-green-500 h-3 rounded-full"
                              style={{ width: `${record.technique}%` }}
                            ></div>
                          </div>
                        </td>
                        <td className="py-1 text-sm">
                          <div className="w-full bg-gray-200 rounded-full h-3">
                            <div
                              className="bg-red-500 h-3 rounded-full"
                              style={{ width: `${record.pitch}%` }}
                            ></div>
                          </div>
                        </td>
                      </tr>
                    ))}
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
