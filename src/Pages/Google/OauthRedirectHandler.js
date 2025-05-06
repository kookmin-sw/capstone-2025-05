import { useEffect } from 'react';
import { useLocation, useNavigate } from 'react-router-dom';
import { useAuth } from '../../Context/AuthContext';

export default function OauthRedirectHandler() {
  const location = useLocation();
  const navigate = useNavigate();
  const { login } = useAuth();

  useEffect(() => {
    const hash = window.location.hash; // #/oauth-success?uid=xxx
    console.log('hash', hash);
    const query = hash.split('?')[1];
    const params = new URLSearchParams(query);
    const uid = params.get('uid');

    if (hash.startsWith('#/oauth-success')) {
      if (uid) {
        console.log('✅ App.js에서 OAuth uid 감지됨:', uid);
        login(uid); // context에 저장
        sessionStorage.setItem('uid', uid); // 혹시 몰라 수동 저장
        window.location.hash = '#/main'; // 강제 이동
      }
    }

    console.log('OAuth 진입:', uid);
    if (uid) {
      login(uid);
      navigate('/main');
    }
  }, []);

  return <div>로그인 처리 중입니다...</div>;
}
