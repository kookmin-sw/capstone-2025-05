export const loadSpotifySDK = () => {
  return new Promise((resolve) => {
    const script = document.createElement('script');
    script.src = 'https://sdk.scdn.co/spotify-player.js';
    script.onload = resolve;
    document.body.appendChild(script);
  });
};
