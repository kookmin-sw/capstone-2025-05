// src/hooks/useFileDownloader.js
import axios from 'axios';
import swal from 'sweetalert';

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
      if (error.status === 404) {
        swal({
          text: '파일이 존재하지 않습니다😥',
          icon: 'info',
          buttons: {
            confirm: {
              text: '확인',
              className: 'custom-confirm-button',
            },
          },
        });
        return;
      }
      console.log(error, '에러처리');
    }
  };

  return downloadFile;
}
