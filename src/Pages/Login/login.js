import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
import Google from '../../Assets/google.svg';
import Spotify from '../../Assets/spotify.svg';
import Box from '../../Components/Box/Box.js';
import Input from '../../Components/Input/input.js';
import Button from '../../Components/Button/Button.js';
import Footer from '../../Components/MapleFooter';

export default function Login() {
  const navigate = useNavigate();
  const [id, setId] = useState('');
  const [password, setPassword] = useState('');

  const handleId = (e) => setId(e.target.value);
  const handlePassword = (e) => setPassword(e.target.value);

  const handleLogin = () => {
    navigate('/main');
  };

  const CLIENT_ID = process.env.REACT_APP_CLIENT_ID;
  const REDIRECT_URI = process.env.REACT_APP_REDIRECT_URI;
  const SCOPE = process.env.REACT_APP_SCOPE;
  const googleLink = `https://accounts.google.com/o/oauth2/auth?client_id=${CLIENT_ID}&redirect_uri=${REDIRECT_URI}&response_type=code&scope=${SCOPE}`;

  const handleGoogleLogin = () => {
    window.location.href = googleLink;
  };

  const handleSpoityLogin = () => {
    navigate('/main');
  };

  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      <div className="flex flex-col items-center justify-center flex-grow relative">
        <div className="absolute top-[4%] z-10">
          <img src={Logo} alt="logo" className="w-52 h-auto" />
        </div>

        <div className="absolute top-[12%]">
          <Box width="400px" height="300px">
            <div className="flex flex-col items-center justify-center h-full">
              <div className="flex flex-col items-center gap-y-4">
                <Input
                  width="320px"
                  height="52px"
                  type="text"
                  placeholder="아이디를 입력해 주세요"
                  value={id}
                  onChange={handleId}
                />
                <Input
                  width="320px"
                  height="52px"
                  type="password"
                  placeholder="비밀번호를 입력해 주세요"
                  value={password}
                  onChange={handlePassword}
                />
              </div>
              <div className="mt-8">
                <Button width="320px" height="52px" onClick={handleLogin}>
                  로그인
                </Button>
              </div>
            </div>
          </Box>
        </div>

        <div className="absolute top-[67%] w-[600px] border-t-2 border-[#AFAFAF]"></div>

        <div className="absolute top-[69%]">
          <button
            className="flex flex-row justify-between items-center w-[400px] h-[60px] pl-5 pr-5 rounded-[10px] text-black text-[20px] font-bold bg-[white]"
            onClick={handleGoogleLogin}
          >
            <img src={Google} alt="google logo" />
            구글로 시작하기
            <div className="w-[24px] h-[100%]"></div>
          </button>
        </div>

        <div className="absolute top-[82%]">
          <button
            className="flex flex-row justify-between items-center w-[400px] h-[60px] pl-5 pr-5 rounded-[10px] text-white text-[20px] font-bold bg-[#12D760]"
            onClick={handleSpoityLogin}
          >
            <img src={Spotify} alt="spotify logo" className="w-12 h-12" />
            스포티파이로 시작하기
            <div className="w-[24px] h-[100%]"></div>
          </button>
        </div>
      </div>
      <div className="flex flex-col items-center">
        <div className="absolute top-[89%]">
          <button
            className="text-[#AFAFAF] text-sm underline hover:text-gray-500"
            onClick={() => {
              navigate('/signup');
            }}
          >
            회원가입
          </button>
        </div>
      </div>
      <Footer className="absolute bottom-0 w-full" />
    </div>
  );
}
