import React, { useState } from 'react';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
import Input from '../../Components/Input/input.js';
import Button from '../../Components/Button/Button.js';
import Footer from '../../Components/MapleFooter';

export default function Profile() {
  const [nickname, setNickname] = useState('');

  const handleNickname = (e) => {
    setNickname(e.target.value);
  };

  const invalidNickname = nickname.length === 0 || nickname.length > 10;

  const handleComplete = () => {
    if (!invalidNickname) {
      console.log(nickname);
      console.log('complete!');
    }
  };

  return (
    <div className="flex flex-col h-screen">
      <Header />
      <div className="flex flex-col items-center flex-grow relative">
        <div className="absolute top-[20%]">
          <img src={Logo} alt="logo" className="w-60 h-auto" />
        </div>

        <span className="absolute top-[40%] text-[20px] font-bold">
          설정할 닉네임을 입력해 주세요
        </span>

        <div className="absolute top-[52%]">
          <Input
            width="400px"
            height="52px"
            type="text"
            placeholder="닉네임을 입력해 주세요"
            value={nickname}
            onChange={handleNickname}
          />
        </div>

        <div className="absolute top-[76%] text-center">
          <Button
            width="100px"
            height="40px"
            onClick={handleComplete}
            disabled={invalidNickname}
            style={{
              backgroundColor: invalidNickname ? '#AFAFAF' : '',
              cursor: invalidNickname ? 'default' : 'pointer',
            }}
          >
            완료
          </Button>
        </div>
      </div>
      <Footer />
    </div>
  );
}
