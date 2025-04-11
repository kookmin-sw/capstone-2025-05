import { useEffect, useState } from 'react';
import { Pause, Play } from 'lucide-react';

export default function PlayerBar({
  title,
  artist,
  image,
  isPlay,
  handlePlay,
  handlePause,
  player,
}: any) {
  const [position, setPosition] = useState(0); // ms
  const [duration, setDuration] = useState(1); // ms (1로 초기화해 0 나눗셈 방지)

  // ⏱ 재생 시간 갱신 루프
  useEffect(() => {
    let interval: any;

    if (isPlay && player) {
      interval = setInterval(async () => {
        const state = await player.getCurrentState();
        if (state) {
          setPosition(state.position);
          setDuration(state.duration);
        }
      }, 1000);
    }

    return () => clearInterval(interval);
  }, [isPlay, player]);

  // ⏩ 슬라이더 이동 시 재생 위치 변경
  const handleSeek = (e: React.ChangeEvent<HTMLInputElement>) => {
    const seekTo = Number(e.target.value);
    player?.seek(seekTo);
    setPosition(seekTo);
  };

  // ⏳ mm:ss 포맷
  const formatTime = (ms: number) => {
    const minutes = Math.floor(ms / 60000);
    const seconds = Math.floor((ms % 60000) / 1000);
    return `${minutes}:${seconds < 10 ? '0' : ''}${seconds}`;
  };

  return (
    <div className="fixed bottom-0 left-0 w-full bg-[#222] text-white px-6 py-4 flex items-center justify-between shadow-2xl z-50">
      {/* 왼쪽: 앨범 이미지 + 제목 */}
      <div className="flex items-center gap-4">
        <img
          src={image}
          alt="cover"
          className="w-14 h-14 rounded-md object-cover"
        />
        <div>
          <h3 className="text-sm font-semibold">{title}</h3>
          <p className="text-xs text-gray-300">{artist}</p>
        </div>
      </div>
      {/* 가운데: 플레이/일시정지 버튼 */}
      <div className="flex flex-col items-center w-1/3">
        <button
          onClick={isPlay ? handlePause : handlePlay}
          className="w-10 h-10 mb-1 rounded-full bg-[#A57865] hover:bg-white hover:text-[#A57865] text-white flex items-center justify-center transition-colors"
        >
          {isPlay ? <Pause size={20} /> : <Play size={20} />}
        </button>

        {/* 슬라이더 */}
        <div className="flex items-center gap-2 w-full">
          <span className="text-xs w-10 text-right">
            {formatTime(position)}
          </span>
          <input
            type="range"
            min={0}
            max={duration}
            value={position}
            onChange={handleSeek}
            className="w-full"
          />
          <span className="text-xs w-10">{formatTime(duration)}</span>
        </div>
      </div>
      <div className="w-20 md:block hidden" /> {/* 오른쪽 패딩용 */}
    </div>
  );
}
