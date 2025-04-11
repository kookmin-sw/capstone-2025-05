import React, { useState } from 'react';
import { Link, useNavigate } from 'react-router-dom';
import Dropdown from '../../Components/Dropdown/dropdown.js';
import Input from '../../Components/Input/input.js';
import Header from '../../Components/MapleHeader.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Profile from '../../Assets/MyPage/profile.svg';

export default function Admin() {
  const [nickname, setNickname] = useState('홍길동');
  const [email, setEmail] = useState('gildong@gmail.com');
  const [skillLevel, setSkillLevel] = useState('전문가');
  const [genre, setGenre] = useState('Pop');
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [isDeleteModalOpen, setIsDeleteModalOpen] = useState(false);
  const [password, setPassword] = useState('');
  const [errors, setErrors] = useState({
    nickname: '',
    email: '',
    password: '',
  });

  const [profilePic, setProfilePic] = useState(Profile); // 기본 프로필 사진
  const navigate = useNavigate();

  const handleSave = () => {
    let isValid = true;
    const newErrors = { nickname: '', email: '' };

    if (!nickname.trim()) {
      newErrors.nickname = '*닉네임을 입력해주세요';
      isValid = false;
    }

    if (!email.trim()) {
      newErrors.email = '*이메일을 입력해주세요';
      isValid = false;
    }

    setErrors(newErrors);

    if (isValid) {
      setIsModalOpen(true);
    }
  };

  const handleDeleteAccount = () => {
    const correctPassword = '1234';

    if (password.trim() === '') {
      setErrors({ ...errors, password: '*비밀번호를 입력해주세요' });
      return;
    }

    if (password !== correctPassword) {
      setErrors({ ...errors, password: '*비밀번호가 틀렸습니다' });
      return;
    }

    setIsDeleteModalOpen(false);
    alert('계정이 삭제되었습니다');
    navigate('/main');
  };

  const handleProfilePicChange = (e) => {
    const file = e.target.files[0];
    if (file) {
      const reader = new FileReader();
      reader.onloadend = () => {
        setProfilePic(reader.result); // 새로운 프로필 사진으로 업데이트
      };
      reader.readAsDataURL(file);
    }
  };

  return (
    <div className="flex flex-col">
      <Header />
      <div className="flex">
        <div className="w-[180px] h-[770px] bg-[#463936] text-white p-4 flex flex-col justify-between">
          <div>
            <h2 className="text-md font-bold">MAPLE</h2>
            <ul className="mt-4 space-y-2">
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Information} alt="내 정보 아이콘" className="w-4 h-4" />
                <Link to="/mypage" className="text-white">내 정보</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                <Link to="/playedmusic">연주한 곡</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting">관리</Link>
              </li>
            </ul>
          </div>
          <div>
            <p className="font-semibold">Kildong Hong</p>
          </div>
        </div>

        <div className="flex-1 flex items-center justify-center bg-[#F5F1EC]">
          <div className="bg-white p-8 rounded-lg shadow-lg w-full max-w-md">
            <h2 className="text-xl font-bold mb-6 text-center">내 프로필</h2>

            {/* 프로필 사진 클릭시 변경 */}
            <div className="flex justify-center mb-4">
              <img
                src={profilePic}
                alt="프로필"
                className="w-32 h-32 rounded-full cursor-pointer"
                onClick={() => document.getElementById('profilePicInput').click()} // 클릭 시 파일 선택 열기
              />
              <input
                type="file"
                id="profilePicInput"
                accept="image/*"
                className="hidden"
                onChange={handleProfilePicChange}
              />
            </div>

            <div className="flex flex-col">
              <div className="flex flex-col">
                <label className="text-gray-700 font-semibold mb-1">닉네임</label>
                <Input
                  type="text"
                  value={nickname}
                  onChange={(e) => setNickname(e.target.value)}
                  width="100%"
                  placeholder="닉네임"
                />
                {errors.nickname && <p className="text-red-500 text-sm mt-1">{errors.nickname}</p>}
              </div>

              <div className="flex flex-col">
                <label className="text-gray-700 font-semibold mb-1 mt-3">이메일</label>
                <Input
                  type="text"
                  value={email}
                  onChange={(e) => setEmail(e.target.value)}
                  width="100%"
                  placeholder="이메일"
                />
                {errors.email && <p className="text-red-500 text-sm mt-1">{errors.email}</p>}
              </div>

              <div className="flex flex-col">
                <label className="text-gray-700 font-semibold mb-1 mt-3">스킬 레벨</label>
                <Dropdown
                  name="skillLevel"
                  value={skillLevel}
                  onChange={(e) => setSkillLevel(e.target.value)}
                  width="100%"
                />
              </div>

              <div className="flex flex-col">
                <label className="text-gray-700 font-semibold mb-1 mt-3">선호 장르</label>
                <Dropdown
                  name="genre"
                  value={genre}
                  onChange={(e) => setGenre(e.target.value)}
                  width="100%"
                />
              </div>

              <button
                className="bg-[#A57865] text-white rounded-lg py-2 mt-10 hover:bg-opacity-80"
                onClick={handleSave}
              >
                수정하기
              </button>

              <button
                className="text-red-500 text-sm mt-4 hover:underline"
                onClick={() => setIsDeleteModalOpen(true)}
              >
                탈퇴하기
              </button>
            </div>
          </div>
        </div>
      </div>

      {isModalOpen && (
        <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50">
          <div className="bg-white p-6 rounded-lg shadow-lg text-center">
            <p className="text-lg font-bold">수정이 완료되었습니다</p>
            <button
              className="mt-4 bg-[#A57865] text-white px-4 py-2 rounded-lg hover:bg-opacity-80"
              onClick={() => setIsModalOpen(false)}
            >
              닫기
            </button>
          </div>
        </div>
      )}

      {isDeleteModalOpen && (
        <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50">
          <div className="bg-white p-6 rounded-lg shadow-lg text-center">
            <p className="text-lg font-bold mb-4">계정을 삭제하려면 비밀번호를 입력하세요</p>
            <Input
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              placeholder="비밀번호"
              width="100%"
            />
            {errors.password && <p className="text-red-500 text-sm mt-1">{errors.password}</p>}
            <div className="flex justify-around mt-4">
              <button
                className="bg-[#A57865] text-white px-4 py-2 rounded-lg hover:bg-opacity-80"
                onClick={handleDeleteAccount}
              >
                삭제하기
              </button>
              <button
                className="bg-gray-300 text-gray-800 px-4 py-2 rounded-lg hover:bg-opacity-80"
                onClick={() => setIsDeleteModalOpen(false)}
              >
                취소
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
