import librosa
import numpy as np
from librosa.sequence import dtw
from scipy.signal import butter, lfilter

def bandpass_filter(data, lowcut, highcut, sr, order=5):
    nyquist = 0.5 * sr
    low = lowcut / nyquist
    high = highcut / nyquist
    b, a = butter(order, [low, high], btype="band")
    return lfilter(b, a, data)

def sync_with_dtw(original_file, user_file, sr=22050):
    # 두 파일을 동일 sr로 로드
    y1, _ = librosa.load(original_file, sr=sr)
    y2, _ = librosa.load(user_file, sr=sr)
    
    # 필요하다면 HPF/LPF or bandpass
    y1 = bandpass_filter(y1, 80, 1500, sr)
    y2 = bandpass_filter(y2, 80, 1500, sr)

    # 스펙트로그램 생성 시 동일한 파라미터 사용
    n_mels = 128  # mel 밴드 수 지정
    S1 = librosa.feature.melspectrogram(y=y1, sr=sr, n_mels=n_mels)
    S2 = librosa.feature.melspectrogram(y=y2, sr=sr, n_mels=n_mels)
    
    # log-scale melspectrogram으로 변환
    S1_db = librosa.power_to_db(S1, ref=np.max)
    S2_db = librosa.power_to_db(S2, ref=np.max)

    # DTW
    D, wp = librosa.sequence.dtw(S1_db, S2_db, subseq=True)
    
    # 단순 예시: 구간 추출
    aligned_start = min([p[0] for p in wp])
    aligned_end = max([p[0] for p in wp])
    
    # 프레임을 시간으로 변환
    return (aligned_start * 512 / sr, aligned_end * 512 / sr)  # hop_length=512 가정

def analyze_file_with_pitch(file_path, sr=22050, start_time=None, end_time=None):
    y, _ = librosa.load(file_path, sr=sr)
    if start_time and end_time:
        y = y[int(start_time*sr) : int(end_time*sr)]

    # 노이즈 필터링
    y = bandpass_filter(y, 80, 1500, sr)

    # 비트/템포 추출
    tempo, beat_frames = librosa.beat.beat_track(y=y, sr=sr)
    beat_times = librosa.frames_to_time(beat_frames, sr=sr)

    # (개선) 프레임 단위 피치 추출
    pitches, mags = librosa.piptrack(y=y, sr=sr)
    # pitches.shape = (freq_bins, time_frames)
    # 여기서 각 프레임별 가장 큰 magnitude를 가지는 bin을 찾아 F0 추출
    pitch_values = []
    for frame in range(pitches.shape[1]):
        idx = np.argmax(mags[:, frame])
        pitch = pitches[idx, frame]
        if pitch > 0:  # 0이 아니면 pitch 있다고 가정
            pitch_values.append(pitch)

    # 대표값(평균) 사용
    if len(pitch_values) > 0:
        avg_pitch = np.mean(pitch_values)
    else:
        avg_pitch = None

    return {
        "tempo": tempo,
        "beat_times": beat_times,
        "avg_pitch": avg_pitch,
    }

def compare_analysis(original, user):
    feedback = []

    # 템포 비교 (numpy 값을 float로 변환)
    tempo_diff = float(abs(original["tempo"] - user["tempo"]))
    if tempo_diff < 5:
        feedback.append("템포가 원곡과 유사합니다.")
    else:
        if original["tempo"] > user["tempo"]:
            feedback.append(f"템포가 약 {tempo_diff:.2f} BPM 느립니다.")
        else:
            feedback.append(f"템포가 약 {tempo_diff:.2f} BPM 빠릅니다.")

    # 박자 비교
    original_beats = original["beat_times"]
    user_beats = user["beat_times"]
    min_len = min(len(original_beats), len(user_beats))
    if min_len > 0:
        beat_diffs = np.abs(original_beats[:min_len] - user_beats[:min_len])
        avg_beat_diff = float(np.mean(beat_diffs))  # numpy 값을 float로 변환
        if avg_beat_diff < 0.05:
            feedback.append("박자가 거의 정확합니다.")
        else:
            feedback.append(f"박자가 평균 {avg_beat_diff:.3f} 초 정도 차이가 납니다.")
    else:
        feedback.append("박자 정보를 비교할 수 없습니다.")

    # 피치 비교
    if original["avg_pitch"] is None or user["avg_pitch"] is None:
        feedback.append("음정을 분석할 수 없습니다.")
    else:
        freq_diff = float(abs(original["avg_pitch"] - user["avg_pitch"]))  # numpy 값을 float로 변환
        if freq_diff < 5:
            feedback.append("음정이 거의 일치합니다.")
        else:
            if original["avg_pitch"] > user["avg_pitch"]:
                feedback.append(f"음정이 기준보다 {freq_diff:.2f}Hz 낮습니다. 튜닝 필요.")
            else:
                feedback.append(f"음정이 기준보다 {freq_diff:.2f}Hz 높습니다. 튜닝 필요.")

    return feedback

# 실행 예시
if __name__ == "__main__":
    sr = 22050
    original_file = "audio/26-gp.wav"
    user_file = "audio/26-guitar.wav"

    # DTW로 동기화 구간 찾고
    start_time, end_time = sync_with_dtw(original_file, user_file, sr=sr)

    # 동일 구간 분석
    original_analysis = analyze_file_with_pitch(original_file, sr=sr, start_time=start_time, end_time=end_time)
    user_analysis = analyze_file_with_pitch(user_file, sr=sr, start_time=start_time, end_time=end_time)

    # 비교
    feedback = compare_analysis(original_analysis, user_analysis)
    for line in feedback:
        print(line)
