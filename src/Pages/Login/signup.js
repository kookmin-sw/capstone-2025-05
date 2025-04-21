import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
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

  const handleSignup = async () => {
    const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

    const trimmedEmail = email.trim();
    const trimmedPassword = password.trim();

    if (!trimmedEmail || !trimmedPassword) {
      alert('이메일과 비밀번호를 모두 입력해주세요.');
      return;
    }

    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!emailRegex.test(trimmedEmail)) {
      alert('유효한 이메일 주소를 입력해주세요.');
      return;
    }

    if (trimmedPassword.length < 6) {
      alert('비밀번호는 6자 이상이어야 합니다.');
      return;
    }

    try {
      const res = await axios.post(
        `${BACKEND_URL}/account/email-sign-up`,
        {
          email: trimmedEmail,
          password: trimmedPassword,
        },
        {
          headers: {
            'Content-Type': 'application/json',
          },
        },
      );

      const uid = res.data.data.uid;

      if (res.data.success) {
        localStorage.setItem('uid', uid);
        console.log('회원가입 성공');
        navigate('/profile');
      } else {
        alert('회원가입 실패');
      }
    } catch (error) {
      console.error('회원가입 에러:', error);
      if (error.response) {
        console.error('서버 응답 상태:', error.response.status);
        console.error('서버 응답 데이터:', error.response.data);
      }
      if (error.response && error.response.status === 422) {
        alert('입력값이 유효하지 않습니다.');
      } else {
        alert('회원가입 중 오류가 발생했습니다.');
      }
    }
  };

  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      <div className="flex flex-col items-center justify-center flex-grow relative">
        <div className="absolute top-[10%] z-10">
          <img src={Logo} alt="logo" className="w-52 h-auto" />
        </div>

        <div className="absolute top-[18%]">
          <Box width="400px" height="432px">
            <div className="flex flex-col items-center justify-center h-full">
              <span className="text-center text-lg font-semibold mb-8">
                사용할 이메일과 비밀번호를 입력해 주세요
              </span>
              <div className="flex flex-col items-center gap-y-6">
                <Input
                  width="320px"
                  height="52px"
                  type="email"
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
              <div className="mt-16">
                <Button width="320px" height="52px" onClick={handleSignup}>
                  회원가입
                </Button>
              </div>
            </div>
          </Box>
        </div>
      </div>

      <Footer className="absolute bottom-0 w-full" />
    </div>
  );
}
