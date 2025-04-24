import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postComments = ({ uid, 작성일시, postid, 작성자, 내용, 비밀번호 }) => {
  return api.post(`post/comments`, {
    //프로필이미지도 추가해야됨
    uid,
    postid,
    내용,
    비밀번호,
    작성일시,
    작성자,
  });
};

export const usePostCommentsMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: postComments,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries(['comments', variables.postid]); // 캐시된 데이터 갱신
    },
  });
};
