import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const postComments = ({
  id,
  uid,
  작성일시,
  프로필이미지,
  postid,
  작성자,
  내용,
}) => {
  return api.post(`/comment`, {
    id,
    uid,
    작성일시,
    프로필이미지,
    postid,
    작성자,
    내용,
    좋아요수: 0,
    싫어요수: 0,
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
