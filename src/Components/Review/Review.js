import React from 'react';
import recommendGood from '../../Assets/Images/recommend_good.png';
import recommendBad from '../../Assets/Images/recommend_bad.png';

export default function Review({ comments }) {
  return (
    <div className="flex items-center mb-8">
      <div className="rounded-full overflow-hidden mr-2">
        <img
          src={comments.profile}
          width={'80px'}
          height={'80px'}
          className="object-cover"
        />
      </div>
      <div className="flex flex-col">
        <strong>{comments.nickname}</strong>
        <p className="my-1">{comments.content}</p>
        <div className="flex">
          <div className="flex mr-1">
            <img
              src={recommendGood}
              width={'30px'}
              height={'30px'}
              className="duration-300 ease-in-out hover:scale-[110%]"
            />
            {comments.likes}
          </div>
          <div className="flex">
            <img
              src={recommendBad}
              width={'30px'}
              height={'30px'}
              className="duration-300 ease-in-out hover:scale-[110%]"
            />
            {comments.dislikes}
          </div>
        </div>
      </div>
    </div>
  );
}
