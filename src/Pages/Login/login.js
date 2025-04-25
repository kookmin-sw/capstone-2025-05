import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';
import Logo from '../../Assets/logo.svg';
import Google from '../../Assets/google.svg';
import Spotify from '../../Assets/spotify.svg';
import Box from '../../Components/Box/Box.js';
import Input from '../../Components/Input/input.js';
import Button from '../../Components/Button/Button.js';
import Footer from '../../Components/MapleFooter';

export default function Login() {
  const navigate = useNavigate();
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');

  const handleEmail = (e) => setEmail(e.target.value);
  const handlePassword = (e) => setPassword(e.target.value);

  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;
  const clientId = process.env.REACT_APP_SPOTIFY_CLIENT_ID;
  const redirectUri = process.env.REACT_APP_SPOTIFY_REDIRECT_URI;
  const scopes = [
    'streaming',
    'user-read-email',
    'user-read-private',
    'user-read-playback-state',
    'user-modify-playback-state',
  ];
  const authUrl =
    'https://accounts.spotify.com/authorize' +
    '?response_type=token' +
    `&client_id=${clientId}` +
    `&scope=${encodeURIComponent(scopes.join(' '))}` +
    `&redirect_uri=${encodeURIComponent(redirectUri)}`;

  const handleLogin = async () => {
    const trimmedEmail = email.trim();
    const trimmedPassword = password.trim();

    if (!trimmedEmail || !trimmedPassword) {
      alert('이메일과 비밀번호를 모두 입력해주세요.');
      return;
    }

    try {
      const res = await axios.post(`${BACKEND_URL}/email-login`, {
        email: trimmedEmail,
        password: trimmedPassword,
      });

      const uid = res.data.uid;

      if (uid) {
        localStorage.setItem('uid', uid);
        console.log('로그인 성공');
        navigate('/main');
      } else {
        alert('로그인 실패: 사용자 정보를 가져올 수 없습니다.');
      }
    } catch (error) {
      console.error('로그인 에러:', error);
      if (error.response) {
        console.error('서버 응답 상태:', error.response.status);
        console.error('서버 응답 데이터:', error.response.data);
        if (error.response.status === 422) {
          alert('입력값이 유효하지 않습니다.');
        } else {
          alert('서버 오류가 발생했습니다.');
        }
      } else {
        alert('네트워크 오류가 발생했습니다.');
      }
    }
  };

  const CLIENT_ID = process.env.REACT_APP_CLIENT_ID;
  const REDIRECT_URI = process.env.REACT_APP_REDIRECT_URI;
  const SCOPE = process.env.REACT_APP_SCOPE;
  const googleLink = `https://accounts.google.com/o/oauth2/auth?client_id=${CLIENT_ID}&redirect_uri=${REDIRECT_URI}&response_type=code&scope=${SCOPE}`;

  const handleGoogleLogin = () => {
    window.location.href = googleLink;
  };

  const handleSpoityLogin = () => {
    window.location.href = authUrl;
  };

  return (
    <div className="flex flex-col min-h-screen">
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
                  placeholder="이메일을 입력해 주세요"
                  value={email}
                  onChange={handleEmail}
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
