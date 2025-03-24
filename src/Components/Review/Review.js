import React from 'react';
import recommendGood from '../../Assets/Images/recommend_good.png';
import recommendBad from '../../Assets/Images/recommend_bad.png';

export default function Review({ comments, fakeImg }) {
  return (
    <div className="flex items-center mb-8">
      <div className="rounded-full overflow-hidden mr-2">
        <img
          src={fakeImg} //나중에 백엔드 완료되면 comments.프로필이미지로 바꾸고 fakeImg는 삭제할것
          width={'80px'}
          height={'80px'}
          className="object-cover"
        />
      </div>
      <div className="flex flex-col">
        <strong>{comments.작성자}</strong>
        <p className="my-1">{comments.내용}</p>
        <div className="flex">
          <div className="flex mr-1">
            <img
              src={recommendGood}
              width={'30px'}
              height={'30px'}
              className="duration-300 ease-in-out hover:scale-[110%]"
            />
            {comments.좋아요수}
          </div>
          <div className="flex">
            <img
              src={recommendBad}
              width={'30px'}
              height={'30px'}
              className="duration-300 ease-in-out hover:scale-[110%]"
            />
            {comments.싫어요수}
          </div>
        </div>
      </div>
    </div>
  );
}
