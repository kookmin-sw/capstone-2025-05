import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../Utils/api';

const postWrite = ({ post }) => {
  return api.post(`/posts`, {
    id: post.id,
    제목: post.title,
    작성자: post.writer,
    작성일시: post.write_time,
    조회수: post.view,
    내용: post.content,
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
