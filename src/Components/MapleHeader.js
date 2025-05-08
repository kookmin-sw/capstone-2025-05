import React, { useState } from 'react';
import Maple from '../Assets/Images/logo.js';
import { RiLogoutBoxRLine, RiLoginBoxLine } from 'react-icons/ri';
import { useNavigate } from 'react-router-dom';
import SearchBox from '../Components/SearchBox/searchBox.js';
import Button from '../Components/Button/Button.js';
import { useAuth } from '../Context/AuthContext.js';

const MapleHeader = () => {
  const menuList = ['분석방', '초보방', '자유게시판', '마이페이지'];
  const [isLogin, setIsLogin] = useState(true);
  const [searchValue, setSearchValue] = useState();
  const { uid, logout } = useAuth();
  const navigate = useNavigate();

  const goPage = (menu) => {
    console.log(menu);
    if (menu === '분석방') {
      navigate('/results');
    } else if (menu === '초보방') {
      navigate('/beginner');
    } else if (menu === '자유게시판') {
      navigate('/notice');
    } else if (menu === '마이페이지') {
      navigate('/mypage');
    }
  };

  const handleLogin = () => {
    navigate('/login');
  };

  const handleLogout = () => {
    logout();
  };

  return (
    <div className="flex w-full items-center justify-between bg-[#FFFFFF]/50 lg:h-[130px]">
      <div className="ml-8" onClick={() => navigate('/main')}>
        <Maple width="150" height="100" />
      </div>
      <div className="menu-area flex">
        {menuList.map((menu) => (
          <li
            className="mx-10 list-none font-bold hover:scale-125 duration-[1s] ease-in-out lg:text-lg"
            onClick={() => goPage(menu)}
          >
            {menu}
          </li>
        ))}
      </div>
      <div className="form-wrapper flex justify-between items-center">
        <form
          className="input-box w-1/5"
          onSubmit={(e) => {
            e.preventDefault(); // 기본 새로고침 방지
            if (!searchValue || searchValue.trim() === '') return; // 빈 값 체크
            navigate(`/searchpage?keyword=${encodeURIComponent(searchValue)}`);
          }}
        >
          <div className="mr-4">
            <SearchBox
              width="250px"
              value={searchValue}
              onChange={(e) => setSearchValue(e.target.value)}
              onClick={() => setSearchValue('')}
            />
          </div>
        </form>
        <div className="flex mr-8">
          {uid ? (
            <div className="hover:brightness-150 duration-[0.5s] ease-in-out">
              <Button width={'100px'} height={'40px'} onClick={handleLogout}>
                <div className="flex items-center">
                  <span>로그아웃</span>
                  <RiLogoutBoxRLine />
                </div>
              </Button>
            </div>
          ) : (
            <div className="hover:brightness-150 duration-[0.5s] ease-in-out">
              <Button width={'100px'} height={'40px'} onClick={handleLogin}>
                <div className="flex items-center">
                  <span>로그인</span>
                  <RiLoginBoxLine />
                </div>
              </Button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default MapleHeader;
