import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import Login from './Pages/Login/login';
import Signup from './Pages/Login/signup';
import Profile from './Pages/Login/profile';
import Main from './Pages/Main/main';
import NoticeBoard from './Pages/NoticeBoard/noticeBoard';
import NoticeDetail from './Pages/NoticeDetail/noticeDetail';
import WritePage from './Pages/WritePage/writePage';
import Ranking from './Pages/Ranking/ranking';
import MyPage from './Pages/MyPage/MyPage';
import PlayedMusic from './Pages/MyPage/PlayedMusic';
import Feedback from './Pages/MyPage/Feedback';
import Setting from './Pages/MyPage/Setting';
import MyActivity from './Pages/MyPage/MyActivity';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <Routes>
          <Route path="/test" element={<TestPage />} />
          <Route path="/login" element={<Login />} />
          <Route path="/signup" element={<Signup />} />
          <Route path="/profile" element={<Profile />} />
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
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
