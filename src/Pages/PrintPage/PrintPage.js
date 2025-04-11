import React, { useRef } from 'react';
import PlayedMusic from '../MyPage/PlayedMusic';
import graph from '../../Assets/Images/음정비교시각화 1.svg';
import Button from '../../Components/Button/Button';
import { useNavigate } from 'react-router-dom';

export default function PrintPage() {
  const record = {
    tempo: 92,
    rhythm: 89,
    pitch: 85,
  };
  const printRef = useRef();

  const navigate = useNavigate();

  const handlePrint = () => {
    const printContent = printRef.current.innerHTML;
    const originalContent = document.body.innerHTML;

    document.body.innerHTML = printContent;
    window.print();
    document.body.innerHTML = originalContent;
    window.location.reload();
  };

  return (
    <div className="min-h-screen">
      <PlayedMusic opacity={true} />
      <div className="absolute flex left-[25%] top-[10%]  w-[45%] h-[80%] bg-white rounded-[15px]">
        <div
          id="미리보기"
          className="flex flex-col justify-center items-center w-full bg-[#D9D9D9] h-full rounded-[15px]"
        >
          <div
            id="인쇄내용"
            className="flex flex-col bg-white h-[90%] w-[90%]"
            ref={printRef}
          >
            <h1 className="my-12 ml-8 text-3xl font-bold">음원 분석</h1>
            <div className="flex">
              <div className="flex flex-col">
                <h1 className="ml-8 text-xl font-bold">
                  <p>음정 비교 시각화</p>
                  <p>그래프</p>
                </h1>
                <img className="ml-8" src={graph} />
              </div>
              <div className="flex flex-col">
                <h1 className="ml-8 text-xl font-bold">
                  <p>원곡 음정 시각화</p>
                  <p>그래프</p>
                </h1>
                <img className="ml-8" src={graph} />
              </div>
            </div>
            <div className="flex">
              <div className="flex flex-col mt-5">
                <h1 className="ml-8 text-xl font-bold">
                  <p>음정 비교 시각화</p>
                  <p>그래프</p>
                </h1>
                <img className="ml-8" src={graph} />
              </div>
              <div className="flex flex-col mt-5 w-[40%]">
                <h1 className="ml-8 text-xl font-bold">
                  <p>나의 연주 분석</p>
                  <p>그래프</p>
                </h1>
                <div className="felx flex-co ml-8 print-color-adjust">
                  <p className="text-blue-500">템포: {record.tempo}%</p>
                  <div
                    className="bg-blue-500 h-3 rounded-full "
                    style={{ width: `${record.tempo}%` }}
                  ></div>
                  <p className="text-green-500 mt-5">박자: {record.rhythm}%</p>
                  <div
                    className="bg-green-500 h-3 rounded-full print:block"
                    style={{ width: `${record.rhythm}%` }}
                  ></div>
                  <p className="text-red-500 mt-5">음정: {record.pitch}%</p>
                  <div
                    className="bg-red-500 h-3 rounded-full print:block"
                    style={{ width: `${record.pitch}%` }}
                  ></div>
                </div>
              </div>
            </div>
          </div>
          <div className="flex relative bottom-[60px] ">
            <div className="hover:scale-[105%] duration-300 ease-in-out ">
              <Button
                onClick={handlePrint}
                backgroundColor="#0973FF"
                width={'80px'}
                height={'40px'}
              >
                인쇄하기
              </Button>
            </div>
            <div className="ml-4 hover:scale-[105%] duration-300 ease-in-out ">
              <Button
                onClick={() => navigate('/feedback')}
                backgroundColor="#ff0303"
                width={'80px'}
                height={'40px'}
              >
                취소하기
              </Button>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
