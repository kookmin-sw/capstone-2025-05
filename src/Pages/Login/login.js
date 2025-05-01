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
import { useAuth } from '../../Context/AuthContext';
import swal from 'sweetalert';

export default function Login() {
  const navigate = useNavigate();
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  // const { login, logout } = useAuth();
  const { login, logout } = useAuth();

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
        // login(uid); //sessionstorage에 세팅 + context uid 세팅
        login(uid); //sessionstorage에 세팅 + context uid 세팅
        swal('', '로그인성공🫡', 'success');
        navigate('/main');
      } else {
        swal('', '로그인 실패: 사용자 정보를 가져올 수 없습니다😥', 'error');
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
      <div className="flex flex-col items-center justify-center h-full">
        <div className="flex z-10 mt-16">
          <img src={Logo} alt="logo" className="w-52 h-auto" />
        </div>

        <div className="flex justify-center w-full">
          <Box width="35%" height="50vh">
            <div className="flex flex-col items-center justify-center w-full h-full">
              <div className="flex flex-col items-center gap-y-4 w-full">
                <Input
                  width="80%"
                  height="52px"
                  type="text"
                  placeholder="이메일을 입력해 주세요"
                  value={email}
                  onChange={handleEmail}
                />
                <Input
                  width="80%"
                  height="52px"
                  type="password"
                  placeholder="비밀번호를 입력해 주세요"
                  value={password}
                  onChange={handlePassword}
                />
              </div>
              <div className="mt-8 w-full flex justify-center">
                <Button width="80%" height="52px" onClick={handleLogin}>
                  로그인
                </Button>
              </div>
            </div>
          </Box>
        </div>

        <div className="flex items-center w-[35%] mt-10 mb-8">
          <div className="flex-grow border-t-2 border-[#AFAFAF]"></div>
          <span className="px-4 text-gray-500 text-sm">또는</span>
          <div className="flex-grow border-t-2 border-[#AFAFAF]"></div>
        </div>

        <div className="flex flex-col items-center mb-2 w-full">
          <div className="flex justify-center w-full">
            <button
              className=" w-[35%] h-[60px] pl-5 pr-5 rounded-[10px] text-black text-[20px] font-bold bg-[white]"
              onClick={handleGoogleLogin}
            >
              <div className="flex justify-center items-center">
                <img src={Google} alt="google logo" />
                <span className="ml-2 text-center">구글로 시작하기</span>
              </div>
            </button>
          </div>
          <div className="flex flex-col items-center mt-2 w-full">
            <button
              className=" w-[35%] h-[60px] pl-5 pr-5 rounded-[10px] text-white text-[20px] font-bold bg-[#12D760]"
              onClick={handleSpoityLogin}
            >
              <div className="flex justify-center items-center">
                <img src={Spotify} alt="spotify logo" className="w-12 h-12" />
                <span className="ml-1 text-center">스포티파이 인증</span>
              </div>
            </button>
            <p className="mt-4 text-sm text-gray-500">
              * 스포티파이 인증을 완료해야 곡 스트리밍 서비스를 이용하실 수
              있습니다.
            </p>
          </div>
        </div>
      </div>

      <div className="flex flex-col items-center mt-2">
        <div className="flex">
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
