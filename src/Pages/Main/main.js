import React, { useRef, useEffect, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { Swiper, SwiperSlide } from 'swiper/react';
import { Navigation, Pagination, Autoplay } from 'swiper/modules';
import axios from 'axios';
import First from '../../Assets/Main/banner1.svg';
import Second from '../../Assets/Main/banner2.svg';
import Third from '../../Assets/Main/banner3.svg';
import Left from '../../Assets/Main/arrowLeft.svg';
import Right from '../../Assets/Main/arrowRight.svg';
import Box from '../../Components/Box/Box';
import Footer from '../../Components/MapleFooter';
import 'swiper/css';
import swal from 'sweetalert';
import { useNewReleases } from '../../Hooks/Main/getNewReleases';
import { useKoreaTopTracks } from '../../Hooks/Main/getTopPlaylists';
import Playbox from '../../Components/Playbox/Playbox';

export default function Main() {
  const navigate = useNavigate();
  const swiperRef = useRef(null);
  const [trend, setTrend] = useState([]);
  const { data: TrendMusic } = useNewReleases();
  const { data: topMusic, isLoading } = useKoreaTopTracks();
  const [playerTarget, setPlayerTarget] = useState();
  console.log('trendMusic', TrendMusic);

  const handleMove = () => {
    navigate('/ranking?song_name=Drowning'); // 랭킹 테스트 - 수정하기
  };
  console.log('topmusic', topMusic);

  return (
    <div className="flex flex-col h-screen">
      <div className="flex flex-grow items-center justify-center mt-12 relative">
        <Swiper
          ref={swiperRef}
          modules={[Navigation, Pagination, Autoplay]}
          spaceBetween={50}
          slidesPerView={1}
          navigation
          pagination={{ clickable: true }}
          autoplay={{ delay: 3000, disableOnInteraction: false }}
          className="w-[1100px] h-[340px] rounded-[10px] relative"
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
          className="absolute left-[36px] top-[50%] -translate-y-1/2 w-6 h-6 cursor-pointer z-10"
          onClick={() => swiperRef.current?.swiper.slidePrev()}
        />
        <img
          src={Right}
          alt="Next"
          className="absolute right-[36px] top-[50%] -translate-y-1/2 w-6 h-6 cursor-pointer z-10"
          onClick={() => swiperRef.current?.swiper.slideNext()}
        />
      </div>

      <div className="w-full mt-12">
        <span className="text-xl font-bold text-[24px] ml-16">인기곡 추천</span>
        <div className="flex flex-wrap justify-center gap-12 mt-4">
          {topMusic &&
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
        <span className="text-xl font-bold text-[24px] ml-16">
          최근 등록된 곡
        </span>
        <img
          src={Right}
          alt="arrow"
          className="absolute left-[220px] -mt-[22px] cursor-pointer"
          onClick={handleMove}
        />
        <div className="flex flex-wrap justify-center gap-12 mt-4">
          {TrendMusic &&
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
