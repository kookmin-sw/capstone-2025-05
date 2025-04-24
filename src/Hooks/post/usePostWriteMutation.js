import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

/* 글 작성 기능 */

const postWrite = ({ post }) => {
  const formData = new FormData();
  formData.append('uid', post.uid);
  formData.append('content', post.content);
  formData.append('author', post.author);
  formData.append('title', post.title);

  // 선택적으로 이미지/오디오가 있다면 추가
  if (post.image) {
    formData.append('image', post.image); // post.image는 File 객체
  }
  if (post.audio) {
    formData.append('audio', post.audio); // post.audio도 File 객체
  }

  return api.post(`/posts`, formData, {
    headers: {
      'Content-Type': 'multipart/form-data',
    },
  });
};

export const usePostWriteMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postWrite,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('postNoticeBoard'); // 캐시된 데이터 갱신
    },
  });
};
