import './App.css';
import { Route, Routes, BrowserRouter } from 'react-router-dom';
import TestPage from './Pages/TestPage/testPage';
import Login from './Pages/Login/login';
import Profile from './Pages/Login/profile';
import Main from './Pages/Main/main';
import WritePage from './Pages/WritePage/writePage';
import MyPage from './Pages/MyPage/MyPage';
import PlayedMusic from './Pages/MyPage/PlayedMusic';

function App() {
  return (
    <div className="App">
      <BrowserRouter>
        <Routes>
          <Route path="/test" element={<TestPage />} />
          <Route path="/login" element={<Login />} />
          <Route path="/profile" element={<Profile />} />
          <Route path="/main" element={<Main />} />
          <Route path="/write" element={<WritePage />} />
          <Route path="/mypage" element={<MyPage />} />
          <Route path="/playedmusic" element={<PlayedMusic />} />
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
