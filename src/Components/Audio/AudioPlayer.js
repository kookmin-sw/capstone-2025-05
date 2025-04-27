import { useState } from 'react';
import CompareWaveform from '../Chart/CompareWaveForm';
import AudioModal from '../Modal/AudioModal';

export default function AudioPlayer({ userAudio, referenceAudio }) {
  const [isModalOpen, setIsModalOpen] = useState(false);

  const openModal = () => setIsModalOpen(true);
  const closeModal = () => setIsModalOpen(false);

  return (
    <div class="flex flex-col items-center justify-center space-y-2">
      <div>
        <h3>
          <strong>사용자 연주</strong>
        </h3>
        <audio id="user-audio" controls src={userAudio}></audio>
      </div>
      <div>
        <h3>
          <strong>원본 연주</strong>
        </h3>
        <audio id="reference-audio" controls src={referenceAudio}></audio>
      </div>
      <button
        onClick={openModal}
        className="bg-green-500 text-white px-4 py-2 rounded"
      >
        동시 재생 및 비교
      </button>

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
