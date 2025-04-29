import React from 'react';
import { Routes, Route } from 'react-router-dom';
import { useAuth } from '../Context/AuthContext';

// 페이지 컴포넌트 불러오기
import TestPage from '../Pages/TestPage/testPage';
import Login from '../Pages/Login/login';
import Signup from '../Pages/Login/signup';
import Profile from '../Pages/Login/profile';
import Main from '../Pages/Main/main';
import NoticeBoard from '../Pages/NoticeBoard/noticeBoard';
import NoticeDetail from '../Pages/NoticeDetail/noticeDetail';
import WritePage from '../Pages/WritePage/writePage';
import Ranking from '../Pages/Ranking/ranking';
import MyPage from '../Pages/MyPage/MyPage';
import PlayedMusic from '../Pages/MyPage/PlayedMusic';
import Feedback from '../Pages/MyPage/Feedback';
import Setting from '../Pages/MyPage/Setting';
import MyActivity from '../Pages/MyPage/MyActivity';
import SearchPage from '../Pages/SearchPage/searchPage';
import PrintPage from '../Pages/PrintPage/PrintPage';

export default function AppRoutes() {
  const { uid } = useAuth();

  return (
    <Routes>
      {uid ? (
        // 로그인한 경우
        <>
          <Route path="/main" element={<Main />} />
          <Route path="/notice" element={<NoticeBoard />} />
          <Route path="/noticeDetail/:id" element={<NoticeDetail />} />
          <Route path="/write" element={<WritePage />} />
          <Route path="/ranking" element={<Ranking />} />
          <Route path="/mypage" element={<MyPage />} />
          <Route path="/playedmusic" element={<PlayedMusic />} />
          <Route path="/feedback" element={<Feedback />} />
          <Route path="/setting" element={<Setting />} />
          <Route path="/myactivity" element={<MyActivity />} />
          <Route path="/searchpage" element={<SearchPage />} />
          <Route path="/print" element={<PrintPage />} />
          <Route path="/test" element={<TestPage />} />
          <Route path="*" element={<Main />} />
        </>
      ) : (
        // 로그인 안 한 경우
        <>
          <Route path="/login" element={<Login />} />
          <Route path="/signup" element={<Signup />} />
          <Route path="/profile" element={<Profile />} />
          <Route path="*" element={<Login />} />
        </>
      )}
    </Routes>
  );
}
