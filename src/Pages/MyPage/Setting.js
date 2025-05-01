import React, { useEffect, useState } from 'react';
import axios from 'axios';
import { Link, useNavigate } from 'react-router-dom';
import Dropdown from '../../Components/Dropdown/dropdown.js';
import Input from '../../Components/Input/input.js';
import Music from '../../Assets/MyPage/Vector.svg';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';
import Profile from '../../Assets/MyPage/profile.svg';

export default function Admin() {
  const [nickname, setNickname] = useState('');
  const [email, setEmail] = useState('');
  const [skillLevel, setSkillLevel] = useState('');
  const [genre, setGenre] = useState('');
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [errors, setErrors] = useState({ nickname: '', email: '' });
  const [profilePic, setProfilePic] = useState(Profile);
  const [isAccountDeleted, setIsAccountDeleted] = useState(false);
  const navigate = useNavigate();
  const uid = localStorage.getItem("uid") || "cLZMFP4802a7dwMo0j4qmcxpnY63";
  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

  useEffect(() => {
    const fetchUserInfo = async () => {
      try {
        const response = await axios.get(`${BACKEND_URL}/get-user-info`, {
          params: { uid }
        });
    
        const userInfo = response.data; // 수정한 부분
    
        if (!userInfo) {
          console.error('No user information found:', response.data);
          return;
        }
    
        setNickname(userInfo.nickname || '');
        setEmail(userInfo.email || '');
        setSkillLevel(userInfo.level || '');
        setGenre(userInfo.interest_genre?.[0] || '');
        setProfilePic(userInfo.profile_image || Profile);
        console.log('Fetched user info:', userInfo);
    
      } catch (error) {
        console.error('Error fetching user info:', error.response || error);
      }
    };
    fetchUserInfo();
  }, [BACKEND_URL, uid]);

  const handleNicknameChange = (e) => setNickname(e.target.value);

  const handleSaveNickname = async () => {
    const trimmedNickname = nickname.trim();

    if (!trimmedNickname) {
      setErrors(prev => ({ ...prev, nickname: '*닉네임을 입력해주세요' }));
      return;
    }

    try {
      const res = await axios.put(`${BACKEND_URL}/edit-user/nickname`, null, {
        params: { uid, nickname: trimmedNickname }
      });
      console.log('Nickname updated:', res.data);
      setIsModalOpen(true);
      window.location.reload();    
    } catch (error) {
      console.error('Error updating nickname:', error.response || error);
    }
  };

  const handleGenreChange = async (e) => {
    const genreValue = e.target.value;
    try {
      const res = await axios.put(`${BACKEND_URL}/edit-user/interest-genre`, [genreValue], {
        params: { uid }
      });
      console.log('Genre updated:', res.data);
      setGenre(genreValue); // ★ 추가: state 업데이트
    } catch (error) {
      console.error('Error updating genre:', error.response || error);
    }
  };
  
  const handleSkillChange = async (e) => {
    const skillLevelValue = Number(e.target.value);
    try {
      const res = await axios.put(`${BACKEND_URL}/edit-user/level`, null, {
        params: { uid, level: skillLevelValue }
      });
      console.log('Skill level updated:', res.data);
      setSkillLevel(skillLevelValue); // ★ 추가
    } catch (error) {
      console.error('Error updating skill level:', error.response || error);
    }
  };
  

  const handleProfilePicChange = async (e) => {
    const file = e.target.files[0];
    if (file) {
      const formData = new FormData();
      formData.append("file", file);
      formData.append("uid", uid);
  
      // 파일 크기 및 형식 체크
      if (!file.type.startsWith("image/")) {
        alert("이미지 파일만 업로드할 수 있습니다.");
        return;
      }
      if (file.size > 5000000) { // 예: 5MB 제한
        alert("파일 크기가 너무 큽니다. 5MB 이하로 업로드해 주세요.");
        return;
      }
  
      try {
        const response = await axios.post(`${BACKEND_URL}/change-profile-image`, formData, {
          headers: {
            "Content-Type": "multipart/form-data"
          }
        });
        setProfilePic(URL.createObjectURL(file)); // 미리보기
        console.log("Uploaded Image URL:", URL.createObjectURL(file));
      } catch (error) {
        console.error("Error uploading profile picture:", error.response || error);
        if (error.response && error.response.data) {
          alert(`Error: ${error.response.data.message || '이미지 업로드에 실패했습니다.'}`);
        } else {
          alert("알 수 없는 오류가 발생했습니다.");
        }
      }
    }
  };

  const handleDeleteAccount = async () => {
    try {
      const res = await axios.delete(`${BACKEND_URL}/delete-user/${uid}`);
      if (res.data.success) {
        setIsAccountDeleted(true);
        setTimeout(() => {
          localStorage.removeItem("uid");
          navigate('/login');
        }, 2000);
      }
    } catch (error) {
      console.error('Error deleting account:', error.response || error);
      alert("탈퇴하는 데 실패했습니다.");
    }
  };

  return (
    <div className="flex flex-col min-h-screen">
      <div className="flex flex-1">
        {/* Sidebar */}
        <div className="w-[12%] bg-[#463936] text-white p-4 flex flex-col justify-between">
          <div>
            <h2 className="text-md font-bold">MAPLE</h2>
            <ul className="mt-4 space-y-2">
              <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                <img src={Information} alt="내 정보 아이콘" className="w-4 h-4" />
                <Link to="/mypage" className="text-white">내 정보</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                <Link to="/playedmusic">연주한 곡</Link>
              </li>
              <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                <Link to="/setting">관리</Link>
              </li>
            </ul>
          </div>
          <div>
             <p className="font-semibold">{nickname || '사용자'}</p>
          </div>
        </div>

        {/* Main Content */}
        <div className="flex-1 flex items-center justify-center bg-[#F5F1EC] py-10">
          <div className="bg-white p-14 rounded-2xl shadow-lg w-full max-w-4xl">
            <h2 className="text-3xl font-bold mb-10 text-center">내 프로필</h2>

            <div className="flex justify-center mb-10">
              <img
                src={profilePic}
                alt="프로필"
                className="w-40 h-40 rounded-full cursor-pointer"
                onClick={() => document.getElementById('profilePicInput').click()}
              />
              <input
                type="file"
                id="profilePicInput"
                accept="image/*"
                className="hidden"
                onChange={handleProfilePicChange}
              />
            </div>

            <div className="space-y-8">
              <div>
                <label className="text-gray-700 font-semibold mb-2 block text-lg">닉네임</label>
                <Input
                  type="text"
                  value={nickname}
                  onChange={handleNicknameChange}
                  width="100%"
                  placeholder="닉네임"
                  className="h-14 text-lg"
                />
                {errors.nickname && <p className="text-red-500 text-sm mt-1">{errors.nickname}</p>}
              </div>

              <div>
                <label className="text-gray-700 font-semibold mb-2 block text-lg">이메일</label>
                <Input
                  type="text"
                  value={email}
                  onChange={(e) => setEmail(e.target.value)}
                  width="100%"
                  placeholder="이메일"
                  className="h-14 text-lg"
                />
                {errors.email && <p className="text-red-500 text-sm mt-1">{errors.email}</p>}
              </div>

              <div>
                <label className="text-gray-700 font-semibold mb-2 block text-lg">스킬 레벨</label>
                <Dropdown
                  name="skillLevel"
                  value={skillLevel}
                  onChange={handleSkillChange}
                  width="100%"
                  className="h-14 text-lg"
                />
              </div>

              <div>
                <label className="text-gray-700 font-semibold mb-2 block text-lg">선호 장르</label>
                <Dropdown
                  name="genre"
                  value={genre}
                  onChange={handleGenreChange}
                  width="100%"
                  className="h-14 text-lg"
                />
              </div>
            </div>

            <div className="flex gap-6 mt-12">
              <button
                className="flex-1 bg-[#A57865] text-white rounded-lg py-4 text-lg hover:bg-opacity-80"
                onClick={handleSaveNickname}
              >
                수정하기
              </button>
              <button
                className="flex-1 bg-red-500 text-white rounded-lg py-4 text-lg hover:bg-opacity-80"
                onClick={handleDeleteAccount}
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

      {isAccountDeleted && (
        <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50">
          <div className="bg-white p-6 rounded-lg shadow-lg text-center">
            <p className="text-lg font-bold text-red-500">계정이 성공적으로 삭제되었습니다.</p>
          </div>
        </div>
      )}
    </div>
  );
}
