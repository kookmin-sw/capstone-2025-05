import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
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
    navigate('/profile');
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
                사용할 아이디와 비밀번호를 입력해 주세요
              </span>
              <div className="flex flex-col items-center gap-y-6">
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
              <div className="mt-16">
                <Button width="320px" height="52px" onClick={handleLogin}>
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
