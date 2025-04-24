import React from 'react';
import printIcon from '../../Assets/Images/print_icon.png';
import { RiArrowDropDownLine } from 'react-icons/ri';

export default function DropdownBttn() {
  return (
    <div className="relative w-40">
      <button className="w-full flex items-center justify-between border px-4 py-2 rounded">
        <div className="flex">
          <img src={printIcon} alt="Print Icon" className="w-5 h-5 mr-2" />
          <span>프린트</span>
        </div>
        <RiArrowDropDownLine />
      </button>
      <ul className="absolute left-0 mt-1 w-full border bg-white rounded shadow-md">
        <li className="px-4 py-2 hover:bg-gray-200 cursor-pointer">
          삼성 프린트
        </li>
        <li className="px-4 py-2 hover:bg-gray-200 cursor-pointer">
          lg 프린트
        </li>
      </ul>
    </div>
  );
}
