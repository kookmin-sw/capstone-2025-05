import { useState } from 'react';
import CompareWaveform from '../Chart/CompareWaveForm';
import AudioModal from '../Modal/AudioModal';
import Button from '../Button/Button';

export default function AudioPlayer({ userAudio, referenceAudio }) {
  const [isModalOpen, setIsModalOpen] = useState(false);

  const openModal = () => setIsModalOpen(true);
  const closeModal = () => setIsModalOpen(false);

  return (
    <div className="flex flex-col items-center justify-center space-y-2 ">
      <div className="w-full">
        <h3>
          <strong>사용자 연주</strong>
        </h3>
        <audio
          id="user-audio"
          controls
          src={userAudio}
          className="w-full"
        ></audio>
      </div>
      <div className="w-full">
        <h3>
          <strong>원본 연주</strong>
        </h3>
        <audio
          id="reference-audio"
          controls
          src={referenceAudio}
          className="w-full"
        ></audio>
      </div>
      <button
        onClick={openModal}
        className="bg-green-500 text-white px-4 py-2 rounded"
      >
        동시 재생 및 비교
      </button>
      <Button onClick={openModal}>동시 재생 및 비교</Button>
      {/* Button에 px-4 py-2 적용 */}
      <AudioModal isOpen={isModalOpen} onClose={closeModal}>
        <CompareWaveform
          key={Date.now()}
          userAudio={userAudio}
          referenceAudio={referenceAudio}
        />
      </AudioModal>
    </div>
  );
}
