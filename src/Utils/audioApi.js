import axios from 'axios';

const MEDIA_URL = process.env.REACT_APP_MEDIA_URL;
// 자유게시판 전체 데이터
const audioApi = axios.create({
  baseURL: MEDIA_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

export default audioApi;
