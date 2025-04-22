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
import SearchPage from './Pages/SearchPage/searchPage.js';
import PrintPage from './Pages/PrintPage/PrintPage';
import { useEffect, useState } from 'react';
import { SpotifyPlayerProvider } from './Context/SpotifyContext';

function App() {
  const [token, setToken] = useState(null);
  const clientId = process.env.REACT_APP_SPOTIFY_CLIENT_ID;
  const redirectUri = process.env.REACT_APP_SPOTIFY_REDIRECT_URI; // Spotify 앱 등록 시 동일하게 설정

  const scopes = [
    'streaming',
    'user-read-email',
    'user-read-private',
    'user-read-playback-state',
    'user-modify-playback-state',
  ];

  const authUrl =
    'https://accounts.spotify.com/authorize' +
    '?response_type=token' +
    `&client_id=${clientId}` +
    `&scope=${encodeURIComponent(scopes.join(' '))}` +
    `&redirect_uri=${encodeURIComponent(redirectUri)}`;

  useEffect(() => {
    // 최초 실행 시
    const hash = window.location.hash;
    const params = new URLSearchParams(hash.replace('#', ''));
    const accessToken = params.get('access_token');
    const expiresIn = params.get('expires_in');

    if (accessToken) {
      const expireAt = Date.now() + parseInt(expiresIn) * 1000;
      localStorage.setItem('spotify_access_token', accessToken);
      localStorage.setItem('spotify_token_expire_at', expireAt.toString());
      window.history.replaceState({}, document.title, '/');
      setToken(accessToken);
    } else {
      const storedToken = localStorage.getItem('spotify_access_token');
      const expireAt = parseInt(
        localStorage.getItem('spotify_token_expire_at'),
      );

      if (storedToken && expireAt && Date.now() < expireAt) {
        setToken(storedToken);
      } else {
        // 👉 여기서 바로 인증 URL로 이동 (만료되었으면)
        window.location.href = authUrl;
      }
    }
  }, []);

  return (
    <div className="App">
      <SpotifyPlayerProvider token={token} authUrl={authUrl}>
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
            <Route path="/searchpage" element={<SearchPage />} />
            <Route path="/print" element={<PrintPage />} />
          </Routes>
        </BrowserRouter>
      </SpotifyPlayerProvider>
    </div>
  );
}

export default App;
