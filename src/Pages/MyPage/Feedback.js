import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import axios from 'axios';
import Box from '../../Components/Box/Box.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import PerformanceChart from '../../Components/Chart/PerformanceChart.js';
import BeatChart from '../../Components/Chart/beatChart.js';
import TechniqueChart from '../../Components/Chart/techniqueChart.js';
import playdata from '../../Data/compare.json';
import feedback from '../../Data/feedback_8583d5bf.json';
import fakedata from '../../Data/chartdata.json';
import ReactMarkdown from 'react-markdown';
import { useAuth } from '../../Context/AuthContext.js';
import AudioPlayer from '../../Components/Audio/AudioPlayer.js';

export default function Feedback() {
  const [record, setRecord] = useState(null);
  const [loading, setLoading] = useState(true);
  const [feedbackData, setFeedbackData] = useState('');
  const [userInfo, setUserInfo] = useState({ nickname: '', email: '' });
  const [rank, setRank] = useState(null);
  const [currentGraphIndex, setCurrentGraphIndex] = useState(0);
  const [specificSong, setSpecificSong] = useState({
    title: 'Drowning',
    artist: '',
    cover_url: '',
  });

  const { uid } = useAuth();
  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;
  const songName = 'Drowning';
  const uploadCount = 1;

  const graphs = [
    {
      label: '음정 분석',
      key: 'pitch',
      color: 'red',
      data: fakedata.note_by_note,
    },
    {
      label: '박자 분석',
      key: 'onset',
      color: 'green',
      data: fakedata.note_by_note,
    },
    {
      label: '기술 분석',
      key: 'technique',
      color: 'blue',
      data: fakedata.note_by_note,
    },
  ];

  function processCompareData(data) {
    const user = data.comparison.user_features;
    const ref = data.comparison.reference_features;

    return {
      pitch_comparison: user.pitches.map((p, i) => ({
        note_index: i,
        user_pitch: p,
        reference_pitch: ref.pitches[i],
      })),
      onset_comparison: user.onsets.map((o, i) => ({
        note_index: i,
        user_onset: o,
        reference_onset: ref.onsets[i],
      })),
      technique_comparison: user.techniques.map((t, i) => ({
        note_index: i,
        user_technique: t,
        reference_technique: ref.techniques[i],
      })),
      feedback: data.feedback,
      audio_urls: {
        user_audio: 'signed_url_to_user_audio',
        reference_audio: 'signed_url_to_reference_audio',
      },
    };
  }

  const processed = processCompareData(playdata);
  console.log(fakedata.note_by_note[0]);
  console.log(record);

  const getChartDataByType = (type) => {
    if (type === 'pitch') {
      return processed.pitch_comparison.map((item, i) => ({
        second: parseFloat((i * 0.5).toFixed(2)),
        original: Math.round(item.reference_pitch),
        played: Math.round(item.user_pitch),
        pitch_difference: Math.abs(item.reference_pitch - item.user_pitch),
        technique_match: true,
      }));
    }
    if (type === 'onset') {
      return processed.onset_comparison.map((item, i) => ({
        second: parseFloat((i * 0.5).toFixed(2)),
        original: parseFloat(item.reference_onset.toFixed(2)),
        played: parseFloat(item.user_onset.toFixed(2)),
        pitch_difference: parseFloat(
          Math.abs(item.reference_onset - item.user_onset).toFixed(2),
        ),
        technique_match: true,
      }));
    }
    if (type === 'technique') {
      return processed.technique_comparison.map((item, i) => ({
        second: parseFloat((i * 0.5).toFixed(2)),
        original: item.reference_technique || 'None',
        played: item.user_technique || 'None',
        technique_match: item.user_technique === item.reference_technique,
      }));
    }
    return [];
  };

  const fetchUserInfo = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/get-user-info`, {
        params: { uid },
      });
      console.log('유저 정보 응답 전체:', response.data);
      const { nickname, email } = response.data || {};
      setUserInfo({ nickname, email });
    } catch (error) {
      console.error('유저 정보 가져오기 실패:', error);
    }
  };

  useEffect(() => {
    const fetchSpecificRecord = async () => {
      if (!BACKEND_URL) {
        console.error(
          'REACT_APP_API_DATABASE_URL 환경변수가 설정되지 않았습니다.',
        );
        setLoading(false);
        return;
      }

      try {
        console.log('Fetching specific record...');
        const response = await axios.get(`${BACKEND_URL}/records/specific`, {
          params: {
            uid,
            song_name: songName,
            upload_count: uploadCount,
          },
        });
        console.log('specific record Data:', response.data.artist);

        if (response.data?.record) {
          setRecord(response.data.record);
          setFeedbackData(feedback.text);
          setSpecificSong({
            title: songName,
            artist: response.data.record.artist || '',
            cover_url: response.data.album_cover_url || '',
          });
        } else if (
          response.data?.pitch !== undefined &&
          response.data?.onset !== undefined &&
          response.data?.technique !== undefined
        ) {
          console.log('specific record Data:', response.data);
          setRecord({
            pitch: response.data.pitch,
            onset: response.data.onset,
            technique: response.data.technique,
          });
          setFeedbackData(response.data.feedback || '');
          setSpecificSong({
            title: songName,
            artist: response.data.artist || '',
            cover_url: response.data.song_cover_url || '',
          });
        }

        const rankResponse = await axios.get(
          `${BACKEND_URL}/my-specific-song-rank`,
          {
            params: { uid, song_name: songName },
          },
        );
        console.log('Rank Response:', rankResponse.data);
        setRank(rankResponse.data);
      } catch (error) {
        console.error('요청 오류:', error);
      } finally {
        setLoading(false);
      }
    };

    if (uid) {
      fetchSpecificRecord();
      fetchUserInfo(); // 여기서 유저 정보 가져오기 호출!
    } else {
      setLoading(false);
    }
  }, [uid, songName, uploadCount, BACKEND_URL]); // 추가한 변수도 의존성 배열에 포함시켜야 함

  const handleCompareClick = () => {
    console.log('동시 재생 버튼 클릭');
    // TODO: 모달 추가 예정
  };

  const handlePrevGraph = () => {
    setCurrentGraphIndex((prevIndex) => {
      const newIndex = prevIndex === 0 ? graphs.length - 1 : prevIndex - 1;
      console.log('Prev Graph Index:', newIndex);
      return newIndex;
    });
  };

  const handleNextGraph = () => {
    setCurrentGraphIndex((prevIndex) => {
      const newIndex = prevIndex === graphs.length - 1 ? 0 : prevIndex + 1;
      console.log('Next Graph Index:', newIndex);
      return newIndex;
    });
  };

  return (
    <div className="flex min-h-screen">
      {/* 사이드바 */}
      <div className="w-[12%] bg-[#463936] text-white p-4 flex flex-col justify-between">
        <div>
          <h2 className="text-md font-bold">MAPLE</h2>
          <ul className="mt-4 space-y-2">
            <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
              <img src={Information} alt="내 정보 아이콘" className="w-4 h-4" />
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
          <p className="font-semibold">{userInfo.nickname || '사용자'}</p>
        </div>
      </div>

      {/* 메인 콘텐츠 */}
      <div className="flex-1 overflow-y-auto p-10 space-y-12">
        {/* 곡 정보 + 오디오 */}
        <div className="flex gap-10">
          <Box
            width="300px"
            height="100%"
            backgroundColor="white"
            overwrite="p-6 shadow-lg"
          >
            <div className="flex flex-col items-center space-y-4">
              {specificSong.cover_url ? (
                <img
                  src={specificSong.cover_url}
                  alt="cover"
                  className="w-56 h-56 object-cover rounded-md"
                />
              ) : (
                <div className="w-56 h-56 bg-gray-200 flex items-center justify-center text-gray-400">
                  이미지 없음
                </div>
              )}
              <div className="text-center">
                <h3 className="text-lg font-bold">{specificSong.title}</h3>
                <p className="text-gray-600">{specificSong.artist}</p>
                <p className="mt-2 font-semibold">
                  {rank ? `${rank.rank}위` : '랭크 없음'}
                </p>
              </div>

              <AudioPlayer
                userAudio={'/Audio/homecoming.wav'}
                referenceAudio={'/Audio/homecoming-error-1.wav'}
              />
            </div>
          </Box>

          <div className="flex-[2] ml-4 h-full">
            <Box
              width="100%"
              height="608px"
              backgroundColor="white"
              overwrite="sm:w-[90%] lg:w-[70%] p-4 overflow-y-auto height-[400px]"
            >
              <div className="ml-4 mt-5">
                <span className="font-bold text-[16px] block mb-4">
                  🎧 Feedback
                </span>
                {feedback ? (
                  <div className="prose prose-sm lg:prose-lg prose-slate max-w-none mt-4 leading-relaxed text-gray-700">
                    <ReactMarkdown>{feedback.text}</ReactMarkdown>
                  </div>
                ) : (
                  <p className="mt-5 text-gray-500">피드백이 없습니다.</p>
                )}
              </div>
            </Box>
          </div>
        </div>

        {/* 분석 부분 */}
        <Box width="100%" height="45%" overwrite="p-6 shadow-lg">
          <div className="flex justify-between items-center mb-4">
            <h3 className="font-bold">{graphs[currentGraphIndex].label}</h3>
            <div className="flex gap-2">
              <button
                onClick={handlePrevGraph}
                className="px-4 py-2 bg-gray-200 rounded-full"
              >
                ◀
              </button>
              <button
                onClick={handleNextGraph}
                className="px-4 py-2 bg-gray-200 rounded-full"
              >
                ▶
              </button>
            </div>
          </div>

          {record ? (
            <>
              <div className="flex items-center gap-3 mb-4">
                <p
                  className={`font-bold text-${graphs[currentGraphIndex].color}-500`}
                >
                  {record[graphs[currentGraphIndex].key]}%
                </p>
                <div className="flex-1 bg-gray-300 rounded-full h-2 overflow-hidden">
                  <div
                    className={`bg-${graphs[currentGraphIndex].color}-500 h-full`}
                    style={{
                      width: `${record[graphs[currentGraphIndex].key]}%`,
                    }}
                  />
                </div>
              </div>
              <div className="flex justify-center">
                {graphs.map((_, index) => (
                  <div
                    key={index}
                    className={`w-2 h-2 rounded-full mx-1 ${
                      currentGraphIndex === index ? 'bg-black' : 'bg-gray-300'
                    }`}
                  />
                ))}
              </div>

              {graphs[currentGraphIndex].key === 'technique' ? (
                <TechniqueChart data={getChartDataByType('technique')} />
              ) : graphs[currentGraphIndex].key === 'onset' ? (
                <BeatChart data={getChartDataByType('onset')} />
              ) : (
                <PerformanceChart data={getChartDataByType('pitch')} />
              )}
            </>
          ) : (
            <p>로딩 중...</p>
          )}
        </Box>
      </div>
    </div>
  );
}
