import { useEffect } from 'react';
import { useSearchParams, useNavigate } from 'react-router-dom';
import { useAuth } from '../../Context/AuthContext';

export default function OauthRedirectHandler() {
  const navigate = useNavigate();
  const [searchParams] = useSearchParams();
  const { login } = useAuth();
  console.log('실행됨');
  useEffect(() => {
    const uid = searchParams.get('uid');
    if (uid) {
      login(uid); // Context 로그인 처리
      navigate('/main'); // 메인 페이지로 이동
    }
  }, []);

  return <div>로그인 처리 중입니다...</div>;
}
