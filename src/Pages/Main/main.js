import React, { useRef, useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { Swiper, SwiperSlide } from 'swiper/react';
import { Navigation, Pagination, Autoplay } from 'swiper/modules';
import First from '../../Assets/Main/banner1.svg';
import Second from '../../Assets/Main/banner2.svg';
import Third from '../../Assets/Main/banner3.svg';
import Left from '../../Assets/Main/arrowLeft.svg';
import Right from '../../Assets/Main/arrowRight.svg';
import Footer from '../../Components/MapleFooter';
import 'swiper/css';
import { useNewReleases } from '../../Hooks/Main/getNewReleases';
import { useKoreaTopTracks } from '../../Hooks/Main/getTopPlaylists';
import Playbox from '../../Components/Playbox/Playbox';
import { useAllSongQuery } from '../../Hooks/Audio/get/getAllSongs';
import AudioPlaybox from '../../Components/Playbox/AudioPlaybox';
import analysisIcon from '../../Assets/Main/분석.svg';
import updateIcon from '../../Assets/Main/updateIcon.svg';
import popularIcon from '../../Assets/Main/음반.svg';

export default function Main() {
  const navigate = useNavigate();
  const swiperRef = useRef(null);
  const { data: TrendMusic } = useNewReleases();
  const { data: topMusic, isLoading } = useKoreaTopTracks();
  const { data: allSongs } = useAllSongQuery();
  const [playerTarget, setPlayerTarget] = useState();
  const cover_url = process.env.REACT_APP_COVER_URL;

  console.log(allSongs, '모든 곡');
  console.log('client_id:', process.env.REACT_APP_SPOTIFY_CLIENT_ID);
  console.log('client_secret:', process.env.REACT_APP_SPOTIFY_CLIENT_SECRET);

  return (
    <div className="flex flex-col min-h-screen">
      <div className="flex items-center justify-center mt-12">
        <Swiper
          ref={swiperRef}
          modules={[Navigation, Pagination, Autoplay]}
          spaceBetween={50}
          slidesPerView={1}
          navigation
          pagination={{ clickable: true }}
          autoplay={{ delay: 3000, disableOnInteraction: false }}
          className="w-[1200px] h-[360px] rounded-[10px]"
        >
          {[
            {
              image: First,
              text: '원곡과 연주의 차이를 확인해 보세요',
              position: 'bottom-4 left-4',
            },
            {
              image: Second,
              text: '당신의 연주를 눈으로 확인하고 실력을 향상하세요',
              position: 'bottom-4 right-4',
            },
            {
              image: Third,
              text: '아름다운 기타 연주',
              position: 'top-4 right-4',
            },
          ].map((banner, index) => (
            <SwiperSlide key={index}>
              <div
                className="w-full h-full bg-cover bg-center rounded-[10px] flex items-center justify-center relative"
                style={{ backgroundImage: `url(${banner.image})` }}
              >
                <span
                  className={`absolute ${banner.position} text-white text-[24px] font-bold`}
                >
                  {banner.text}
                </span>
              </div>
            </SwiperSlide>
          ))}
        </Swiper>

        <img
          src={Left}
          alt="Previous"
          className="absolute left-[calc(50%-700px)] w-8 h-8 cursor-pointer z-10"
          onClick={() => swiperRef.current?.swiper.slidePrev()}
        />
        <img
          src={Right}
          alt="Next"
          className="absolute right-[calc(50%-700px)] w-8 h-8 cursor-pointer z-10"
          onClick={() => swiperRef.current?.swiper.slideNext()}
        />
      </div>

      {/* 분석 가능 곡 섹션 */}
      {allSongs && (
        <div className="w-full mt-12">
          <button
            className="flex items-center cursor-pointer ml-[calc(50%-700px)] transition duration-300 ease-in-out transform hover:translate-x-[18px] hover:scale-[105%]"
            onClick={() =>
              navigate('/musics', {
                state: { musics: allSongs, musicType: 'analysis' },
              })
            }
          >
            <img src={analysisIcon} alt="분석아이콘" className="w-8" />
            <span className="text-xl font-bold text-[24px] ml-1 mr-4">
              분석 가능 곡들
            </span>
            <img src={Right} alt="arrow" className="cursor-pointer" />
          </button>
          <div className="flex flex-wrap justify-center gap-12 mt-4">
            {Array.isArray(allSongs) &&
              allSongs
                .slice(0, 4)
                .map((album, index) => (
                  <AudioPlaybox
                    img={cover_url + '/' + album.thumbnail}
                    title={album.title}
                    artist={album.artist}
                    playurl={album.audio}
                    song_id={album.song_id}
                  />
                ))}
          </div>
        </div>
      )}

      <div className="w-full mt-12">
        <button
          className="flex items-center cursor-pointer transition duration-300 ease-in-out transform hover:translate-x-[18px] hover:scale-[105%] ml-[calc(50%-700px)]"
          onClick={() =>
            navigate('/musics', {
              state: { musics: topMusic, musicType: 'top' },
            })
          }
        >
          <img src={popularIcon} alt="분석아이콘" className="w-8" />
          <span className="text-xl font-bold text-[24px] ml-1 mr-8">
            인기곡 추천
          </span>
          <img src={Right} alt="arrow" className="cursor-pointer" />
        </button>
        <div className="flex flex-wrap justify-center gap-12 mt-4">
          {Array.isArray(topMusic) &&
            topMusic
              .slice(0, 4)
              .map((album, index) => (
                <Playbox
                  img={album.cover}
                  title={album.title}
                  artist={album.artist}
                  playurl={album.uri}
                  playerTarget={playerTarget}
                  setPlayerTarget={setPlayerTarget}
                  isSelected={
                    playerTarget && album.title == playerTarget.title
                      ? true
                      : false
                  }
                />
              ))}
        </div>
      </div>

      <div className="w-full mt-16 mb-16">
        <button
          className="flex items-center cursor-pointer transition duration-300 ease-in-out ml-[calc(50%-700px)] transform hover:translate-x-[15px] hover:scale-[105%]"
          onClick={() =>
            navigate('/musics', {
              state: { musics: TrendMusic, musicType: 'trend' },
            })
          }
        >
          <img src={updateIcon} alt="분석아이콘" className="h-10" />
          <span className="text-xl font-bold text-[24px] ml-1 mr-4">
            최근 등록된 곡
          </span>
          <img src={Right} alt="arrow" />
        </button>
        <div className="flex flex-wrap justify-center gap-12 mt-4">
          {Array.isArray(TrendMusic) &&
            TrendMusic.slice(0, 4).map((album, index) => (
              <Playbox
                img={album.cover}
                title={album.title}
                artist={album.artist}
                playurl={album.uri}
                playerTarget={playerTarget}
                setPlayerTarget={setPlayerTarget}
                isSelected={
                  playerTarget && album.title == playerTarget.title
                    ? true
                    : false
                }
              />
            ))}
        </div>
      </div>
      <Footer email={'maple@gmail.com'} />
    </div>
  );
}
