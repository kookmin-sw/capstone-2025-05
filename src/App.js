import './App.css';
import { BrowserRouter } from 'react-router-dom';
import { useEffect, useState } from 'react';
import { SpotifyPlayerProvider } from './Context/SpotifyContext';
import MapleHeader from './Components/MapleHeader';
import { AuthProvider, useAuth } from './Context/AuthContext'; // 추가
import AppRoutes from './Routes/AppRoutes';
import { Hash } from 'lucide-react';

function App() {
  const [token, setToken] = useState(null);
  const { uid } = useAuth();
  const clientId = process.env.REACT_APP_SPOTIFY_CLIENT_ID;
  const redirectUri = process.env.REACT_APP_SPOTIFY_REDIRECT_URI;
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
      }
    }
  }, []);

  return (
    <div className="App">
      <SpotifyPlayerProvider token={token} authUrl={authUrl}>
        <BrowserRouter>
          <MapleHeader />
          <AppRoutes />
        </BrowserRouter>
      </SpotifyPlayerProvider>
    </div>
  );
}

export default App;
