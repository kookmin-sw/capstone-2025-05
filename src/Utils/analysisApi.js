import axios from 'axios';

const RESULT_URL = process.env.REACT_APP_RESULT_URL;

// 분석결과
const analysisApi = axios.create({
  baseURL: RESULT_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

export default analysisApi;
