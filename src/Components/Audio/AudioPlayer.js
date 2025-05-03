import { useState } from 'react';
import CompareWaveform from '../Chart/CompareWaveForm';
import AudioModal from '../Modal/AudioModal';
import Button from '../Button/Button';

export default function AudioPlayer({ userAudio, referenceAudio }) {
  const [isModalOpen, setIsModalOpen] = useState(false);

  const openModal = () => setIsModalOpen(true);
  const closeModal = () => setIsModalOpen(false);

  return (
    <div className="flex flex-col w-full items-center justify-center space-y-2 ">
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
      <Button onClick={openModal} overwrite={'w-full h-12 mt-[15px]'}>
        동시 재생 및 비교
      </Button>
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
