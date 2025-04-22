import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import axios from 'axios';
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Album from '../../Assets/Main/album/bndCover.svg';
import PerformanceChart from '../../Components/Chart/PerformanceChart.js';
import playdata from '../../Data/compare.json'; //여기에 음정,박자등을 불러오는 api 넣어
import feedback from '../../Data/feedback_8583d5bf.json';
import { useState } from 'react';
import { useEffect } from 'react';
import ReactMarkdown from 'react-markdown';
import graphData from '../../Data/maplegraph.json';


export default function Feedback() {
  const uid = localStorage.getItem("uid");
  const [record, setRecord] = useState(null);
  const [loading, setLoading] = useState(true);

  const songName = "오늘만 I LOVE YOU";
  const uploadCount = 1;

  const recentSong = [
    { title: '오늘만 I LOVE YOU', artist: 'BOYNEXTDOOR', image: Album },
  ];
  

  function processCompareData(data) {
    const user = data.comparison.user_features;
    const ref = data.comparison.reference_features;
    const feedback = data.feedback;

    // pitch_comparison
    const pitch_comparison = user.pitches.map((userPitch, i) => ({
      note_index: i,
      user_pitch: userPitch,
      reference_pitch: ref.pitches[i],
    }));

    // onset_comparison
    const onset_comparison = user.onsets.map((userOnset, i) => ({
      note_index: i,
      user_onset: userOnset,
      reference_onset: ref.onsets[i],
    }));

    // technique_comparison
    const technique_comparison = user.techniques.map((userTech, i) => ({
      note_index: i,
      user_technique: userTech,
      reference_technique: ref.techniques[i],
  

    // audio URL은 예시로 채워둠 (실제 signed URL은 서버에서 받아야 함)
    const audio_urls = {
      user_audio: 'signed_url_to_user_audio',
      reference_audio: 'signed_url_to_reference_audio',
    };
    return {
      pitch_comparison,
      onset_comparison,
      technique_comparison,
      feedback,
      audio_urls,
    };
  }
  
  
  
  const formatChartData = (noteData) => {
    return noteData.map((note) => ({
      second: parseFloat(note.note_start.toFixed(2)), // X축
      original: Math.round(note.original_pitch),
      played: Math.round(note.user_pitch),
      pitch_difference: note.pitch_difference,
      technique_match: note.technique_match,
    }));

  const processed = processCompareData(playdata, '연주데이터 가공처리');
  console.log(processed);

  console.log(feedback);

  useEffect(() => {
    const fetchSpecificRecord = async () => {
      try {
        const response = await axios.get("/mypage/records/specific", {
          params: {
            uid,
            song_name: songName,
            upload_count: uploadCount,
          },
        });
        setRecord(response.data);
      } catch (error) {
        console.error("특정 연습 기록 조회 실패:", error);
      } finally {
        setLoading(false);
      }
    };

    if (uid) fetchSpecificRecord();
    else {
      console.warn("uid가 없습니다.");
      setLoading(false);
    }
  }, [uid, songName, uploadCount]);

  


  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      <div className="flex flex-1">
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
                <Link to="/mypage" className="text-white">
                  내 정보
                </Link>
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

        {/* 메인 컨텐츠 */}

        <div className="w-full pr-6">
          <div className="flex flex-row mt-5 ml-24 gap-6 items-start">
            <div className="flex-1 mt-28 max-w-[260px]">
              <Box width="96.5%" height="100%" backgroundColor="white" overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto h-full">
                <div className="flex justify-center items-center mt-3">
                  <img src={recentSong.image} alt="Album Cover" className="w-[200px] h-[200px] mt-4 object-cover" />
                </div>
                <div className="flex items-center justify-between px-4 mt-2">
                  <div className="flex flex-col w-full">
                    <span className="text-lg font-semibold truncate">{recentSong.title}</span>
                    <span className="text-lg mt-[-4px] truncate">{recentSong.artist}</span>
                  </div>
                </div>
              </Box>
            </div>


            <div className="flex-[2] ml-16 mt-12 h-full">
              <Box width="99%" backgroundColor="white" overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto height-[400px]">
                <div className="ml-4 mt-5">
                  <span className="font-bold text-[16px] block">연주 분석 그래프</span>
                </div>
                <PerformanceChart data={chartData} className="w-full h-full" />
              </Box>
            </div>
          </div>

          <div className="flex flex-row ml-20 mt-20 gap-6">
            <div className="flex-1 max-w-[260px]">
              <Box width="110%" height="295%" backgroundColor="white" overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto">
                <div className="ml-4 mt-7">
                  <p className="text-lg font-semibold">{recentSong.title}</p>
                  <p className="text-sm text-gray-500">{recentSong.artist}</p>
                  {loading ? (
                    <p className="mt-6 text-blue-300">로딩 중...</p>
                  ) : record ? (
                    <div className="ml-0 mt-5">
                      <p className="text-blue-500">템포: {record.tempo}%</p>
                      <div className="bg-blue-500 h-3 rounded-full" style={{ width: `${record.tempo}%` }}></div>
                      <p className="text-green-500 mt-5">박자: {record.rhythm}%</p>
                      <div className="bg-green-500 h-3 rounded-full" style={{ width: `${record.rhythm}%` }}></div>
                      <p className="text-red-500 mt-5">음정: {record.pitch}%</p>
                      <div className="bg-red-500 h-3 rounded-full" style={{ width: `${record.pitch}%` }}></div>
                    </div>
                  ) : (
                    <p className="mt-6 text-red-500">기록을 찾을 수 없습니다.</p>
                  )}
                </div>
              </Box>
            </div>

            <div className="flex-[2 ml-20">
              <Box width="480%" height="295%" backgroundColor="white" overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto">
                <div className="ml-4 mt-5">
                  <span className="font-bold text-[16px] block">Feedback</span>
                  <ul className="list-disc pl-5 mt-5">
                    <li>박자가 빨라요</li>
                    <li>음정이 맞지 않아요 - 튜닝이 필요해요!</li>
                  </ul>
                </div>
              </Box>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
