import React, { useEffect, useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import axios from 'axios';
import Header from '../../Components/MapleHeader';
import Footer from '../../Components/MapleFooter';
import Podium from '../../Assets/Ranking/podium.svg';
import Crown from '../../Assets/Ranking/crown.svg';
import Box from '../../Components/Box/Box';

export default function Ranking() {
  const [rankData, setRankData] = useState([]);
  const [searchParams] = useSearchParams();
  const songName = searchParams.get('song_name');

  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

  useEffect(() => {
    const fetchRankData = async () => {
      try {
        const res = await axios.get(`${BACKEND_URL}/rank`, {
          params: { song_name: songName },
        });

        const normalized = res.data.map((item) => ({
          ...item,
          accuracy: item.score,
          profileImg: item.profile_url,
        }));

        setRankData(normalized);
      } catch (error) {
        console.error('랭킹 불러오기 실패:', error);
        alert('랭킹 정보를 불러오는 데 실패했습니다.');
      }
    };

    if (songName) {
      fetchRankData();
    }
  }, [songName, BACKEND_URL]);

  const top3 = rankData.slice(0, 3);

  return (
    <div className="flex flex-col min-h-screen">
      <div className="flex flex-col items-center flex-grow relative mt-6">
        {rankData.length === 0 ? (
          <div className="text-center mt-20 text-xl font-semibold text-gray-500">
            해당 곡의 랭킹이 없습니다
          </div>
        ) : (
          <>
            {/* Podium + Top 3 */}
            <div className="relative w-full flex justify-center items-end mt-6">
              <img src={Podium} alt="podium" className="mt-[320px]" />
              <img
                src={Crown}
                alt="crown"
                className="absolute top-[-2%] z-10"
              />

              {top3.map((user, idx) => {
                const positions = [
                  'top-[12%]', // 1등
                  'top-[20%] right-[66%]', // 2등
                  'top-[24%] left-[66%]', // 3등
                ];
                return (
                  <div key={idx} className={`absolute ${positions[idx]}`}>
                    <Box width="200px" height="244px">
                      <div className="flex justify-center items-center mt-4">
                        <img
                          src={user.profileImg}
                          alt={`${idx + 1}등 프로필 이미지`}
                          className="w-[160px] h-[160px] object-cover border mt-3"
                        />
                      </div>
                      <div className="flex flex-col items-center mt-2">
                        <span className="text-lg font-semibold">
                          {user.nickname}
                        </span>
                        <span className="text-md">{user.accuracy}%</span>
                      </div>
                    </Box>
                  </div>
                );
              })}
            </div>

            {/* 전체 랭킹 테이블 */}
            <table id="table" className="w-[80%] h-[80%] mt-8 mb-20">
              <thead>
                <tr className="border-y-2 border-[#A57865] h-12">
                  <th className="w-[10%] text-xl text-center py-4">No.</th>
                  <th className="font-bold w-[50%] text-xl text-center py-4">
                    닉네임
                  </th>
                  <th className="w-[10%] text-xl text-center py-4">정확도</th>
                </tr>
              </thead>
              <tbody>
                {rankData.slice(3).map((item) => (
                  <tr
                    key={item.rank}
                    className="border-b-[1px] border-[#A57865] h-12"
                  >
                    <td className="text-center py-4 text-lg">{item.rank}</td>
                    <td className="text-center py-4 text-lg">
                      {item.nickname}
                    </td>
                    <td className="text-center py-4 text-lg">
                      {item.accuracy}%
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </>
        )}
      </div>

      <Footer className="absolute bottom-0 w-full" />
    </div>
  );
}
