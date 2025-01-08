import librosa
import numpy as np
import matplotlib.pyplot as plt

def extract_pitch_with_yin(y, sr=22050, hop_length=512):
    """
    음원에서 YIN 알고리즘으로 기본 주파수(F0)를 추출
    """
    # 오디오 정규화
    y = librosa.util.normalize(y)

    # YIN 알고리즘으로 F0 추출
    f0, voiced_flag, voiced_probs = librosa.pyin(
        y, fmin=librosa.note_to_hz('C2'), fmax=librosa.note_to_hz('C7'), sr=sr, frame_length=512, hop_length=hop_length
    )
    
    # NaN 값을 0으로 변환 (무음 구간 처리)
    f0 = np.nan_to_num(f0, nan=0.0)
    
    return f0, sr

def calculate_harmonics(f0, num_harmonics=4):
    """
    기본 주파수(F0)로부터 배음을 계산
    """
    return [n * f0 for n in range(1, num_harmonics + 1)]

def normalize_to_fundamental(f0_ref, f0_test, tolerance=10):
    """
    배음을 비교하여 동일 음으로 정규화
    """
    harmonics_ref = calculate_harmonics(f0_ref)
    harmonics_test = calculate_harmonics(f0_test)

    for h_test in harmonics_test:
        for h_ref in harmonics_ref:
            if abs(h_test - h_ref) <= tolerance:
                # 동일 음으로 간주하고 기본 주파수 반환
                return f0_ref
    # 동일 음이 아닌 경우 원래 주파수 유지
    return f0_test

def compare_f0_with_harmonics(f0_ref, f0_test, sr, hop_length, tolerance=10):
    """
    두 음원의 F0를 비교하고 배음을 정규화한 결과를 시각화
    """
    # 시간축 계산
    times = librosa.frames_to_time(np.arange(len(f0_ref)), sr=sr, hop_length=hop_length)
    
    # 배음 기반 정규화
    normalized_f0_test = np.array([
        normalize_to_fundamental(f_ref, f_test, tolerance)
        for f_ref, f_test in zip(f0_ref, f0_test)
    ])
    
    # F0 비교 시각화
    plt.figure(figsize=(12, 6))
    plt.plot(times, f0_ref, label='Original Audio (F0)', color='blue')
    # plt.plot(times, f0_test, label='Played Audio (F0)', color='orange', alpha=0.6)
    plt.plot(times, normalized_f0_test, label='Normalized Played Audio (F0)', color='green')
    plt.title('Comparison of Fundamental Frequencies (F0) with Normalization')
    plt.xlabel('Time (s)')
    plt.ylabel('Frequency (Hz)')
    plt.legend()
    plt.grid()
    plt.show()

# 테스트 파일
original_file = 'audio/26-gp(clean).wav'  # 원본 음원
played_file = 'audio/26-record(miss1).wav'  # 연주 음원

# 오디오 로드
origin_y, origin_sr = librosa.load(original_file)
played_y, played_sr = librosa.load(played_file)

# YIN 알고리즘으로 F0 추출
f0_ref, sr = extract_pitch_with_yin(origin_y, origin_sr, 256)
f0_test, _ = extract_pitch_with_yin(played_y, sr, 256)

# 배음 기반 비교 및 정규화
compare_f0_with_harmonics(f0_ref, f0_test, sr, hop_length=256, tolerance=10)
