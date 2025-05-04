export default function PostAttachmentTable({ image_url, audio_url }) {
  return (
    <table className="w-full text-sm text-left border mt-8 border-gray-200">
      <thead>
        <tr className="bg-gray-50 text-gray-700">
          <th className="py-3 px-4 border-b">첨부 이미지</th>
          <th className="py-3 px-4 border-b">첨부 오디오</th>
        </tr>
      </thead>
      <tbody>
        <tr className="text-gray-800">
          <td className="py-3 px-4 border-b">
            {image_url ? (
              <a
                href={image_url}
                download
                className="text-blue-600 hover:underline"
              >
                이미지 다운로드
              </a>
            ) : (
              <span className="text-gray-400">없음</span>
            )}
          </td>
          <td className="py-3 px-4 border-b">
            {audio_url ? (
              <a
                href={audio_url}
                download
                className="text-blue-600 hover:underline"
              >
                오디오 다운로드
              </a>
            ) : (
              <span className="text-gray-400">없음</span>
            )}
          </td>
        </tr>
      </tbody>
    </table>
  );
}
