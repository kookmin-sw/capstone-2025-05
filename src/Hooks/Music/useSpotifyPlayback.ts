// hooks/useSpotifyPlayback.ts
import { useCallback } from 'react';
import axios from 'axios';
import Swal from 'sweetalert2';

export const useSpotifyPlayback = ({
  token,
  deviceId,
  isReady,
  authUrl,
  onError,
  onTokenExpired,
}: any) => {
  const sendPlaybackCommand = useCallback(
    async ({ action, body, authUrl }: any) => {
      if (!isReady || !deviceId) {
        Swal.fire({
          icon: 'warning',
          title: 'Spotify 플레이어가 준비되지 않았어요 🎧',
          text: 'Spotify 인증을 먼저 해주세요!',
          confirmButtonText: 'Spotify 인증하기',
          allowOutsideClick: false,
        }).then((result) => {
          if (result.isConfirmed) {
            window.location.href = authUrl;
          }
        });

        return;
      }

      try {
        const res = await axios.put(
          `https://api.spotify.com/v1/me/player/${action}?device_id=${deviceId}`,
          body,
          {
            headers: {
              Authorization: `Bearer ${token}`,
              'Content-Type': 'application/json',
            },
          },
        );
        return res;
      } catch (err: any) {
        if (err.response?.status === 401) {
          console.log('🔐 토큰 만료! 다시 로그인');
          localStorage.removeItem('spotify_access_token');
          onTokenExpired?.();
          window.location.href = authUrl;
        } else {
          const message =
            err.response?.data?.error?.message || '❌ 알 수 없는 오류 발생';
          console.error(message);
          onError?.(message);
        }
      }
    },
    [isReady, deviceId, token, authUrl, onError, onTokenExpired],
  );

  return { sendPlaybackCommand };
};
