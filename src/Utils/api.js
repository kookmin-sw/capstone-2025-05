import axios from 'axios';

const BACKEND_URL = process.env.REACT_APP_API_DATABASE_URL;
// 자유게시판 전체 데이터
const api = axios.create({
  baseURL: BACKEND_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

export default api;
