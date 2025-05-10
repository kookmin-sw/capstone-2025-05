import axios from 'axios';

const ANALYSIS_URL = process.env.REACT_APP_ANALYSIS_URL;

// 분석결과
const analysisApi = axios.create({
  baseURL: ANALYSIS_URL,
  headers: {
    'Content-Type': 'application/json',
  },
});

export default analysisApi;
