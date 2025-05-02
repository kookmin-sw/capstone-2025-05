import React from 'react';
import Search from '../../Assets/search.svg';

export default function SearchBox(props) {
  return (
    <div
      className="relative w-full"
      style={{
        width: props.width,
        height: props.height,
      }}
    >
      <input
        className={`w-full border border-[#A57865] rounded-full pr-12 pl-4 py-2 outline-none ${props.tailwind}`}
        style={{ height: '100%' }}
        onClick={props.onClick}
        onChange={props.onChange}
        value={props.value}
        onKeyDown={(e) => {
          if (e.key === 'Enter') {
            props.onSearch?.(props.value);
          }
          props.onKeyDown?.(e); // 기존 핸들링 유지
        }}
      />
      <button
        className="absolute right-4 top-1/2 transform -translate-y-1/2"
        onClick={() => props.onSearch?.(props.value)}
      >
        <img src={Search} alt="검색" />
      </button>
    </div>
  );
}
