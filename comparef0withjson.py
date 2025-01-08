import librosa
import numpy as np
import matplotlib.pyplot as plt
import sounddevice as sd
from scipy import signal

import json

def extract_pitch_with_yin(y, sr=22050, hop_length=512):
    """
    음원에서 YIN 알고리즘으로 기본 주파수(F0)를 추출
    """
    y = librosa.util.normalize(y)
    f0, voiced_flag, voiced_probs = librosa.pyin(
        y,
        fmin=librosa.note_to_hz('C2'),
        fmax=librosa.note_to_hz('C7'),
        sr=sr,
        frame_length=512,
        hop_length=hop_length
    )
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
                return f0_ref
    return f0_test

def create_measures_from_bpm(audio_duration, bpm, total_measures=32, beats_per_measure=4):
    """
    주어진 BPM으로 마디 위치 생성 (32마디에 맞춤)
    """
    # 이론적인 한 마디 길이
    beat_duration = 60 / bpm  # 한 비트 길이 (초)
    measure_duration = beat_duration * beats_per_measure  # 한 마디 길이 (초)
    
    # 32마디에 맞춰 마디 길이 조정
    adjusted_measure_duration = audio_duration / total_measures
    
    print(f"Audio duration: {audio_duration:.2f} seconds")
    print(f"Theoretical measure duration: {measure_duration:.2f} seconds")
    print(f"Adjusted measure duration: {adjusted_measure_duration:.2f} seconds")
    
    # 32마디에 맞춘 시간 위치 생성
    measure_times = np.arange(total_measures + 1) * adjusted_measure_duration
    
    return measure_times

def segment_by_measures(f0, sr, hop_length, measure_times):
    """
    마디별로 F0 데이터 분할
    """
    # 시간축 생성
    times = librosa.frames_to_time(np.arange(len(f0)), sr=sr, hop_length=hop_length)
    
    # 디버깅 정보
    print(f"Time range: {times[0]:.2f} to {times[-1]:.2f} seconds")
    print(f"Measure times: {measure_times}")
    
    # 마디별 데이터 저장
    measure_segments = []
    
    for i in range(len(measure_times) - 1):
        start_time = measure_times[i]
        end_time = measure_times[i + 1]
        
        # 해당 마디에 속하는 F0 데이터 추출
        mask = (times >= start_time) & (times < end_time)
        segment = f0[mask]
        
        print(f"Measure {i+1}: {len(segment)} frames")
        measure_segments.append(segment)
    
    return measure_segments

def play_audio_segment(y, sr):
    """
    오디오 세그먼트 재생
    """
    sd.play(y, sr)
    sd.wait()  # 재생이 끝날 때까지 대기

def extract_measure_audio(y, sr, start_time, end_time):
    """
    전체 오디오에서 특정 마디 부분만 추출
    """
    start_sample = int(start_time * sr)
    end_sample = int(end_time * sr)
    return y[start_sample:end_sample]

