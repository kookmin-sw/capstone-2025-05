import React, { useState } from 'react';
import Maple from '../Assets/Images/logo.js';
import { RiLogoutBoxRLine, RiLoginBoxLine } from 'react-icons/ri';
import { useNavigate } from 'react-router-dom';
import SearchBox from '../Components/SearchBox/searchBox.js';
import Button from '../Components/Button/Button.js';

const MapleHeader = () => {
  const menuList = ['분석하기', '초보방', '자유게시판', '마이페이지'];
  const [isLogin, setIsLogin] = useState(true);
  const navigate = useNavigate();
  return (
    <div className="flex w-full items-center justify-between bg-[#FFFFFF]/50">
      <div className="ml-8" onClick={() => navigate('/')}>
        <Maple width="150" height="100" />
      </div>
      <div className="menu-area flex">
        {menuList.map((menu) => (
          <li
            className="mx-10 list-none font-bold hover:scale-125 duration-[1s] ease-in-out"
            onClick={() => navigate(`/${menu}`)}
          >
            {menu}
          </li>
        ))}
      </div>
      <div className="form-wrapper flex justify-between items-center">
        <form className="input-box w-1/5" onSubmit={''}>
          <div className="searchBox mr-4">
            <SearchBox width="250px" />
          </div>
        </form>
        <div className="flex mr-8">
          {isLogin ? (
            <div className="hover:brightness-150 duration-[0.5s] ease-in-out">
              <Button
                width={'100px'}
                height={'40px'}
                onClick={() => setIsLogin(!isLogin)}
              >
                <div className="flex items-center">
                  <span>로그아웃</span>
                  <RiLogoutBoxRLine />
                </div>
              </Button>
            </div>
          ) : (
            <div className="hover:brightness-150 duration-[0.5s] ease-in-out">
              <Button
                width={'100px'}
                height={'40px'}
                onClick={() => setIsLogin(!isLogin)}
              >
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
