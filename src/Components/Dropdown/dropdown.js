import React from 'react';
import Input from '../../Components/Input/input.js';
import Arrow from '../../Assets/arrow.svg';

export default function Dropdown(props) {
  const options = {
    skillLevel: [
      { value: '0', label: '입문 (기타를 시작한 지 얼마 안됨)' },
      { value: '1', label: '초급 (코드 몇 개 연주할 줄 아는 수준)' },
      { value: '2', label: '중급 (기본적인 곡 연주 가능)' },
      { value: '3', label: '상급 (핑거스타일, 즉흥 연주 가능)' },
      {
        value: '4',
        label: '전문가 (공연/레슨 경험 있음, 세션 연주 가능)',
      },
    ],
    genre: [
      { value: '0', label: '록 (Rock)' },
      { value: '1', label: '메탈 (Metal)' },
      { value: '2', label: '블루스 (Blues)' },
      { value: '3', label: '재즈 (Jazz)' },
      { value: '4', label: '클래식 (Classical)' },
      { value: '5', label: '포크 (Folk)' },
      { value: '6', label: '컨트리 (Country)' },
      { value: '7', label: '어쿠스틱 팝 (Acoustic Pop)' },
      { value: '8', label: '팝 (Pop)' },
      { value: '9', label: 'R&B / 소울 (R&B / Soul)' },
      { value: '10', label: '펑크 (Punk)' },
      { value: '11', label: '인디 (Indie)' },
    ],
  };

  const placeholderText =
    props.name === 'skillLevel'
      ? '실력을 선택해 주세요'
      : props.name === 'genre'
        ? '선호하는 장르를 선택해 주세요'
        : '옵션을 선택해 주세요';

  return (
    <div className="relative">
      <Input
        type="select"
        value={props.value}
        onChange={props.onChange}
        options={options[props.name] || []}
        width={props.width}
        height={props.height}
        placeholder={placeholderText}
      />

      <img
        src={Arrow}
        alt="arrow"
        className="absolute right-3 top-1/2 transform -translate-y-1/2 pointer-events-none"
      />
    </div>
  );
}
