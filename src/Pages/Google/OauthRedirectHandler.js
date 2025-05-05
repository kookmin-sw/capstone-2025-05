import { useEffect } from 'react';
import { useSearchParams, useNavigate } from 'react-router-dom';
import { useAuth } from '../../Context/AuthContext';

export default function OauthRedirectHandler() {
  const [searchParams] = useSearchParams();
  const uid = searchParams.get('uid');
  const navigate = useNavigate();
  const { login } = useAuth();

  useEffect(() => {
    if (uid) {
      login(uid); // Context 로그인 처리
      navigate('/main'); // 메인 페이지로 이동
    }
  }, [uid]);

  return <div>로그인 처리 중입니다...</div>;
}