def compare_f0_with_measures(f0_ref,
                             f0_test,
                             sr,
                             hop_length,
                             bpm,
                             tolerance=10,
                             y_ref=None,
                             y_test=None,
                             export_json=False,
                             json_file="analysis_result.json"):
    """
    마디 단위로 F0 비교 및 시각화 (32마디)
    export_json=True 이면 결과를 JSON 파일로도 저장한다.
    """
    # 데이터 확인
    print(f"F0 ref shape: {f0_ref.shape}")
    print(f"F0 test shape: {f0_test.shape}")
    
    # 전체 오디오 길이 계산 (F0 데이터 기준)
    audio_duration = len(f0_ref) * hop_length / sr
    
    # 마디 위치 계산
    measure_times = create_measures_from_bpm(audio_duration, bpm, total_measures=32)
    
    print(f"Using BPM: {bpm}")
    print(f"Total measures: {len(measure_times) - 1}")
    
    # 마디별 데이터 분할
    measures_ref = segment_by_measures(f0_ref, sr, hop_length, measure_times)
    measures_test = segment_by_measures(f0_test, sr, hop_length, measure_times)
    
    print(f"Number of measures: {len(measures_ref)}")
    if len(measures_ref) > 0:
        print(f"First measure shape: {measures_ref[0].shape}")
    
    # 마디별 차이 계산을 위한 리스트
    measure_differences = []
    
    # 각 마디별 정규화 및 차이 계산
    normalized_measures_test = []
    
    for measure_ref, measure_test in zip(measures_ref, measures_test):
        if len(measure_ref) == 0 or len(measure_test) == 0:
            print("Empty measure found!")
            measure_differences.append(0)
            normalized_measures_test.append([])
            continue
            
        normalized = np.array([
            normalize_to_fundamental(f_ref, f_test, tolerance)
            for f_ref, f_test in zip(measure_ref, measure_test)
        ])
        normalized_measures_test.append(normalized)
        
        # 마디별 평균 차이 계산
        diff = np.mean(np.abs(measure_ref - normalized))
        measure_differences.append(diff)
    
    if not normalized_measures_test:
        print("No normalized data generated!")
        return
    
    # 차이값의 임계값 계산 (예: 상위 25% 이상인 경우를 큰 차이로 간주)
    threshold = np.percentile(measure_differences, 75)
    
    # -------------------------------
    # 1) 시각화 (matplotlib)
    # -------------------------------
    fig, axes = plt.subplots(8, 4, figsize=(20, 24))
    axes = axes.flatten()
    
    def on_click(event):
        if event.inaxes:
            ax_idx = np.where(axes == event.inaxes)[0]
            if len(ax_idx) > 0:
                measure_idx = ax_idx[0]
                if measure_idx < len(measure_times) - 1:
                    start_time = measure_times[measure_idx]
                    end_time = measure_times[measure_idx + 1]
                    
                    if y_ref is not None and y_test is not None:
                        # 원본 마디 재생
                        print(f"\n재생 중: {measure_idx + 1}번 마디 (원본)")
                        measure_audio = extract_measure_audio(y_ref, sr, start_time, end_time)
                        play_audio_segment(measure_audio, sr)
                        
                        # 잠시 대기
                        plt.pause(0.5)
                        
                        # 테스트 마디 재생
                        print(f"재생 중: {measure_idx + 1}번 마디 (테스트)")
                        measure_audio = extract_measure_audio(y_test, sr, start_time, end_time)
                        play_audio_segment(measure_audio, sr)
    
    # 클릭 이벤트 연결
    fig.canvas.mpl_connect('button_press_event', on_click)
    
    # 마디별 차이 큰 구간(large diff 구간) 기록을 위한 구조
    per_measure_data = []

    for i in range(min(32, len(normalized_measures_test))):
        ax = axes[i]
        measure_ref = measures_ref[i]
        measure_norm = normalized_measures_test[i]
        
        if len(measure_ref) == 0:
            print(f"Empty measure at index {i}")
            continue
        
        # 마디 내부 시간축 계산 (상대 시간)
        times = np.arange(len(measure_ref)) * hop_length / sr
        
        # 그래프 그리기
        ax.plot(times, measure_ref, label='Original', color='blue')
        ax.plot(times, measure_norm, label='Normalized', color='green')
        
        # 차이가 큰 마디인지 확인
        is_significant_diff = measure_differences[i] > threshold
        
        # largeDiffSections를 저장하기 위한 임시 리스트
        large_diff_sections = []
        
        # 차이가 큰 마디에서만 세부 구간 표시
        if is_significant_diff:
            point_differences = np.abs(measure_ref - measure_norm)
            point_threshold = np.percentile(point_differences, 90)
            
            # 연속된 큰 차이 구간 찾기
            from itertools import groupby
            from operator import itemgetter
            
            large_diff_points = point_differences > point_threshold
            
            for k, g in groupby(enumerate(large_diff_points), key=itemgetter(1)):
                if k:  # True인 구간만 처리
                    group = list(g)
                    start_idx = group[0][0]
                    end_idx = group[-1][0]
                    
                    # 최소 3포인트 이상 연속되는 경우만 표시
                    if end_idx - start_idx >= 3:
                        # 사각형 그리기
                        rect = plt.Rectangle(
                            (times[start_idx], min(measure_ref[start_idx], measure_norm[start_idx])),
                            times[end_idx] - times[start_idx],
                            max(
                                max(measure_ref[start_idx:end_idx+1]),
                                max(measure_norm[start_idx:end_idx+1])
                            ) - min(
                                min(measure_ref[start_idx:end_idx+1]),
                                min(measure_norm[start_idx:end_idx+1])
                            ),
                            facecolor='red',
                            alpha=0.2
                        )
                        ax.add_patch(rect)
                        
                        # large_diff_sections에 구간 기록 (절대시간이 아닌, 마디 내부 상대시간)
                        large_diff_sections.append({
                            "startTime": float(times[start_idx]),
                            "endTime": float(times[end_idx])
                        })
        
        title_color = 'red' if is_significant_diff else 'black'
        ax.set_title(f'Measure {i+1}\nClick to play', color=title_color)
        ax.set_ylabel('Frequency (Hz)')
        ax.set_xlabel('Time (s)')
        ax.grid(True)
        if i == 0:
            ax.legend()
        
        # perMeasureData에 기록할 dict 구성
        per_measure_data.append({
            "index": i,
            "refF0": measure_ref.tolist(),
            "testF0": measures_test[i].tolist(),
            "normalizedTestF0": measure_norm.tolist(),
            "averageDiff": float(measure_differences[i]),
            "largeDiffSections": large_diff_sections
        })
    
    plt.tight_layout()
    plt.show()
    
    # -------------------------------
    # 2) JSON 데이터 구조 만들기
    # -------------------------------
    result_data = {
        "measureTimes": measure_times.tolist(),     # 마디 시작/끝 시점
        "f0Ref": f0_ref.tolist(),                   # 전체 음원(원본)의 F0
        "f0Test": f0_test.tolist(),                 # 전체 음원(테스트)의 F0
        "measureDifferences": [float(d) for d in measure_differences],
        "threshold": float(threshold),
        "perMeasureData": per_measure_data
    }
    
    # JSON 파일로 저장 (export_json=True 일 때만)
    if export_json:
        with open(json_file, "w", encoding="utf-8") as f:
            json.dump(result_data, f, ensure_ascii=False, indent=2)
        print(f"\n[INFO] JSON result exported to '{json_file}'")

def analyze_guitar_performance(original_file, played_file, bpm,
                               export_json=False,
                               json_file="analysis_result.json"):
    """
    기타 연주 분석 실행 (BPM 지정)
    export_json=True 이면 결과를 JSON 파일로 저장
    """
    # 오디오 로드
    origin_y, origin_sr = librosa.load(original_file)
    played_y, played_sr = librosa.load(played_file)
    
    # F0 추출
    f0_ref, sr = extract_pitch_with_yin(origin_y, origin_sr, 256)
    f0_test, _ = extract_pitch_with_yin(played_y, played_sr, 256)
    
    # 마디별 비교 분석 (오디오 데이터도 전달)
    compare_f0_with_measures(
        f0_ref, f0_test, sr, hop_length=256, bpm=bpm,
        tolerance=10,
        y_ref=origin_y,
        y_test=played_y,
        export_json=export_json,
        json_file=json_file
    )

# 사용 예시
if __name__ == "__main__":
    analyze_guitar_performance(
        original_file='audio/26-gp(clean).wav',
        played_file='audio/26-record(clean).wav',
        bpm=92,
        export_json=True,                  # True로 설정하면 JSON 파일로 결과 저장
        json_file="analysis_result.json"   # 저장할 파일명
    )
