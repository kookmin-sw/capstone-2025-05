import React, { useRef } from 'react';
import { useNavigate } from 'react-router-dom';
import { Swiper, SwiperSlide } from 'swiper/react';
import { Navigation, Pagination, Autoplay } from 'swiper/modules';
import Header from '../../Components/MapleHeader';
import First from '../../Assets/Main/banner1.svg';
import Second from '../../Assets/Main/banner2.svg';
import Third from '../../Assets/Main/banner3.svg';
import Left from '../../Assets/Main/arrowLeft.svg';
import Right from '../../Assets/Main/arrowRight.svg';
import Box from '../../Components/Box/Box';
import Album1 from '../../Assets/Main/album/iveCover.svg';
import Album2 from '../../Assets/Main/album/bndCover.svg';
import Album3 from '../../Assets/Main/album/riizeCover.svg';
import Album4 from '../../Assets/Main/album/gdCover.svg';
import Footer from '../../Components/MapleFooter';
import 'swiper/css';

export default function Main() {
  const navigate = useNavigate();
  const swiperRef = useRef(null);

  const handleMove = () => {
    navigate('/'); // 등록된 음원 페이지로 이동
  };

  const albums = [
    { cover: Album1, title: 'REBEL HEART', artist: 'IVE (아이브)' },
    { cover: Album2, title: '오늘만 I LOVE YOU', artist: 'BOYNEXTDOOR' },
    { cover: Album3, title: 'COMBO', artist: 'RIIZE' },
    { cover: Album4, title: 'HOME SWEET HOME', artist: 'G-DRAGON' },
  ];

  return (
    <div className="flex flex-col h-screen">
      <Header />
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
          {albums.map((album, index) => (
            <Box key={index} width="248px" height="304px">
              <div className="flex justify-center items-center mt-4">
                <img
                  src={album.cover}
                  alt="Album Cover"
                  className="w-[220px] h-[220px] object-cover"
                />
              </div>
              <div className="flex items-center justify-between px-4 mt-2">
                <div className="flex flex-col w-[168px]">
                  <span className="text-lg font-semibold truncate">
                    {album.title}
                  </span>
                  <span className="text-lg mt-[-4px] truncate">
                    {album.artist}
                  </span>
                </div>
                <button className="w-10 h-10 rounded-full bg-[#A57865] flex items-center justify-center shadow-none hover:bg-white hover:shadow-xl transition-colors group">
                  <div className="w-0 h-0 border-l-[12px] border-l-white border-t-[8px] border-b-[8px] border-t-transparent border-b-transparent group-hover:border-l-[#A57865] transition-colors"></div>
                </button>
              </div>
            </Box>
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
          {albums.map((album, index) => (
            <Box key={index} width="248px" height="304px">
              <div className="flex justify-center items-center mt-4">
                <img
                  src={album.cover}
                  alt="Album Cover"
                  className="w-[220px] h-[220px] rounded-[10px] object-cover"
                />
              </div>
              <div className="flex items-center justify-between px-4 mt-2">
                <div className="flex flex-col w-[168px]">
                  <span className="text-lg font-semibold truncate">
                    {album.title}
                  </span>
                  <span className="text-lg mt-[-4px] truncate">
                    {album.artist}
                  </span>
                </div>
                <button className="w-10 h-10 rounded-full bg-[#A57865] flex items-center justify-center shadow-none hover:bg-white hover:shadow-xl transition-colors group">
                  <div className="w-0 h-0 border-l-[12px] border-l-white border-t-[8px] border-b-[8px] border-t-transparent border-b-transparent group-hover:border-l-[#A57865] transition-colors"></div>
                </button>
              </div>
            </Box>
          ))}
        </div>
      </div>
      <Footer />
    </div>
  );
}
