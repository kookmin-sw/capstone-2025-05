import { useEffect } from 'react';
import { useNavigate, useLocation } from 'react-router-dom';
import { useAuth } from '../../Context/AuthContext';

export default function OauthRedirectHandler() {
  const navigate = useNavigate();
  const location = useLocation();
  const { login } = useAuth();

  useEffect(() => {
    const params = new URLSearchParams(location.search);
    const uid = params.get('uid');
    const picture = params.get('picture');

    if (uid) {
      console.log('✅ OAuth uid 감지됨:', uid);
      login(uid);
      sessionStorage.setItem('uid', uid);
      if (picture) {
        sessionStorage.setItem('profile_picture', picture);
      }

      navigate('/main', { replace: true });
    } else {
      navigate('/login');
    }
  }, [location]);

  return <div>로그인 처리 중입니다...</div>;
}
