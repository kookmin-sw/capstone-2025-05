import React, { useEffect, useRef, useState } from 'react';
import { Link, useNavigate, useParams } from 'react-router-dom';
import axios from 'axios';
import Box from '../../Components/Box/Box.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import 음반 from '../../Assets/Main/음반.svg';
import PerformanceChart from '../../Components/Chart/PerformanceChart.js';
import BeatChart from '../../Components/Chart/beatChart.js';
import TechniqueChart from '../../Components/Chart/techniqueChart.js';
import ReactMarkdown from 'react-markdown';
import { useAuth } from '../../Context/AuthContext.js';
import AudioPlayer from '../../Components/Audio/AudioPlayer.js';
import { FaPrint } from 'react-icons/fa6';
import { useMemo } from 'react';
import { useLocation } from 'react-router-dom';
import { useSongByIdQuery } from '../../Hooks/Audio/get/getSongById.js';
import CircularProgressBar from '../../Components/Graph/CircularProgressBar.js';
import useFileDownloader from '../../Hooks/useFileDownloader.js';
import mediaApi from '../../Utils/audioApi.js';
import Button from '../../Components/Button/Button.js';

const ANALYSIS_URL = process.env.REACT_APP_ANALYSIS_URL;
const MEDIA_URL = process.env.REACT_APP_MEDIA_URL;
const COVER_URL = process.env.REACT_APP_COVER_URL;
export default function ResultDetail() {
  const { uid } = useAuth();
  const printRef = useRef();
  const location = useLocation().state;
  const typeName = location.type;
  const song_id = location.song_id; //location.song_id
  const user_id = typeName === 'userResults' ? uid : location.uid; //type이 userResult인 경우만 uid가 내 계정
  const downloadFile = useFileDownloader();
  const { data: songData } = useSongByIdQuery(song_id);
  console.log(songData, '노래정보');

  const [loading, setLoading] = useState(true);
  const [userInfo, setUserInfo] = useState({ nickname: '', email: '' });

  const specificSong = {
    title: songData?.title,
    artist: songData?.artist,
    cover_url: COVER_URL + '/' + songData?.thumbnail,
    original_audio_url: COVER_URL + '/' + songData?.audio,
  };

  console.log('오디오 url', specificSong.original_audio_url);

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

  const processed = useMemo(() => {
    if (!result) return null;
    return processCompareData(result);
  }, [result]);

  const overall_score = useMemo(() => {
    return processed?.scores.overall_score;
  }, [processed]);
  console.log(processed, '가공데이터');

  const graphs = [
    { label: '음정 분석', key: 'pitch', color: 'red' },
    { label: '박자 분석', key: 'rhythm', color: 'green' },
    { label: '기술 분석', key: 'technique', color: 'blue' },
  ];

  const getChartDataByType = (processed, type) => {
    if (type === 'pitch') {
      return (
        processed?.pitch_comparison.map((item, i) => ({
          second: parseFloat((i * 0.5).toFixed(2)),
          original: Math.round(item.reference_pitch),
          played: Math.round(item.user_pitch),
          pitch_difference: Math.abs(item.reference_pitch - item.user_pitch),
          technique_match: true,
        })) || []
      );
    }
    if (type === 'rhythm') {
      return (
        processed?.onset_comparison.map((item, i) => ({
          second: parseFloat((i * 0.5).toFixed(2)),
          original: Math.round(item.reference_onset),
          played: Math.round(item.user_onset),
          pitch_difference: Math.round(
            Math.abs(item.reference_onset - item.user_onset),
          ),
          technique_match: true,
        })) || []
      );
    }
    if (type === 'technique') {
      return (
        processed?.technique_comparison.map((item, i) => ({
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
        })) || []
      );
    }
    return [];
  };

  const fetchUserInfo = async () => {
    try {
      const response = await axios.get(`${BACKEND_URL}/get-user-info`, {
        params: { uid: user_id },
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
        const response = await axios.get(`${ANALYSIS_URL}/results/${taskId}`);
        console.log(response, 'resultdetail');
        setResult(response.data);
        setLoading(false);
      } catch (err) {
        setLoading(false);
      }
    };

    fetchResultDetail();
  }, [taskId]);

  useEffect(() => {
    if (user_id) fetchUserInfo();
  }, [user_id]);

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

      <div
        ref={printRef}
        className="flex-1 overflow-y-auto p-10 space-y-12 print-area"
      >
        <div className="flex gap-10">
          <Box
            width="300px"
            height="100%"
            backgroundColor="white"
            overwrite="p-6 shadow-lg h-full"
          >
            <div className="flex flex-col items-center space-y-4">
              <div className="w-56 h-56 bg-gray-200 flex items-center justify-center text-gray-400 rounded-md">
                <img src={specificSong.cover_url} className="" />
              </div>
              <div className="text-center">
                <h3 className="text-lg font-bold">{specificSong.title}</h3>
                <p className="text-gray-600">{specificSong.artist}</p>
              </div>
              {/* <AudioPlayer
                userAudio={specificSong.original_audio_url}
                referenceAudio={specificSong.original_audio_url}
              /> */}

              <button
                className={
                  'w-56 h-10 font-bold bg-[#fcc5ae] rounded-[10px] text-white text-center flex items-center justify-center'
                }
                onClick={() =>
                  downloadFile(
                    `${MEDIA_URL}/songs/${song_id}/audio`,
                    `${specificSong.title}.wav`,
                  )
                }
              >
                💿 음원 다운로드
              </button>
              <button
                className={
                  'w-56 h-10 font-bold bg-[#bf9684] rounded-[10px] text-white'
                }
                onClick={() =>
                  downloadFile(
                    `${MEDIA_URL}/songs/${song_id}/sheet`,
                    `${specificSong.title}_악보.gp5`,
                  )
                }
              >
                📄 악보 다운로드
              </button>
              <p className="text-sm text-gray-600 mt-2">
                ※ 다운로드된 gp5 악보 파일은{' '}
                <a
                  href="https://sourceforge.net/projects/tuxguitar/"
                  target="_blank"
                  className="text-blue-500 underline"
                >
                  TuxGuitar
                </a>{' '}
                또는 Guitar Pro 프로그램으로 열어 PDF로 저장할 수 있어요.
              </p>
            </div>
          </Box>

          <div className="flex-[2] ml-4">
            <Box
              width="100%"
              height="100%"
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
                {userInfo?.nickname && overall_score && (
                  <div className="text-lg font-semibold mb-2 text-gray-800">
                    <span className="font-bold text-xl ">
                      {userInfo.nickname}
                    </span>
                    님의 총점은{' '}
                    <span className=" font-bold text-xl">
                      {parseInt(overall_score)}
                    </span>
                    점입니다!
                    <span>
                      &nbsp;
                      {overall_score >= 90
                        ? '매우 훌륭한 연주였어요🎉'
                        : overall_score >= 80
                          ? '점점 더 좋아지고 있어요! 조금만 더 힘내요 💪'
                          : '조금 아쉬운 결과예요. 꾸준히 연습하면 분명 더 좋아질 거예요!'}
                    </span>
                  </div>
                )}
                <div className=" prose prose-sm lg:prose-lg prose-slate max-w-none mt-4 leading-relaxed text-gray-700 print:overflow-visible print:w-auto">
                  <ReactMarkdown>{processed?.feedback}</ReactMarkdown>
                  <div className="flex justify-center mt-8">
                    {graphs.map((graph) => (
                      <CircularProgressBar
                        key={graph.key}
                        graph={graph}
                        percentage={
                          processed?.scores?.[
                            `${graph.key}_match_percentage`
                          ] ?? 0
                        }
                      />
                    ))}
                  </div>
                </div>
              </div>
            </Box>
          </div>
        </div>

        <div className="space-y-6">
          {graphs.map((graph) => (
            <Box
              key={graph.key}
              width="100%"
              overwrite="p-6 shadow-lg print:overflow-visible print:w-auto"
            >
              <h3 className={`text-xl font-bold text-${graph.color}-500 mb-4`}>
                {graph.label}
              </h3>
              {graph.key === 'technique' ? (
                <TechniqueChart
                  data={getChartDataByType(processed, 'technique')}
                />
              ) : graph.key === 'rhythm' ? (
                <BeatChart data={getChartDataByType(processed, 'rhythm')} />
              ) : (
                <PerformanceChart
                  data={getChartDataByType(processed, 'pitch')}
                />
              )}
            </Box>
          ))}
        </div>
      </div>
    </div>
  );
}
