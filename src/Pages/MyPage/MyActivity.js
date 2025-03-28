import React from 'react';
import { Link } from 'react-router-dom'; 
import Box from '../../Components/Box/Box.js';
import Header from '../../Components/MapleHeader.js';
import Information from '../../Assets/MyPage/sidebar_profile.svg';
import Setting from '../../Assets/MyPage/Setting.svg';  
import Music from '../../Assets/MyPage/Vector.svg';    
import Bookmark from '../../Assets/MyPage/filledBookmark.svg';
import Heart from '../../Assets/MyPage/filledHeart.svg';
import Write from '../../Assets/MyPage/wirte.svg';

export default function MyActivity() {
    const bookmarks = [
      { title: "기타 잘 치는 법좀", content: "여기에는 기타 잘 치는 방법에 대한 내용이 들어갑니다. 예를 들어, 기본적인 코드 연습부터 시작해서 고급 기법까지 설명할 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 3", content: "세 번째 북마크된 글의 내용입니다. 다양한 연주 방법에 대한 설명이 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." }
    ];
    const likes = [
      { title: "좋아요 글 1", content: "좋아요 받은 글 내용 1. 이 글은 다른 사람들이 많은 관심을 가진 글입니다." },
      { title: "좋아요 글 2", content: "좋아요 받은 글 내용 2. 이 글은 기타와 관련된 팁을 공유하고 있습니다." },
      { title: "좋아요 글 3", content: "좋아요 받은 글 내용 3. 독특한 연주 스타일을 소개하는 글입니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." }
      // 예시를 더 추가할 수 있습니다.
    ];
    const myPosts = [
      { title: "내 글 1", content: "내가 작성한 글 내용 1. 기타 연주 연습에 대한 글입니다." },
      { title: "내 글 2", content: "내가 작성한 글 내용 2. 기타 기법을 마스터하기 위한 단계별 계획입니다." },
      { title: "내 글 3", content: "내가 작성한 글 내용 3. 초보자를 위한 기타 연습법을 공유합니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." },
      { title: "북마크 글 2", content: "이 글은 두 번째 북마크된 글입니다. 이 글에는 기타와 관련된 많은 정보가 포함되어 있을 수 있습니다." }
    ];

    return (
      <div className="flex flex-col">
        <Header />
        <div className="flex">
          <div className="w-[180px] h-[620px] bg-[#463936] text-white p-4 flex flex-col justify-between">
            <div>
              <h2 className="text-md font-bold">MAPLE</h2>
              <ul className="mt-4 space-y-2">
                <li className="menu-item flex items-center gap-2 py-2 shadow-lg">
                  <img src={Information} alt="내 정보 아이콘" className="w-4 h-4" />
                  <Link to="/mypage" className="text-white">내 정보</Link>
                </li>
                <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                  <img src={Music} alt="연주한 곡 아이콘" className="w-4 h-4" />
                  <Link to="/playedmusic" className="text-white">연주한 곡</Link>
                </li>
                <li className="menu-item flex items-center gap-2 py-2 hover:shadow-lg">
                  <img src={Setting} alt="관리 아이콘" className="w-4 h-4" />
                  <Link to="/setting" className="text-white">관리</Link>
                </li>
              </ul>
            </div>

            <div>
              <p className="font-semibold">Kildong Hong</p>
              <button className="mt-2 text-sm text-red-400 hover:underline">
                탈퇴하기
              </button>
            </div>
          </div>

          <div className="flex-grow p-6">
            <div className="flex gap-12 mt-5 ml-7">

              <Box width="300px" height="530px">
                <div className="p-3 flex items-center gap-3 border-b">
                  <img src={Bookmark} alt="북마크" className="w-8 h-8" />
                  <span className="font-bold text-[18px]">북마크</span>
                </div>
                <div className="h-[470px] overflow-y-auto p-3 space-y-2">
                  {bookmarks.map((item, index) => (
                    <div key={index} className="p-2 bg-gray-100 rounded-md shadow block hover:underline">
                      <div className="font-bold">{item.title}</div>
                      <div className="text-sm text-gray-600 line-clamp-2">{item.content}</div>
                    </div>
                  ))}
                </div>
              </Box>

              {/* 좋아요 */}
              <Box width="300px" height="530px">
                <div className="p-3 flex items-center gap-3 border-b">
                  <img src={Heart} alt="좋아요" className="w-8 h-8" />
                  <span className="font-bold text-[18px]">좋아요</span>
                </div>
                <div className="h-[470px] overflow-y-auto p-3 space-y-2">
                  {likes.map((item, index) => (
                    <div key={index} className="p-2 bg-gray-100 rounded-md shadow block hover:underline">
                      <div className="font-bold">{item.title}</div>
                      <div className="text-sm text-gray-600 line-clamp-2">{item.content}</div>
                    </div>
                  ))}
                </div>
              </Box>

              {/* 내가 쓴 글 */}
              <Box width="300px" height="530px">
                <div className="p-3 flex items-center gap-3 border-b">
                  <img src={Write} alt="내가 쓴 글" className="w-8 h-8" />
                  <span className="font-bold text-[18px]">내가 쓴 글</span>
                </div>
                <div className="h-[470px] overflow-y-auto p-3 space-y-2">
                  {myPosts.map((item, index) => (
                    <div key={index} className="p-2 bg-gray-100 rounded-md shadow block hover:underline">
                      <div className="font-bold">{item.title}</div>
                      <div className="text-sm text-gray-600 line-clamp-2">{item.content}</div>
                    </div>
                  ))}
                </div>
              </Box>

            </div>
          </div>
        </div>
      </div>
    );
}
