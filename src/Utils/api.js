import axios from 'axios';

const api = axios.create({
  baseURL: 'https://c4a86d04-6038-4278-8530-5893188f1d63.mock.pstmn.io/',
  headers: {
    Accept: 'application/json',
  },
});

export default api;
