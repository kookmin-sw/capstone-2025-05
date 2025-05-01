import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';
import Header from '../../Components/MapleHeader';
import Logo from '../../Assets/logo.svg';
import Input from '../../Components/Input/input.js';
import Dropdown from '../../Components/Dropdown/dropdown.js';
import Button from '../../Components/Button/Button.js';
import Footer from '../../Components/MapleFooter';

export default function Profile() {
  const navigate = useNavigate();
  const [nickname, setNickname] = useState('');
  const [skillLevel, setSkillLevel] = useState('');
  const [genre, setGenre] = useState('');

  const handleNickname = (e) => {
    setNickname(e.target.value);
  };

  const handleSkillChange = (e) => {
    setSkillLevel(Number(e.target.value));
  };

  const handleGenreChange = (e) => {
    setGenre(Number(e.target.value));
  };

  const invalidNickname = nickname.length === 0 || nickname.length > 10;
  const invalidSkill = skillLevel === '';
  const invalidGenre = genre === '';

  const handleComplete = async () => {
    if (invalidNickname || invalidSkill || invalidGenre) return;

    const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;
    const uid = localStorage.getItem('uid');

    try {
      const res = await axios.post(`${BACKEND_URL}/set-user-info`, {
        uid: uid,
        nickname: nickname,
        interest_genre: [genre],
        level: skillLevel,
      });

      if (res.data?.message === '유저 정보 입력 완료') {
        console.log('프로필 저장 성공');
        navigate('/main');
      } else {
        alert('프로필 저장 실패');
      }
    } catch (error) {
      console.error('프로필 저장 중 에러:', error);
      if (error.response) {
        console.error('서버 응답 상태:', error.response.status);
        console.error('서버 응답 데이터:', error.response.data);
      }
      if (error.response && error.response.status === 422) {
        alert('입력값이 유효하지 않습니다.');
      } else {
        alert('프로필 저장 중 오류가 발생했습니다.');
      }
    }
  };

  return (
    <div className="flex flex-col h-screen">
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
            placeholder="닉네임(10글자 이내)을 입력해 주세요"
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
