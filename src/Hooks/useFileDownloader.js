// src/hooks/useFileDownloader.js
import axios from 'axios';

export default function useFileDownloader() {
  const downloadFile = async (url, filename) => {
    try {
      const response = await axios.get(url, {
        responseType: 'blob',
      });

      const blob = new Blob([response.data]);
      const link = document.createElement('a');
      link.href = URL.createObjectURL(blob);
      link.download = filename;
      link.click();

      URL.revokeObjectURL(link.href);
    } catch (error) {
      console.error('파일 다운로드 실패:', error);
      alert('다운로드에 실패했습니다.');
    }
  };

  return downloadFile;
}
