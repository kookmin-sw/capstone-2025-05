import React from 'react';
import Header from '../../Components/MapleHeader';
import Footer from '../../Components/MapleFooter';
import Podium from '../../Assets/Ranking/podium.svg';
import Crown from '../../Assets/Ranking/crown.svg';
import rankData from '../../Data/rank_data.json';
import Box from '../../Components/Box/Box';

export default function Ranking() {
  const top3 = rankData.slice(0, 3);

  return (
    <div className="flex flex-col min-h-screen">
      <Header />

      <div className="flex flex-col items-center flex-grow relative mt-6">
        <div className="relative w-full flex justify-center items-end mt-6">
          {/* podium 이미지 */}
          <img src={Podium} alt="podium" className="mt-[320px]" />
          <img src={Crown} alt="crown" className="absolute top-[-2%] z-10" />

          {/* 1등 */}
          <div className="absolute top-[12%]">
            <Box width="200px" height="244px">
              <div className="flex justify-center items-center mt-4">
                <img
                  src={top3[0]?.profileImg}
                  alt="1등 프로필 이미지"
                  className="w-[160px] h-[160px] object-cover border  mt-3"
                />
              </div>
              <div className="flex flex-col items-center mt-2">
                <span className="text-lg font-semibold">
                  {top3[0]?.nickname}
                </span>
                <span className="text-md">{top3[0]?.accuracy}%</span>
              </div>
            </Box>
          </div>

          {/* 2등 */}
          <div className="absolute top-[20%] right-[66%]">
            <Box width="200px" height="244px">
              <div className="flex justify-center items-center mt-4">
                <img
                  src={top3[1]?.profileImg}
                  alt="2등 프로필 이미지"
                  className="w-[160px] h-[160px] object-cover border mt-3"
                />
              </div>
              <div className="flex flex-col items-center mt-2">
                <span className="text-lg font-semibold">
                  {top3[1]?.nickname}
                </span>
                <span className="text-md">{top3[1]?.accuracy}%</span>
              </div>
            </Box>
          </div>

          {/* 3등 */}
          <div className="absolute top-[24%] left-[66%]">
            <Box width="200px" height="244px">
              <div className="flex justify-center items-center mt-4">
                <img
                  src={top3[2]?.profileImg}
                  alt="3등 프로필 이미지"
                  className="w-[160px] h-[160px] object-cover border mt-3"
                />
              </div>
              <div className="flex flex-col items-center mt-2">
                <span className="text-lg font-semibold">
                  {top3[2]?.nickname}
                </span>
                <span className="text-md">{top3[2]?.accuracy}%</span>
              </div>
            </Box>
          </div>
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
            {rankData.map((item) => (
              <tr
                key={item.rank}
                className="border-b-[1px] border-[#A57865] h-12"
              >
                <td className="text-center py-4 text-lg">{item.rank}</td>
                <td className="text-center py-4 text-lg">{item.nickname}</td>
                <td className="text-center py-4 text-lg">{item.accuracy}%</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <Footer className="absolute bottom-0 w-full" />
    </div>
  );
}
