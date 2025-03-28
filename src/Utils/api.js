import axios from 'axios';
// 자유게시판 전체 데이터
const api = axios.create({
  baseURL: 'http://localhost:8001/',
  headers: {
    'Content-Type': 'application/json',
  },
});

export default api;
