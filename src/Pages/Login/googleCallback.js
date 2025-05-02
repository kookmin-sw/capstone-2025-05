import React, { useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';

export default function GoogleCallback() {
  const navigate = useNavigate();
  const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;

  useEffect(() => {
    const handleGoogleCallback = async () => {
      const code = new URLSearchParams(window.location.search).get('code');

      if (!code) {
        console.error('code가 없습니다.');
        alert('구글 인증에 실패했습니다.');
        return;
      }

      try {
        const callbackUrl = `${BACKEND_URL}/account/google-auth-callback?code=${code}`;
        const res = await axios.get(callbackUrl);

        const uid = res.data.uid;

        if (uid) {
          sessionStorage.setItem('uid', uid);
          console.log('구글 로그인 성공');
          navigate('/main');
        } else {
          console.log('구글 로그인 실패');
          navigate('/login');
        }
      } catch (error) {
        console.error('구글 로그인 오류:', error);

        if (error.response) {
          console.error('서버 응답 오류:', error.response.data);

          if (error.response.status === 400) {
            alert(
              `구글 로그인 실패: ${JSON.stringify(error.response.data.detail)}`,
            );
          } else if (error.response.status === 500) {
            alert(
              `서버 내부 오류: ${JSON.stringify(error.response.data.detail)}`,
            );
          } else {
            alert('알 수 없는 서버 오류가 발생했습니다.');
          }
        } else {
          alert('서버에 연결할 수 없습니다.');
        }
      }
    };

    handleGoogleCallback();
  }, [navigate, BACKEND_URL]);

  return (
    <div className="flex justify-center items-center h-screen text-lg">
      구글 로그인 처리 중입니다...
    </div>
  );
}
