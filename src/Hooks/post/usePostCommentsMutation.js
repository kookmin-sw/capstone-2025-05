import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

/* 댓글 전송 기능 */

const postComments = ({
  uid,
  postid,
  작성자,
  내용,
  비밀번호,
  프로필이미지,
}) => {
  return api.post(`/comment`, {
    //프로필이미지도 추가해야됨
    uid,
    postid,
    내용,
    비밀번호,
    작성자,
    프로필이미지,
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
