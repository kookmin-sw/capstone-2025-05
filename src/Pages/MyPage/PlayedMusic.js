import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import axios from 'axios';
import Box from '../../Components/Box/Box.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Cover_1 from '../../Assets/Main/album/iveCover.svg';
import { useRecordQuery } from '../../Hooks/MyPage/PlayedMusic/useRecordQuery.js';
import { useUserQuery } from '../../Hooks/MyPage/PlayedMusic/useUserInfoQuery.js';
import { useRecentRecordsQuery } from '../../Hooks/MyPage/PlayedMusic/useRecentRecordQuery.js';
import swal from 'sweetalert';

export default function PlayedMusic() {
  const [records, setRecords] = useState([]);
  const uid = 'cLZMFP4802a7dwMo0j4qmcxpnY63';

  // api연결
  const { data: recordData } = useRecordQuery(uid);
  const {
    data: recentRecords,
    isLoading: recentLoading,
    isError: recentError,
  } = useRecentRecordsQuery(uid);
  const { data: userInfo } = useUserQuery(uid);
  console.log(recentRecords, '최근 연주한곡');
  console.log(recordData, '기록');
  console.log(userInfo);
  useEffect(() => {
    // record데이터가 호출되면 아래 코드 실행
    console.log('기록응답');
    if (recordData) {
      try {
        const allRecords = [];

        for (const [songName, recordsArray] of Object.entries(recordData)) {
          recordsArray.forEach((record) => {
            allRecords.push({
              ...record,
              song: songName,
            });
          });
        }

        setRecords(allRecords || []);
      } catch (error) {
        swal('', `연습 기록 가져오기 실패:${error}`, 'error');
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
                <Link to="/playedmusic">연주한 곡</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting">관리</Link>
              </li>
            </ul>
          </div>
          <div>
            <p className="font-semibold">{userInfo?.nickname || '사용자'}</p>
          </div>
        </div>

        <div className="w-[100%] overflow-y-auto p-10">
          <h2 className="text-xl font-bold ml-8 mb-6">최근 연주한 곡</h2>
          <div className="flex flex-wrap gap-10 ml-2 justify-start">
            {recentRecords && recentRecords?.length > 0 ? (
              recentRecords.map((song, index) => (
                <Box
                  key={song.song_id || index}
                  width="23%"
                  height="50%"
                  overwrite="h-[18%] p-4 flex flex-col justify-between"
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
                    <div className="flex flex-col w-[full]">
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
              <p>로딩 중...</p>
            )}
          </div>

          {/* 연습 기록 */}
          <h2 className="text-xl font-bold mt-10 ml-8 mb-6">연습 기록</h2>
          <div className="w-full justify-center mt-4 ml-9">
            <Box
              width="96.5%"
              height="600px"
              backgroundColor="white"
              overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto"
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
