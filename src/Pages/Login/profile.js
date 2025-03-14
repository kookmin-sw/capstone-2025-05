import React, { useState } from 'react';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
import Input from '../../Components/Input/input.js';
import Dropdown from '../../Components/Dropdown/dropdown.js';
import Button from '../../Components/Button/Button.js';
import Footer from '../../Components/MapleFooter';

export default function Profile() {
  const [nickname, setNickname] = useState('');
  const [skillLevel, setSkillLevel] = useState('');
  const [genre, setGenre] = useState('');

  const handleNickname = (e) => {
    setNickname(e.target.value);
  };

  const handleSkillChange = (e) => {
    setSkillLevel(e.target.value);
  };

  const handleGenreChange = (e) => {
    setGenre(e.target.value);
  };

  const invalidNickname = nickname.length === 0 || nickname.length > 10;
  const invalidSkill = skillLevel === '';
  const invalidGenre = genre === '';

  const handleComplete = () => {
    if (!invalidNickname && !invalidSkill && !invalidGenre) {
      console.log(nickname);
      console.log(skillLevel);
      console.log(genre);
      console.log('complete!');
    }
  };

  return (
    <div className="flex flex-col h-screen">
      <Header />
      <div className="flex flex-col items-center flex-grow relative">
        <div className="absolute top-[12%]">
          <img src={Logo} alt="logo" className="w-60 h-auto" />
        </div>

        <span className="absolute top-[32%] text-[20px] font-bold">
          설정할 닉네임을 입력하고 실력과 선호 장르를 선택해 주세요
        </span>

        <div className="absolute top-[44%]">
          <Input
            width="400px"
            height="52px"
            type="text"
            placeholder="닉네임을 입력해 주세요"
            value={nickname}
            onChange={handleNickname}
          />
        </div>

        <div className="absolute top-[56%]">
          <Dropdown
            name="skillLevel"
            value={skillLevel}
            onChange={handleSkillChange}
            width="400px"
            height="52px"
          />
        </div>

        <div className="absolute top-[68%]">
          <Dropdown
            name="genre"
            value={genre}
            onChange={handleGenreChange}
            width="400px"
            height="52px"
          />
        </div>

        <div className="absolute top-[88%] text-center">
          <Button
            width="100px"
            height="40px"
            onClick={handleComplete}
            disabled={invalidNickname || invalidSkill || invalidGenre}
            style={{
              backgroundColor:
                invalidNickname || invalidSkill || invalidGenre
                  ? '#AFAFAF'
                  : '',
              cursor:
                invalidNickname || invalidSkill || invalidGenre
                  ? 'default'
                  : 'pointer',
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
