import AudioPlayer from '../../Components/Audio/AudioPlayer';

export default function audioTestPage() {
  return (
    <div className="flex flex-col items-center">
      <AudioPlayer
        userAudio={'/Audio/homecoming.wav'}
        referenceAudio={'/Audio/homecoming-error-1.wav'}
      />
    </div>
  );
}
