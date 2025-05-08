import { useEffect, useRef, useState } from 'react';
import WaveSurfer from 'wavesurfer.js';
import Timeline from 'wavesurfer.js/dist/plugins/timeline.esm.js';
import RegionsPlugin from 'wavesurfer.js/dist/plugins/regions.esm.js';
import AudioChart from './AudioChart';
import swal from 'sweetalert';

export default function CompareWaveform({ userAudio, referenceAudio }) {
  const userRef = useRef(null);
  const refRef = useRef(null);
  const userWave = useRef(null);
  const refWave = useRef(null);

  const [waveWidth, setWaveWidth] = useState(500); // Width
  const [audioRate, setAudioRate] = useState(1); // Playback speed
  const [userCurrentTime, seUserCurrentTime] = useState(0); // 연주곡 재생 위치
  const [refCurrentTime, setRefCurrentTime] = useState(0); // 연주곡 재생 위치
  const [isPlaying, setIsPlaying] = useState(false);

  useEffect(() => {
    if (!userRef.current || !refRef.current) return;

    userWave.current = WaveSurfer.create({
      container: userRef.current,
      url: userAudio,
      backend: 'MediaElement',
      waveColor: '#4ade80',
      progressColor: '#dd5e98',
      height: 100,
      responsive: true,
      width: waveWidth,
      barWidth: 2,
      barGap: 1,
      barRadius: 2,
      dragToSeek: true,
      plugins: [Timeline.create(), RegionsPlugin.create()],
    });

    refWave.current = WaveSurfer.create({
      container: refRef.current,
      url: referenceAudio,
      backend: 'MediaElement',
      waveColor: '#60a5fa',
      progressColor: '#dd5e98',
      height: 100,
      responsive: true,
      width: waveWidth,
      barWidth: 2,
      barGap: 1,
      barRadius: 2,
      dragToSeek: true,
      plugins: [Timeline.create(), RegionsPlugin.create()],
    });

    // 재생 위치 실시간 업데이트
    userWave.current.on('audioprocess', () => {
      seUserCurrentTime(userWave.current.getCurrentTime());
    });

    refWave.current.on('audioprocess', () => {
      setRefCurrentTime(refWave.current.getCurrentTime());
    });

    return () => {
      userWave.current?.destroy();
      refWave.current?.destroy();
    };
  }, [userAudio, referenceAudio]);

  const handleWidthChange = (e) => {
    const newWidth = Number(e.target.value);
    setWaveWidth(newWidth);
    userWave.current?.setOptions({ width: newWidth });
    refWave.current?.setOptions({ width: newWidth });
  };

  const handleAudioRateChange = (e) => {
    const rate = Number(e.target.value);
    setAudioRate(rate);
    userWave.current?.setPlaybackRate(rate);
    refWave.current?.setPlaybackRate(rate);
  };

  const handlePlayBoth = () => {
    const isNowPlaying = userWave.current?.isPlaying();
    userWave.current?.playPause().catch((err) => {
      swal({
        text: `로딩중... ` + '\n' + '잠시후 다시 실행시켜주세요!',
        icon: 'info',
        buttons: {
          confirm: {
            text: '확인',
            className: 'custom-confirm-button',
          },
        },
      });
    });
    refWave.current?.playPause().catch((err) => {
      swal({
        text: `로딩중 ...` + '\n' + '잠시후 다시 실행시켜주세요!',
        icon: 'info',
        buttons: {
          confirm: {
            text: '확인',
            className: 'custom-confirm-button',
          },
        },
      });
    });
    setIsPlaying(!isNowPlaying);
  };

  return (
    <div className="flex flex-col items-center space-y-6">
      <div className="flex">
        <div className="flex flex-col">
          <AudioChart
            waveWidth={waveWidth}
            waveRef={refRef}
            refCurrentTime={refCurrentTime.toFixed(2)}
            filter={'원본'}
          />

          <AudioChart
            waveWidth={waveWidth}
            waveRef={userRef}
            refCurrentTime={refCurrentTime.toFixed(2)}
            filter={'연주'}
          />
        </div>
      </div>
      <div className="flex flex-col items-center mt-4">
        <div className="flex ">
          <label className="mb-2 mr-2">
            <strong>Width: {waveWidth}px</strong>
          </label>
          <input
            type="range"
            min="300"
            max="900"
            step="10"
            value={waveWidth}
            onChange={handleWidthChange}
            className="w-64"
          />
        </div>
        <div className="flex ">
          <label className="mb-2 mr-2 ">
            <strong>Speed: {audioRate}x</strong>
          </label>
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          <input
            type="range"
            min="0.5"
            max="2"
            step="0.1"
            value={audioRate}
            onChange={handleAudioRateChange}
            className="w-64"
          />
        </div>
      </div>
      <button
        onClick={handlePlayBoth}
        className="absolute bottom-6 right-6 mt-6 px-6 py-3 rounded-full bg-green-500 hover:bg-green-400 active:scale-95 shadow-lg hover:shadow-2xl flex items-center justify-center text-white text-2xl font-bold transition-all duration-300 hover:-translate-y-1"
      >
        Play/Pause
      </button>
    </div>
  );
}
