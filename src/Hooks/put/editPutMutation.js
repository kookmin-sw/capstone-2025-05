import { useMutation, useQueryClient } from '@tanstack/react-query';
import api from '../../Utils/api';

const updatePost = ({
  id,
  editedTitle,
  writer,
  write_time,
  view,
  editedContent,
}) => {
  return api.put(`/posts/${id}`, {
    id,
    제목: editedTitle,
    작성자: writer,
    작성일시: write_time,
    조회수: view,
    내용: editedContent,
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
