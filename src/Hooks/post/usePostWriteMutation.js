import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postWrite = ({ post }) => {
  return api.post(`/post/posts`, {
    uid: post.uid,
    게시판종류이름: 'string',
    내용: post.content,
    댓글갯수: 0,
    비밀번호: '1234',
    작성일시: post.write_time,
    작성자: post.author,
    제목: post.title,
    조회수: post.view,
    좋아요수: 0,
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
