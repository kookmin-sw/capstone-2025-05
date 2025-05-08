import axios from 'axios';

const AUDIO_URL = process.env.REACT_APP_AUDIO_URL;
// 자유게시판 전체 데이터
const audioApi = axios.create({
  baseURL: AUDIO_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

export default audioApi;
