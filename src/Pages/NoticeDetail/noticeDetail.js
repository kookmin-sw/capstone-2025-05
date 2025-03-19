import React, { useState } from 'react';
import MapleHeader from '../../Components/MapleHeader';

import Button from '../../Components/Button/Button';
import Review from '../../Components/Review/Review';
import profile from '../../Assets/Images/google_profile.png';
import profile2 from '../../Assets/Images/google_profile2.png';
import heart from '../../Assets/Images/heart.png';
import flag from '../../Assets/Images/flag.png';
import { FaAngleDown, FaAngleUp } from 'react-icons/fa6';
// 하트 아이콘 저작권(fariha begum)
//깃발 아이콘 저작권(Hilmy Abiyyu A.)
export default function NoticeDetail() {
  // 더미 데이터 (실제 데이터 연동 시 props나 state로 변경)
  const post = {
    title: '기타 잘치는 법 좀 ...',
    writer: '마이클',
    date: '2025-02-13',
    views: 15,
    content:
      '기타 잘 치시는 분들 기타 잘 치는 법 좀 알려주세요~!!\nLost Starts 연습중인데 코드 보는 법 가르쳐주시면 커피 한 잔 사드리겠습니다!',
  };

  const comments = [
    {
      id: 1,
      profile,
      nickname: 'dgkim',
      content: '기타 너무 어려워요ㅠㅠㅠ 저도 가르쳐 주세요',
      likes: 110,
      dislikes: 12,
    },
    {
      id: 2,
      profile: profile2,
      nickname: '기타고수',
      content: '제가 알려드릴게요. 언제 어디서 볼까요??',
      likes: 110,
      dislikes: 12,
    },
    {
      id: 3,
      profile,
      nickname: 'park',
      content: '....기타......',
      likes: 95,
      dislikes: 12,
    },
    {
      id: 4,
      profile: profile2,
      nickname: 'guitar',
      content: '별로 안 어려운데....',
      likes: 11,
      dislikes: 120,
    },
  ];

  const filterComments = () => {
    setIsShow((prevIsShow) => {
      const newIsShow = !prevIsShow;
      setFilteredComments(newIsShow ? comments : comments.slice(0, 3));
      return newIsShow;
    });
  };

  const [isShow, setIsShow] = useState(false);
  const [filteredComments, setFilteredComments] = useState(
    comments.slice(0, 3),
  );

  return (
    <div>
      <MapleHeader />
      <section
        id="NoticeBoard"
        className="bg-[F0EDE6] flex flex-col items-center mt-10"
      >
        <div className="w-[80%]  bg-white shadow-lg  rounded-lg">
          <div className="min-h-[60vh]">
            <div
              id="title"
              className=" border-b border-[#C4A08E] p-4 min-h-[11vh]"
            >
              <h1 className="text-2xl font-bold align-middle !important">
                {post.title}
              </h1>
            </div>
            <table className="w-full  border-collapse min-h-[8vh]">
              <thead className="border-b border-[#C4A08E]">
                <tr>
                  <th className="bg-[#F0EDE6] border-r border-[#C4A08E] w-auto !important ">
                    글쓴이
                  </th>
                  <td>
                    <span className="ml-3">{post.writer}</span>
                  </td>
                  <th className="bg-[#F0EDE6] border-x border-[#C4A08E] w-auto !important">
                    작성 날짜
                  </th>
                  <td>
                    <span className="ml-3">{post.date}</span>
                  </td>
                  <th className="bg-[#F0EDE6] border-x border-[#C4A08E] w-auto !important">
                    조회수
                  </th>
                  <td>
                    <span className="ml-3">{post.views}</span>
                  </td>
                </tr>
              </thead>
            </table>
            <p className="mt-6 ml-2 whitespace-pre-line">{post.content}</p>
          </div>
        </div>

        <div className="flex w-[80%] justify-between mt-4">
          <div className="flex">
            <div className="mr-2 w-10">
              <img
                src={heart}
                className="duration-300 ease-in-out hover:scale-[110%]"
              />
            </div>
            <div className="w-10">
              <img
                src={flag}
                className="duration-300 ease-in-out hover:scale-[110%]"
              />
            </div>
          </div>
          <div className="flex">
            <div className="duration-300 ease-in-out hover:scale-[110%]">
              <Button width="60px" height="40px">
                수정
              </Button>
            </div>
            <div className="duration-300 ease-in-out hover:scale-[110%]">
              <Button width="60px" height="40px" backgroundColor="white">
                <span className="text-[#A57865] !important">목록</span>
              </Button>
            </div>
            <div className="duration-300 ease-in-out hover:scale-[110%]">
              <Button width="60px" height="40px">
                글쓰기
              </Button>
            </div>
          </div>
        </div>
      </section>
      <section id="reviews" className="flex flex-col items-center mt-14">
        <div
          id="review-caption"
          className="flex border-b-[2px] border-b-black w-[80%]"
        >
          <h className="font-bold text-xl mb-2">
            <strong>댓글 {comments.length}개</strong>{' '}
          </h>
        </div>
        <div id="review-contents" className="mt-4 w-[80%]">
          {comments.length <= 3 &&
            comments.map((item, index) => <Review comments={item} />)}
          {comments.length > 3 &&
            filteredComments.map((item, index) => <Review comments={item} />)}
        </div>
        <div>
          {!isShow && <FaAngleDown size={'30px'} onClick={filterComments} />}
          {isShow && <FaAngleUp size={'30px'} onClick={filterComments} />}
        </div>
      </section>
    </div>
  );
}
