import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import Login from './Pages/Login/login';
import Signup from './Pages/Login/signup';
import Profile from './Pages/Login/profile';
import Main from './Pages/Main/main';
import WritePage from './Pages/WritePage/writePage';
import MyPage from './Pages/MyPage/MyPage';
import NoticeBoard from './Pages/NoticeBoard/noticeBoard';
import NoticeDetail from './Pages/NoticeDetail/noticeDetail';

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
          <Route path="/write" element={<WritePage />} />
          <Route path="/mypage" element={<MyPage />} />
          <Route path="/notice" element={<NoticeBoard />} />
          <Route path="/noticeDetail/:id" element={<NoticeDetail />} />
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
