import React from 'react';
import Header from '../../Components/MapleHeader';
import Footer from '../../Components/MapleFooter';

// 주제별로 링크 분리
const linkGroups = {
  basic: [
    {
      url: 'https://m.blog.naver.com/eowjd92/220270122911',
      title: '기본 코드 잡는 법',
      description: '기타 기본 코드 세팅에 대한 블로그 포스트',
      type: 'blog',
    },
    {
      url: 'https://blog.naver.com/murim_gosu/222532712829',
      title: '기타 독학할 때 잡기 어려운 코드 BEST 3',
      description:
        '초보자들이 어려워하는 코드와 그 해결 방법을 소개하는 블로그 포스트',
      type: 'blog',
    },
    {
      url: 'https://www.youtube.com/watch?v=25bXwbpE60I&list=PLhpI9y5iAB8BqdpqnK1LWiwvCIGQUNs-q',
      title: '코드 잡는 법 1 / C코드 잡는 법 [기타코드 배우기]',
      description: '언제까지 기초코드만 잡을 것인가? 코드잡는 법 강좌!!',
      type: 'youtube',
    },
  ],
  powerChord: [
    {
      url: 'https://www.youtube.com/watch?v=M-kIhuwlTz4',
      title: '파워코드 정복하기 : 기초편',
      description:
        '파워코드를 잡는 자세, 뮤트하는 방법 등을 상세하게 설명하는 기초 강의',
      type: 'youtube',
    },
  ],
  technique: [
    {
      url: 'https://www.youtube.com/playlist?list=PL_JwD6H8sbcy25pC3O7_-bj4aLTJkRHZD',
      title: '[기본 테크닉] 유튜브 플레이리스트',
      description:
        '초보자를 위한 풀링오프, 슬라이드 등 다양한 기본 테크닉을 소개하는 강좌 모음',
      type: 'blog',
    },
    {
      url: 'https://blog.naver.com/asdasj/222093118981',
      title: '초보자를 위한 슬라이드 배우기',
      description:
        '슬라이드 테크닉의 기본 개념과 연습 방법을 소개하는 블로그 포스트',
      type: 'blog',
    },
    {
      url: 'https://www.youtube.com/watch?v=fGqOLdA0PHo',
      title:
        '[초급레슨] 기타 솔로의 기본이자 필수 테크닉 해머온, 풀링오프, 슬라이드의 기초를 배워보는 시간',
      description:
        '기타 솔로 연주에서 자주 사용되는 해머온, 풀링오프, 슬라이드 테크닉을 다루는 강의',
      type: 'youtube',
    },
  ],
  scale: [
    {
      url: 'https://blog.naver.com/artistwant/221233213578',
      title: '기타솔로를 위한 코드톤과 스케일 연습하기',
      description:
        '코드톤과 스케일을 활용한 솔로 연습 방법을 소개하는 블로그 포스트',
      type: 'blog',
    },
  ],
};

// 유튜브 썸네일 추출 함수
const getYoutubeThumbnail = (url) => {
  const match = url.match(/v=([^&]+)/);
  return match ? `https://img.youtube.com/vi/${match[1]}/hqdefault.jpg` : null;
};

// 공통 카드 컴포넌트
const LinkCard = ({ preview }) => (
  <a
    href={preview.url}
    target="_blank"
    rel="noopener noreferrer"
    className="flex items-center w-[1120px] space-x-8 border p-4 rounded-xl shadow-md"
  >
    {preview.type === 'youtube' && (
      <img
        src={getYoutubeThumbnail(preview.url)}
        alt={preview.title}
        className="w-60 h-40 object-cover rounded-lg"
      />
    )}
    <div>
      <div className="text-xl font-bold text-[#463936]">{preview.title}</div>
      <p className="text-lg text-gray-600 mt-2">{preview.description}</p>
    </div>
  </a>
);

export default function Beginner() {
  return (
    <div className="flex flex-col min-h-screen">
      <div className="flex flex-col mt-16 ml-20 space-y-16">
        <div>
          <h2 className="text-2xl font-bold mb-4">기본 코드 잡는법</h2>
          <div className="space-y-4">
            {linkGroups.basic.map((preview, index) => (
              <LinkCard key={index} preview={preview} />
            ))}
          </div>
        </div>

        <div>
          <h2 className="text-2xl font-bold mb-4">파워코드 강의</h2>
          <div className="space-y-4">
            {linkGroups.powerChord.map((preview, index) => (
              <LinkCard key={index} preview={preview} />
            ))}
          </div>
        </div>

        <div>
          <h2 className="text-2xl font-extrabold mb-4">테크닉 관련 강의</h2>
          <div className="space-y-4">
            {linkGroups.technique.map((preview, index) => (
              <LinkCard key={index} preview={preview} />
            ))}
          </div>
        </div>

        <div>
          <h2 className="text-2xl font-extrabold mb-4">스케일 연습</h2>
          <div className="space-y-4">
            {linkGroups.scale.map((preview, index) => (
              <LinkCard key={index} preview={preview} />
            ))}
          </div>
        </div>
      </div>
      <Footer className="mt-auto w-full" />
    </div>
  );
}
