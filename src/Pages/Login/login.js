import React from 'react';
import { useNavigate } from 'react-router-dom';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
import Google from '../../Assets/google.svg';
import Footer from '../../Components/MapleFooter';

export default function Login() {
  const navigate = useNavigate();

  const handleLogin = () => {
    navigate('/profile');
  };

  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      <div className="flex flex-col items-center flex-grow relative">
        <div className="absolute top-[20%]">
          <img src={Logo} alt="logo" className="w-60 h-auto" />
        </div>

        <div className="absolute top-[42%]">
          <button
            className="flex flex-row justify-between items-center w-[360px] h-[60px] pl-5 pr-5 rounded-[10px] text-black text-[16px] font-bold bg-[white]"
            onClick={handleLogin}
          >
            <img src={Google} alt="google logo" />
            구글로 시작하기
            <div className="w-[22px] h-[100%]"></div>
          </button>
        </div>

        <div className="absolute top-[60%] w-[800px] border-t-2 border-[#AFAFAF]"></div>

        <div className="absolute top-[68%] text-center">
          <span className="text-[#AFAFAF] text-sm text-[24px]">
            이메일 인증을 위해 구글 계정으로만 로그인 가능합니다.
          </span>
        </div>
      </div>
      <Footer className="absolute bottom-0 w-full" />
    </div>
  );
}
