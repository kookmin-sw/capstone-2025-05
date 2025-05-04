import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

/*게시글 수정하기 */
const updatePost = ({ post_id, editedTitle, editedContent }) => {
  const formData = new FormData();
  formData.append('title', editedTitle);
  formData.append('content', editedContent);

  return api.put(`/posts/${post_id}`, formData, {
    headers: {
      'Content-Type': 'multipart/form-data',
    },
  });
};

export const useEditPutMutation = () => {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn: updatePost,
    onSuccess: (data, variables) => {
      queryClient.invalidateQueries('putNoticeBoard'); // 캐시된 데이터 갱신
    },
  });
};
