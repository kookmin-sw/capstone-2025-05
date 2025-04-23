// hooks/useSpotifyPlayback.ts
import { useCallback } from 'react';
import axios from 'axios';

export const useSpotifyPlayback = ({
  token,
  deviceId,
  isReady,
  authUrl,
  onError,
  onTokenExpired,
}: any) => {
  const sendPlaybackCommand = useCallback(
    async ({ action, body }: any) => {
      if (!isReady || !deviceId) {
        onError?.('🎧 Spotify Player가 아직 준비되지 않았어요!');
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
