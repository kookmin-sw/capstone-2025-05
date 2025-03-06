import React from 'react';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
import Input from '../../Components/Input/input.js';
import Button from '../../Components/Button/Button.js';
import Footer from '../../Components/MapleFooter';

const handleComplete = () => {
  console.log('complete!');
};

export default function Profile() {
  return (
    <div className="flex flex-col min-h-screen">
      <Header />
      <div className="flex flex-col items-center flex-grow relative">
        <div className="absolute top-[20%]">
          <img src={Logo} alt="logo" className="w-60 h-auto" />
        </div>

        <span className="absolute top-[40%] text-[18px] font-bold">
          설정할 닉네임을 입력해 주세요
        </span>

        <div className="absolute top-[52%]">
          <Input
            width="400px"
            height="40px"
            type="text"
            placeholder="닉네임을 입력해 주세요"
          />
        </div>

        <div className="absolute top-[68%] text-center">
          <Button width="100px" height="40px" onClick={handleComplete}>
            완료
          </Button>
        </div>
      </div>
      <Footer />
    </div>
  );
}
