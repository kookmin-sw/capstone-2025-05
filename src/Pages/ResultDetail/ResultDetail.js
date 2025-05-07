import React, { useEffect, useRef, useState } from 'react';
import { Link, useNavigate, useParams } from 'react-router-dom';
import axios from 'axios';
import Box from '../../Components/Box/Box.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import PerformanceChart from '../../Components/Chart/PerformanceChart.js';
import BeatChart from '../../Components/Chart/beatChart.js';
import TechniqueChart from '../../Components/Chart/techniqueChart.js';
import ReactMarkdown from 'react-markdown';
import { useAuth } from '../../Context/AuthContext.js';
import AudioPlayer from '../../Components/Audio/AudioPlayer.js';
import fakeResult from '../../Data/resultDetail.json';
import { FaPrint } from 'react-icons/fa6';

const API_BASE_URL = process.env.REACT_APP_RESULT_URL;

export default function ResultDetail() {
  const { uid } = useAuth();
  const printRef = useRef();

  const [record, setRecord] = useState(null);
  const [loading, setLoading] = useState(true);
  const [userInfo, setUserInfo] = useState({ nickname: '', email: '' });
  const [currentGraphIndex, setCurrentGraphIndex] = useState(0);
  const [specificSong, setSpecificSong] = useState({
    title: 'Test Song',
    artist: 'Unknown',
    cover_url: '',
  });
  const navigate = useNavigate();

  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

  const [result, setResult] = useState(null);
  const { taskId } = useParams();

  function processCompareData(data) {
    const user = data.result.user_features;
    const ref = data.result.reference_features;

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
      feedback: data.result.feedback,
      scores: data.result.scores,
    };
  }

  const processed = result ? processCompareData(result) : null;

  const graphs = [
    { label: '음정 분석', key: 'pitch', color: 'red' },
    { label: '박자 분석', key: 'rhythm', color: 'green' },
    { label: '기술 분석', key: 'technique', color: 'blue' },
  ];

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
        original: Array.isArray(item.reference_technique)
          ? item.reference_technique.join(', ')
          : item.reference_technique,
        played: Array.isArray(item.user_technique)
          ? item.user_technique.join(', ')
          : item.user_technique,
        technique_match:
          JSON.stringify(item.user_technique) ===
          JSON.stringify(item.reference_technique),
      }));
    }
    return [];
  };

  const fetchUserInfo = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/get-user-info`, {
        params: { uid },
      });
      const { nickname, email } = response.data || {};
      setUserInfo({ nickname, email });
    } catch (error) {
      console.error('유저 정보 가져오기 실패:', error);
    }
  };

  const handlePrint = () => {
    const printContent = printRef.current.innerHTML;
    const originalContent = document.body.innerHTML;

    document.body.innerHTML = printContent;
    window.print();
    document.body.innerHTML = originalContent;
    window.location.reload();
  };

  useEffect(() => {
    const fetchResultDetail = async () => {
      try {
        setLoading(true);
        const response = await axios.get(`${API_BASE_URL}/results/${taskId}`);
        setResult(response.data);
        setLoading(false);
      } catch (err) {
        setLoading(false);
      }
    };

    fetchResultDetail();
  }, [taskId]);

  useEffect(() => {
    if (uid) fetchUserInfo();
  }, [uid]);

  return (
    <div className="flex min-h-screen">
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
        <p className="font-semibold">{userInfo?.nickname || '사용자'}</p>
      </div>

      <div ref={printRef} className="flex-1 overflow-y-auto p-10 space-y-12">
        <div className="flex gap-10">
          <Box
            width="300px"
            height="100%"
            backgroundColor="white"
            overwrite="p-6 shadow-lg"
          >
            <div className="flex flex-col items-center space-y-4">
              <div className="w-56 h-56 bg-gray-200 flex items-center justify-center text-gray-400 rounded-md">
                이미지 없음
              </div>
              <div className="text-center">
                <h3 className="text-lg font-bold">{specificSong.title}</h3>
                <p className="text-gray-600">{specificSong.artist}</p>
              </div>
              <AudioPlayer
                userAudio="/Audio/homecoming.wav"
                referenceAudio="/Audio/homecoming-error-1.wav"
              />
            </div>
          </Box>

          <div className="flex-[2] ml-4 h-full">
            <Box
              width="100%"
              height="608px"
              backgroundColor="white"
              overwrite="p-4 overflow-y-auto"
            >
              <div className="ml-4 mt-5">
                <div className="font-bold text-2xl flex items-center justify-between mb-4">
                  <span>🎧 Feedback</span>
                  <button onClick={handlePrint} className="hover: cursor">
                    <FaPrint />
                  </button>
                </div>
                <div className="prose prose-sm lg:prose-lg prose-slate max-w-none mt-4 leading-relaxed text-gray-700">
                  <ReactMarkdown>{processed.feedback}</ReactMarkdown>
                </div>
              </div>
            </Box>
          </div>
        </div>

        <Box width="100%" height="45%" overwrite="p-6 shadow-lg">
          <div className="flex justify-between items-center mb-4">
            <h3 className="font-bold">{graphs[currentGraphIndex].label}</h3>
            <div className="flex gap-2">
              <button
                onClick={() =>
                  setCurrentGraphIndex((prev) =>
                    prev === 0 ? graphs.length - 1 : prev - 1,
                  )
                }
                className="px-4 py-2 bg-gray-200 rounded-full"
              >
                ◀
              </button>
              <button
                onClick={() =>
                  setCurrentGraphIndex((prev) =>
                    prev === graphs.length - 1 ? 0 : prev + 1,
                  )
                }
                className="px-4 py-2 bg-gray-200 rounded-full"
              >
                ▶
              </button>
            </div>
          </div>

          <div className="flex items-center gap-3 mb-4">
            <p
              className={`font-bold text-${graphs[currentGraphIndex].color}-500`}
            >
              {processed.scores[
                `${graphs[currentGraphIndex].key}_match_percentage`
              ].toFixed(1)}
              %
            </p>
            <div className="flex-1 bg-gray-300 rounded-full h-2 overflow-hidden">
              <div
                className={`bg-${graphs[currentGraphIndex].color}-500 h-full`}
                style={{
                  width: `${processed.scores[`${graphs[currentGraphIndex].key}_match_percentage`].toFixed(1)}%`,
                }}
              />
            </div>
          </div>

          {graphs[currentGraphIndex].key === 'technique' ? (
            <TechniqueChart data={getChartDataByType('technique')} />
          ) : graphs[currentGraphIndex].key === 'onset' ? (
            <BeatChart data={getChartDataByType('onset')} />
          ) : (
            <PerformanceChart data={getChartDataByType('pitch')} />
          )}
        </Box>
      </div>
    </div>
  );
}
