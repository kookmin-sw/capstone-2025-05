import os
import tempfile
import numpy as np
import scipy.signal  # SciPy 패치를 위해 추가
from celery import Celery
from celery.utils.log import get_task_logger
import json
from datetime import datetime
import librosa

# SciPy 호환성 패치 적용
# 최신 버전의 SciPy에서는 scipy.signal.hann이 scipy.signal.windows.hann으로 이동했습니다
if not hasattr(scipy.signal, 'hann') and hasattr(scipy.signal, 'windows') and hasattr(scipy.signal.windows, 'hann'):
    print("패치 적용 (tasks): scipy.signal.hann -> scipy.signal.windows.hann")
    scipy.signal.hann = scipy.signal.windows.hann

from workers.dsp import analyze_simple, compare_audio_with_reference
# dsp 모듈의 개별 함수들을 직접 가져와서 진행 상황을 추적할 수 있도록 합니다
from workers.dsp import (
    load_audio_from_bytes, load_midi_from_bytes, 
    extract_tempo, extract_onsets, extract_pitch_with_crepe,
    predict_techniques, align_audio_with_dtw, segment_audio_with_midi_notes,
    enhanced_segment_audio_with_midi_notes, extract_chroma
)
# 피드백 생성기 추가
from workers.feedback import GrokFeedbackGenerator
# MongoDB 저장 기능 추가
from app.db import save_analysis_result, save_comparison_result, save_feedback, save_reference_features, get_reference_features

# Initialize Celery app
celery_app = Celery('maple_audio_analyzer')
celery_app.config_from_object('celeryconfig')
logger = get_task_logger(__name__)


@celery_app.task(bind=True, name='workers.tasks.analyze_audio')
def analyze_audio(self, audio_bytes, request_dict=None):
    """
    Celery task to analyze audio content.
    
    Parameters:
    - audio_bytes: Binary content of the uploaded audio file
    - request_dict: Dictionary with additional analysis parameters
    
    Returns:
    - Dictionary with analysis results
    """
    logger.info(f"Starting audio analysis task {self.request.id}")
    self.update_state(state='STARTED', meta={'progress': 0})
    
    try:
        # Process the request dictionary if provided (5%)
        self.update_state(state='PROCESSING', meta={'progress': 5})
        if request_dict is None:
            request_dict = {}
        
        analysis_type = request_dict.get('analysis_type', 'simple')
        user_id = request_dict.get('user_id')
        song_id = request_dict.get('song_id')
        generate_feedback = request_dict.get('generate_feedback', False)
        
        # 오디오 로드 (10%)
        self.update_state(state='PROCESSING', meta={'progress': 10})
        y, sr = load_audio_from_bytes(audio_bytes)
        
        # 오디오 초기 처리 (15%)
        self.update_state(state='PROCESSING', meta={'progress': 15})
        
        # 템포 추출 (20%)
        self.update_state(state='PROCESSING', meta={'progress': 20})
        tempo = extract_tempo(y, sr)
        
        # 템포 분석 후처리 (25%)
        self.update_state(state='PROCESSING', meta={'progress': 25})
        
        # 노트 시작점 전처리 (30%)
        self.update_state(state='PROCESSING', meta={'progress': 30})
        
        # 노트 시작점 추출 (40%)
        self.update_state(state='PROCESSING', meta={'progress': 40})
        onsets = extract_onsets(y, sr)
        
        # 노트 시작점 후처리 (45%)
        self.update_state(state='PROCESSING', meta={'progress': 45})
        
        # 세그먼트 생성 (50%)
        self.update_state(state='PROCESSING', meta={'progress': 50})
        segments = []
        for i in range(len(onsets) - 1):
            start = int(onsets[i] * sr)
            end = int(onsets[i+1] * sr)
            segments.append(y[start:end])
        
        if onsets:
            start = int(onsets[-1] * sr)
            segments.append(y[start:])
        
        # 세그먼트 후처리 (55%)
        self.update_state(state='PROCESSING', meta={'progress': 55})
        
        # 기법 예측 준비 (60%)
        self.update_state(state='PROCESSING', meta={'progress': 60})
        model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
        techniques = []
        
        # 기법 예측 진행 (65%)
        self.update_state(state='PROCESSING', meta={'progress': 65})
        
        # 기법 예측 (70%)
        self.update_state(state='PROCESSING', meta={'progress': 70})
        if os.path.exists(model_path):
            techniques = predict_techniques(segments, model_path, sr)
        
        # 기법 예측 후처리 (75%)
        self.update_state(state='PROCESSING', meta={'progress': 75})
        
        # 결과 준비 (80%)
        self.update_state(state='PROCESSING', meta={'progress': 80})
        
        # 결과 생성 (85%)
        self.update_state(state='PROCESSING', meta={'progress': 85})
        
        # 결과 생성 및 마무리 (90%)
        self.update_state(state='FINALIZING', meta={'progress': 90})
        result = {
            "tempo": tempo,
            "onsets": onsets,
            "number_of_notes": len(onsets),
            "duration": float(len(y) / sr),
            "techniques": techniques,
            "created_at": datetime.utcnow().isoformat()
        }
        
        # 메타데이터 추가 (92%)
        self.update_state(state='FINALIZING', meta={'progress': 92})
        result['metadata'] = {
            'user_id': user_id,
            'song_id': song_id,
            'analysis_type': analysis_type,
            'task_id': self.request.id
        }

        # 피드백 생성 옵션이 활성화된 경우
        if generate_feedback:
            self.update_state(state='FINALIZING', meta={'progress': 95})
            logger.info(f"Generating feedback for analysis task {self.request.id}")
            
            try:
                # GROK API를 사용하여 피드백 생성
                feedback_generator = GrokFeedbackGenerator()
                feedback_result = feedback_generator.generate_feedback(result, is_comparison=False)
                
                # 피드백이 성공적으로 생성된 경우 결과에 추가
                if 'feedback' in feedback_result:
                    result['feedback'] = feedback_result['feedback']
                    result['feedback_metadata'] = feedback_result.get('metadata', {})
                    
                    # 피드백을 MongoDB에 저장
                    feedback_result['created_at'] = datetime.utcnow().isoformat()
                    save_feedback(self.request.id, feedback_result)
                # 오류가 발생한 경우 오류 메시지 추가
                elif 'error' in feedback_result:
                    result['feedback_error'] = feedback_result['error']
            except Exception as e:
                logger.exception(f"Error generating feedback: {str(e)}")
                result['feedback_error'] = f"피드백 생성 중 오류 발생: {str(e)}"
        
        # MongoDB에 결과 저장 (97%)
        self.update_state(state='FINALIZING', meta={'progress': 97})
        logger.info(f"Saving analysis results to MongoDB for task {self.request.id}")
        save_analysis_result(self.request.id, result)
        
        # 완료 (99%)
        self.update_state(state='FINALIZING', meta={'progress': 99})
        logger.info(f"Audio analysis task {self.request.id} completed successfully")
        # 100% 완료는 Celery에서 자동으로 처리됩니다
        return result
        
    except Exception as e:
        logger.exception(f"Error in audio analysis task: {str(e)}")
        raise


@celery_app.task(bind=True, name='workers.tasks.compare_audio')
def compare_audio(self, user_audio_bytes, reference_audio_bytes=None, midi_bytes=None, user_id=None, song_id=None, generate_feedback=False):
    """
    Celery task to compare user audio with reference audio and/or MIDI.
    
    Parameters:
    - user_audio_bytes: Binary content of the user's audio recording
    - reference_audio_bytes: Binary content of the reference audio (optional)
    - midi_bytes: Binary content of the MIDI file (optional)
    - user_id: ID of the user who uploaded the audio (optional)
    - song_id: ID of the song being analyzed (optional)
    - generate_feedback: Whether to generate textual feedback using GROK API
    
    Returns:
    - Dictionary with comparison results
    """
    logger.info(f"Starting audio comparison task {self.request.id}")
    self.update_state(state='STARTED', meta={'progress': 5})
    
    try:
        if song_id:
            # DB에서 레퍼런스 특성 가져오기
            logger.info(f"Getting reference features from DB for song_id: {song_id}")
            ref_features = get_reference_features(song_id)
            
            if not ref_features:
                raise ValueError(f"레퍼런스 오디오 특성을 찾을 수 없습니다: song_id={song_id}")
            
            # 저장된 MIDI 데이터 가져오기
            midi_data = ref_features.get('midi_data')
            
            # 새로운 MIDI가 제공된 경우 이를 우선 사용
            if midi_bytes:
                notes, tempos, tempo_times = load_midi_from_bytes(midi_bytes)
                midi_data = {
                    "notes": notes,
                    "tempos": tempos,
                    "tempo_times": tempo_times
                }
            
            # 사용자 오디오 로드
            user_y, sr = load_audio_from_bytes(user_audio_bytes)
            
            # 미리 계산된 레퍼런스 특성 사용
            ref_tempo = ref_features.get('tempo')
            ref_onsets = ref_features.get('onsets')
            ref_pitches = ref_features.get('pitches')
            ref_techniques = ref_features.get('techniques')
            ref_chroma_data = ref_features.get('chroma')
            
            # DTW 정렬을 위한 크로마 데이터 처리
            time_mapping = None
            if ref_chroma_data:
                try:
                    # 저장된 레퍼런스 크로마 데이터를 numpy 배열로 변환
                    ref_chroma = np.array(ref_chroma_data)
                    
                    # 사용자 오디오 크로마 추출
                    self.update_state(state='PROCESSING', meta={'progress': 12, 'message': 'Extracting chroma features'})
                    user_chroma = extract_chroma(user_y, sr)
                    
                    # DTW 정렬 수행
                    self.update_state(state='PROCESSING', meta={'progress': 15, 'message': 'Performing DTW alignment'})
                    
                    # 오디오 파일 길이 계산
                    user_duration = len(user_y) / sr
                    # 참조 오디오 길이는 온셋 기반으로 추정 (마지막 온셋 + 적절한 버퍼)
                    ref_duration = ref_onsets[-1] + 2.0 if ref_onsets else 0
                    
                    # 크로마 특성에 대한 시간 배열 생성
                    user_times = librosa.frames_to_time(np.arange(user_chroma.shape[1]), sr=sr, hop_length=2048)
                    ref_times = np.linspace(0, ref_duration, ref_chroma.shape[1])  # 크로마 프레임 수에 맞게 선형 시간 배열 생성
                    
                    # FastDTW로 정렬
                    from scipy.spatial.distance import euclidean
                    from fastdtw import fastdtw
                    
                    distance, path = fastdtw(user_chroma.T, ref_chroma.T, dist=euclidean)
                    
                    # DTW 경로에 기반하여 시간 매핑 생성
                    raw_time_mapping = []
                    for user_idx, ref_idx in path:
                        if user_idx < len(user_times) and ref_idx < len(ref_times):
                            user_time = user_times[user_idx]
                            ref_time = ref_times[ref_idx]
                            raw_time_mapping.append((user_time, ref_time))
                    
                    # 시간 매핑 확장 처리
                    if raw_time_mapping:
                        # 참조 시간 기준으로 정렬
                        raw_time_mapping = sorted(raw_time_mapping, key=lambda x: x[1])
                        
                        # 중복 제거
                        filtered_mapping = []
                        last_ref_time = -1
                        
                        for user_time, ref_time in raw_time_mapping:
                            if ref_time > last_ref_time:  # 단조 증가 유지
                                filtered_mapping.append((user_time, ref_time))
                                last_ref_time = ref_time
                        
                        # 선형 보간법으로 1000개 균등한 포인트 생성
                        time_mapping = []
                        ref_times_interp = np.linspace(0, ref_duration, num=1000)
                        
                        # 매핑이 충분하면 보간 수행
                        if len(filtered_mapping) > 1:
                            user_times_array = np.array([t[0] for t in filtered_mapping])
                            ref_times_array = np.array([t[1] for t in filtered_mapping])
                            
                            # numpy 보간 함수 사용
                            user_times_interp = np.interp(
                                ref_times_interp, 
                                ref_times_array,
                                user_times_array,
                                left=0,
                                right=user_duration
                            )
                            
                            for user_t, ref_t in zip(user_times_interp, ref_times_interp):
                                time_mapping.append((user_t, ref_t))
                        
                        logger.info(f"Created time mapping with DTW: {len(time_mapping)} points")
                except Exception as e:
                    logger.warning(f"DTW 정렬 중 오류 발생: {str(e)}")
                    logger.warning("간격 기반 비교로 대체합니다.")
                    time_mapping = None
            
            # 사용자 오디오 특성 추출
            self.update_state(state='PROCESSING', meta={'progress': 20})
            user_tempo = extract_tempo(user_y, sr)
            
            # MIDI 데이터가 있으면 세그먼트 생성에 활용
            if midi_data and 'notes' in midi_data:
                self.update_state(state='PROCESSING', meta={'progress': 30})
                notes = midi_data['notes']
                
                # DTW 정렬 결과가 있으면 그것을 사용
                if time_mapping:
                    user_segments, user_timestamps, user_onset_deviations = enhanced_segment_audio_with_midi_notes(
                        user_y, time_mapping, notes, sr, search_window=0.2
                    )
                else:
                    # 온셋 정렬 기반 세그먼트화
                    user_onsets = extract_onsets(user_y, sr)
                    
                    # MIDI 노트와 가장 가까운 온셋 찾기
                    midi_onsets = [note['start'] for note in notes]
                    
                    # 온셋으로 세그먼트 생성
                    user_segments = []
                    user_timestamps = []
                    user_onset_deviations = []
                    
                    for i, midi_onset in enumerate(midi_onsets):
                        if user_onsets:
                            # 가장 가까운 온셋 찾기
                            nearest_idx = min(range(len(user_onsets)), key=lambda i: abs(user_onsets[i] - midi_onset))
                            user_onset = user_onsets[nearest_idx]
                            
                            # 세그먼트 경계 설정
                            start_time = user_onset
                            if i < len(midi_onsets) - 1:
                                # 다음 MIDI 노트까지 또는 다음 감지된 온셋까지
                                next_midi = midi_onsets[i+1]
                                candidates = [onset for onset in user_onsets if onset > start_time and onset < next_midi + 0.1]
                                if candidates:
                                    end_time = min(candidates)
                                else:
                                    end_time = start_time + (next_midi - midi_onset)  # 다음 MIDI 노트 기간만큼
                            else:
                                # 마지막 노트는 1초 또는 다음 온셋까지
                                candidates = [onset for onset in user_onsets if onset > start_time]
                                if candidates:
                                    end_time = candidates[0]
                                else:
                                    end_time = start_time + 1.0
                            
                            # 오디오 세그먼트 추출
                            start_sample = int(start_time * sr)
                            end_sample = min(int(end_time * sr), len(user_y))
                            
                            if start_sample < len(user_y) and end_sample > start_sample:
                                segment = user_y[start_sample:end_sample]
                                user_segments.append(segment)
                                user_timestamps.append(start_time)
                                user_onset_deviations.append(start_time - midi_onset)
            else:
                # MIDI 없이 온셋 기반 세그먼트 생성
                self.update_state(state='PROCESSING', meta={'progress': 30})
                user_onsets = extract_onsets(user_y, sr)
                
                user_segments = []
                for i in range(len(user_onsets) - 1):
                    start = int(user_onsets[i] * sr)
                    end = int(user_onsets[i+1] * sr)
                    if start < len(user_y) and end <= len(user_y):
                        user_segments.append(user_y[start:end])
                
                # 마지막 온셋
                if user_onsets:
                    start = int(user_onsets[-1] * sr)
                    if start < len(user_y):
                        user_segments.append(user_y[start:])
                
                user_timestamps = user_onsets
                user_onset_deviations = [0] * len(user_onsets)
            
            # 세그먼트가 없으면 전체 오디오를 하나의 세그먼트로 처리
            if not user_segments:
                user_segments = [user_y]
                user_timestamps = [0]
                user_onset_deviations = [0]
            
            # 음정 추출
            self.update_state(state='PROCESSING', meta={'progress': 50})
            user_pitches = extract_pitch_with_crepe(user_segments, sr)
            
            # 연주 기법 예측
            self.update_state(state='PROCESSING', meta={'progress': 65})
            model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
            user_techniques = []
            
            if os.path.exists(model_path):
                user_techniques = predict_techniques(user_segments, model_path, sr)
            
            # 점수 계산
            self.update_state(state='PROCESSING', meta={'progress': 80})
            
            # 템포 매치 점수
            tempo_match = 100 - min(100, abs(user_tempo - ref_tempo) * 2)
            
            # 음정 매치 점수 계산
            pitch_diffs = []
            for user_pitch, ref_pitch in zip(user_pitches, ref_pitches):
                if user_pitch > 0 and ref_pitch > 0:
                    cents_diff = 1200 * np.log2(user_pitch / ref_pitch) if user_pitch > 0 and ref_pitch > 0 else 1200
                    pitch_diffs.append(min(1200, abs(cents_diff)))
            
            pitch_match = 100 - (np.mean(pitch_diffs) / 1200 * 100 if pitch_diffs else 0)
            
            # 리듬 매치 점수 계산 - DTW 없이 최선의 방법으로 진행
            # 발음 시작점 간격을 사용하여 리듬 유사성 측정 (상대적 리듬)
            user_intervals = []
            ref_intervals = []
            
            # 발음 시작점 간격 계산
            for i in range(1, len(user_onsets)):
                user_intervals.append(user_onsets[i] - user_onsets[i-1])
            
            for i in range(1, len(ref_onsets)):
                ref_intervals.append(ref_onsets[i] - ref_onsets[i-1])
            
            # 간격 비교
            interval_diffs = []
            min_len = min(len(user_intervals), len(ref_intervals))
            for i in range(min_len):
                interval_diffs.append(abs(user_intervals[i] - ref_intervals[i]))
            
            # 간격 기반 리듬 유사도 계산
            if interval_diffs:
                rhythm_match = 100 - min(100, np.mean(interval_diffs) * 100)
            else:
                # 충분한 간격이 없으면 템포만 비교
                rhythm_match = tempo_match
            
            # 간격 기반의 리듬 매치를 절대 및 상대 리듬 매치로 사용
            absolute_rhythm_match = rhythm_match
            relative_rhythm_match = rhythm_match
            
            # 템포 변동성 기반의 표현력 유사도 계산
            if len(user_intervals) > 0 and len(ref_intervals) > 0:
                user_std = np.std(user_intervals)
                ref_std = np.std(ref_intervals)
                expression_similarity = 100 - min(100, abs(user_std - ref_std) / max(ref_std, 0.1) * 100)
            else:
                expression_similarity = None
            
            # 연주 기법 매치 점수 계산
            technique_matches = []
            for user_tech, ref_tech in zip(user_techniques, ref_techniques):
                match = 100 if set(user_tech) == set(ref_tech) else 0
                technique_matches.append(match)
            
            technique_match = np.mean(technique_matches) if technique_matches else 0
            
            # 종합 점수 계산
            overall_score = 0.4 * pitch_match + 0.3 * rhythm_match + 0.2 * technique_match + 0.1 * tempo_match
            
            # 결과 정리
            self.update_state(state='FINALIZING', meta={'progress': 90})
            result = {
                "user_features": {
                    "tempo": user_tempo,
                    "onsets": user_onsets,
                    "pitches": user_pitches,
                    "techniques": user_techniques
                },
                "reference_features": {
                    "tempo": ref_tempo,
                    "onsets": ref_onsets,
                    "pitches": ref_pitches,
                    "techniques": ref_techniques
                },
                "scores": {
                    "tempo_match_percentage": tempo_match,
                    "pitch_match_percentage": pitch_match,
                    "rhythm_match_percentage": rhythm_match,
                    "technique_match_percentage": technique_match,
                    "overall_score": overall_score,
                    "rhythm_absolute_match": absolute_rhythm_match,
                    "rhythm_relative_match": relative_rhythm_match,
                    "expression_similarity": expression_similarity
                },
                "created_at": datetime.utcnow().isoformat()
            }
            
            # MIDI 데이터가 있으면 추가
            if midi_data:
                result["midi_data"] = midi_data
            
            # 노트 간격 비교 데이터 생성
            if len(user_intervals) > 0 and len(ref_intervals) > 0:
                interval_comparisons = []
                for i in range(min(len(user_intervals), len(ref_intervals))):
                    comparison = {
                        "note_index": i + 1,  # 간격은 노트 1부터 시작
                        "user_interval": user_intervals[i],
                        "reference_interval": ref_intervals[i],
                        "interval_difference": abs(user_intervals[i] - ref_intervals[i]),
                        "timing_comparison": "better" if user_intervals[i] > ref_intervals[i] * 0.95 and user_intervals[i] < ref_intervals[i] * 1.05 else 
                                            "same" if user_intervals[i] == ref_intervals[i] else "worse"
                    }
                    interval_comparisons.append(comparison)
                
                result["note_comparisons"] = interval_comparisons
            
            # 메타데이터 추가
            self.update_state(state='FINALIZING', meta={'progress': 95})
            result['metadata'] = {
                'user_id': user_id,
                'song_id': song_id,
                'task_id': self.request.id,
                'has_reference': True,
                'used_stored_reference': True,
                'has_midi': midi_bytes is not None
            }
            
            # 피드백 생성
            if generate_feedback:
                self.update_state(state='FINALIZING', meta={'progress': 97})
                logger.info(f"Generating feedback for comparison task {self.request.id}")
                
                try:
                    # GROK API를 사용하여 피드백 생성
                    feedback_generator = GrokFeedbackGenerator()
                    feedback_result = feedback_generator.generate_feedback(
                        result, 
                        is_comparison=(reference_audio_bytes is not None)
                    )
                    
                    # 피드백이 성공적으로 생성된 경우 결과에 추가
                    if 'feedback' in feedback_result:
                        result['feedback'] = feedback_result['feedback']
                        result['feedback_metadata'] = feedback_result.get('metadata', {})
                        
                        # 피드백을 MongoDB에 저장
                        feedback_result['created_at'] = datetime.utcnow().isoformat()
                        save_feedback(self.request.id, feedback_result)
                    # 오류가 발생한 경우 오류 메시지 추가
                    elif 'error' in feedback_result:
                        result['feedback_error'] = feedback_result['error']
                except Exception as e:
                    logger.exception(f"Error generating feedback: {str(e)}")
                    result['feedback_error'] = f"피드백 생성 중 오류 발생: {str(e)}"
            
            # MongoDB에 결과 저장
            self.update_state(state='FINALIZING', meta={'progress': 98})
            logger.info(f"Saving comparison results to MongoDB for task {self.request.id}")
            save_comparison_result(self.request.id, result)
            
            # 완료
            self.update_state(state='FINALIZING', meta={'progress': 99})
            logger.info(f"Audio comparison task {self.request.id} completed successfully (using stored reference)")
            return result
        
        elif reference_audio_bytes is None:
            # 참조 오디오와 song_id 모두 없는 경우 오류 발생
            raise ValueError("참조 오디오가 제공되지 않았고, song_id도 없습니다. 비교 분석을 위해서는 참조 오디오나 song_id가 필요합니다.")
        
        # 참조 오디오가 직접 제공된 경우 (기존 로직)
        logger.info(f"Using provided reference audio for comparison")
        
        # 1. 오디오 로드 (10%)
        self.update_state(state='PROCESSING', meta={'progress': 10})
        user_y, sr = load_audio_from_bytes(user_audio_bytes)
        ref_y, _ = load_audio_from_bytes(reference_audio_bytes)
        
        # 2. DTW를 사용한 오디오 정렬 (20%)
        self.update_state(state='PROCESSING', meta={'progress': 20})
        time_mapping = align_audio_with_dtw(user_y, ref_y, sr)
        
        # 3. MIDI 로드 및 세그먼트 생성 (30%)
        self.update_state(state='PROCESSING', meta={'progress': 30})
        if midi_bytes:
            notes, tempos, tempo_times = load_midi_from_bytes(midi_bytes)
            self.update_state(state='PROCESSING', meta={'progress': 35})
            user_segments, user_timestamps, user_onset_deviations = enhanced_segment_audio_with_midi_notes(
                user_y, time_mapping, notes, sr, search_window=0.2
            )
            self.update_state(state='PROCESSING', meta={'progress': 40})
            ref_segments, ref_timestamps = segment_audio_with_midi_notes(ref_y, time_mapping, notes, sr)
            
            # MIDI 오디오에서 발음 시작점 추출
            midi_onsets = [note['start'] for note in notes]
        else:
            # MIDI 없이 발음 시작점 기반 세그먼트 생성
            self.update_state(state='PROCESSING', meta={'progress': 35})
            user_onsets = extract_onsets(user_y, sr)
            ref_onsets = extract_onsets(ref_y, sr)
            
            # 세그먼트 생성
            self.update_state(state='PROCESSING', meta={'progress': 40})
            user_segments = []
            for i in range(len(user_onsets) - 1):
                start = int(user_onsets[i] * sr)
                end = int(user_onsets[i+1] * sr)
                user_segments.append(user_y[start:end])
            if user_onsets:
                start = int(user_onsets[-1] * sr)
                user_segments.append(user_y[start:])
            
            ref_segments = []
            for i in range(len(ref_onsets) - 1):
                start = int(ref_onsets[i] * sr)
                end = int(ref_onsets[i+1] * sr)
                ref_segments.append(ref_y[start:end])
            if ref_onsets:
                start = int(ref_onsets[-1] * sr)
                ref_segments.append(ref_y[start:])
            
            user_timestamps = user_onsets
            ref_timestamps = ref_onsets
            midi_onsets = None
        
        # 4. 템포 추출 (45%)
        self.update_state(state='PROCESSING', meta={'progress': 45})
        user_tempo = extract_tempo(user_y, sr)
        ref_tempo = extract_tempo(ref_y, sr)
        
        # 5. 음정 추출 (50-60%)
        self.update_state(state='PROCESSING', meta={'progress': 50})
        user_pitches = extract_pitch_with_crepe(user_segments, sr)
        self.update_state(state='PROCESSING', meta={'progress': 60})
        ref_pitches = extract_pitch_with_crepe(ref_segments, sr)
        
        # 6. 연주 기법 예측 (60-70%)
        self.update_state(state='PROCESSING', meta={'progress': 65})
        model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
        user_techniques = []
        ref_techniques = []
        
        if os.path.exists(model_path):
            user_techniques = predict_techniques(user_segments, model_path, sr)
            self.update_state(state='PROCESSING', meta={'progress': 70})
            ref_techniques = predict_techniques(ref_segments, model_path, sr)
        
        # 7. 발음 시작점 추출 및 타이밍 분석 (75%)
        self.update_state(state='PROCESSING', meta={'progress': 75})
        user_onsets = extract_onsets(user_y, sr)
        ref_onsets = extract_onsets(ref_y, sr)
        
        user_onset_differences = []
        ref_onset_differences = []
        
        if midi_bytes and midi_onsets:
            # MIDI 오디오에 대한 타이밍 차이 계산
            for user_onset in user_onsets:
                closest_midi_idx = np.argmin(np.abs(np.array(midi_onsets) - user_onset))
                user_diff = abs(user_onset - midi_onsets[closest_midi_idx])
                user_onset_differences.append(user_diff)
                
            for ref_onset in ref_onsets:
                closest_midi_idx = np.argmin(np.abs(np.array(midi_onsets) - ref_onset))
                ref_diff = abs(ref_onset - midi_onsets[closest_midi_idx])
                ref_onset_differences.append(ref_diff)
        
        # 8. 점수 계산 (80-85%)
        self.update_state(state='PROCESSING', meta={'progress': 80})
        
        # 템포 매치 점수
        tempo_match = 100 - min(100, abs(user_tempo - ref_tempo) * 2)
        
        # 음정 매치 점수 계산
        pitch_diffs = []
        for user_pitch, ref_pitch in zip(user_pitches, ref_pitches):
            if user_pitch > 0 and ref_pitch > 0:
                cents_diff = 1200 * np.log2(user_pitch / ref_pitch) if user_pitch > 0 and ref_pitch > 0 else 1200
                pitch_diffs.append(min(1200, abs(cents_diff)))
        
        pitch_match = 100 - (np.mean(pitch_diffs) / 1200 * 100 if pitch_diffs else 0)
        
        # 리듬 매치 점수 계산 - DTW 없이 최선의 방법으로 진행
        # 발음 시작점 간격을 사용하여 리듬 유사성 측정 (상대적 리듬)
        user_intervals = []
        ref_intervals = []
        
        # 발음 시작점 간격 계산
        for i in range(1, len(user_onsets)):
            user_intervals.append(user_onsets[i] - user_onsets[i-1])
        
        for i in range(1, len(ref_onsets)):
            ref_intervals.append(ref_onsets[i] - ref_onsets[i-1])
        
        # 간격 비교
        interval_diffs = []
        min_len = min(len(user_intervals), len(ref_intervals))
        for i in range(min_len):
            interval_diffs.append(abs(user_intervals[i] - ref_intervals[i]))
        
        # 간격 기반 리듬 유사도 계산
        if interval_diffs:
            rhythm_match = 100 - min(100, np.mean(interval_diffs) * 100)
        else:
            # 충분한 간격이 없으면 템포만 비교
            rhythm_match = tempo_match
        
        # 간격 기반의 리듬 매치를 절대 및 상대 리듬 매치로 사용
        absolute_rhythm_match = rhythm_match
        relative_rhythm_match = rhythm_match
        
        # 템포 변동성 기반의 표현력 유사도 계산
        if len(user_intervals) > 0 and len(ref_intervals) > 0:
            user_std = np.std(user_intervals)
            ref_std = np.std(ref_intervals)
            expression_similarity = 100 - min(100, abs(user_std - ref_std) / max(ref_std, 0.1) * 100)
        else:
            expression_similarity = None
        
        # 연주 기법 매치 점수 계산
        technique_matches = []
        for user_tech, ref_tech in zip(user_techniques, ref_techniques):
            match = 100 if set(user_tech) == set(ref_tech) else 0
            technique_matches.append(match)
        
        technique_match = np.mean(technique_matches) if technique_matches else 0
        
        # 종합 점수 계산
        overall_score = 0.4 * pitch_match + 0.3 * rhythm_match + 0.2 * technique_match + 0.1 * tempo_match
        
        # 오디오 특성 정보 생성
        features = {
            "tempo": tempo,
            "onsets": onsets.tolist() if isinstance(onsets, np.ndarray) else onsets,
            "pitches": pitches,
            "techniques": techniques,
            "created_at": datetime.utcnow().isoformat()
        }
        
        # 크로마 데이터 추출 및 저장 (DTW 정렬에 필요)
        if len(y) > 0:
            chroma = extract_chroma(y, sr)
            if isinstance(chroma, np.ndarray):
                features["chroma"] = chroma.tolist()  # numpy 배열을 리스트로 변환하여 저장
        
        # 메타데이터 추가
        metadata = {
            "song_id": song_id,
            "task_id": self.request.id,
            "has_midi": midi_bytes is not None
        }
        features["metadata"] = metadata
        
        # 8. DB에 저장 (95%)
        self.update_state(state='FINALIZING', meta={'progress': 95})
        doc_id = save_reference_features(song_id, features, midi_data)
        
        logger.info(f"레퍼런스 오디오 분석 완료 및 저장 (song_id: {song_id}, doc_id: {doc_id})")
        
        # 9. 결과 반환 (100%)
        self.update_state(state='FINALIZING', meta={'progress': 99})
        result = {
            "song_id": song_id,
            "features": features,
            "document_id": doc_id
        }
        
        if midi_data:
            result["has_midi"] = True
        
        return result
    
    except Exception as e:
        logger.exception(f"Error in audio comparison task: {str(e)}")
        raise

@celery_app.task(bind=True, name='workers.tasks.analyze_reference_audio')
def analyze_reference_audio(self, audio_bytes, song_id, midi_bytes=None, description=None):
    """
    레퍼런스 오디오를 분석하여 특성을 추출하고 DB에 저장하는 Celery 태스크
    
    Parameters:
    - audio_bytes: 레퍼런스 오디오 파일의 바이너리 콘텐츠
    - song_id: 곡 식별자 (고유 ID)
    - midi_bytes: 미디 파일의 바이너리 콘텐츠 (선택 사항)
    - description: 곡에 대한 설명 (선택 사항)
    
    Returns:
    - 추출된 특성 정보와 DB에 저장된 문서 ID
    """
    logger.info(f"레퍼런스 오디오 분석 태스크 시작 {self.request.id}, song_id: {song_id}")
    self.update_state(state='STARTED', meta={'progress': 0})
    
    try:
        # 1. 오디오 로드 (10%)
        self.update_state(state='PROCESSING', meta={'progress': 10})
        y, sr = load_audio_from_bytes(audio_bytes)
        
        # 2. 템포 추출 (20%)
        self.update_state(state='PROCESSING', meta={'progress': 20})
        tempo = extract_tempo(y, sr)
        
        # 3. 노트 시작점 추출 (30%)
        self.update_state(state='PROCESSING', meta={'progress': 30})
        onsets = extract_onsets(y, sr)
        
        # 4. 크로마 특성 추출 (DTW 정렬에 필요) (35%)
        self.update_state(state='PROCESSING', meta={'progress': 35})
        chroma = extract_chroma(y, sr)
        
        # 5. MIDI 로드 및 세그먼트 생성 (40%)
        self.update_state(state='PROCESSING', meta={'progress': 40})
        midi_data = None
        segments = []
        
        if midi_bytes:
            # MIDI 파일이 제공된 경우
            notes, tempos, tempo_times = load_midi_from_bytes(midi_bytes)
            
            # 노트 정보를 기반으로 세그먼트 생성
            for i in range(len(notes) - 1):
                start = int(notes[i]['start'] * sr)
                end = int(notes[i+1]['start'] * sr)
                if start < len(y) and end <= len(y):
                    segments.append(y[start:end])
            
            # 마지막 노트
            if notes:
                start = int(notes[-1]['start'] * sr)
                if start < len(y):
                    segments.append(y[start:])
            
            # MIDI 데이터 저장
            midi_data = {
                "notes": notes,
                "tempos": tempos,
                "tempo_times": tempo_times
            }
        else:
            # MIDI 없이 발음 시작점 기반 세그먼트 생성
            for i in range(len(onsets) - 1):
                start = int(onsets[i] * sr)
                end = int(onsets[i+1] * sr)
                if start < len(y) and end <= len(y):
                    segments.append(y[start:end])
            
            # 마지막 부분
            if onsets:
                start = int(onsets[-1] * sr)
                if start < len(y):
                    segments.append(y[start:])
        
        # 세그먼트가 없으면 전체 오디오를 하나의 세그먼트로 처리
        if not segments:
            segments = [y]
        
        # 6. 음정 추출 (60%)
        self.update_state(state='PROCESSING', meta={'progress': 60})
        pitches = extract_pitch_with_crepe(segments, sr)
        
        # 7. 연주 기법 예측 (80%)
        self.update_state(state='PROCESSING', meta={'progress': 80})
        model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
        techniques = []
        
        if os.path.exists(model_path):
            techniques = predict_techniques(segments, model_path, sr)
        
        # 8. 결과 정리 (90%)
        self.update_state(state='FINALIZING', meta={'progress': 90})
        
        # 오디오 특성 정보 생성
        features = {
            "tempo": tempo,
            "onsets": onsets.tolist() if isinstance(onsets, np.ndarray) else onsets,
            "pitches": pitches,
            "techniques": techniques,
            "created_at": datetime.utcnow().isoformat()
        }
        
        # 크로마 데이터 저장 (DTW 정렬에 필요)
        if isinstance(chroma, np.ndarray):
            features["chroma"] = chroma.tolist()  # numpy 배열을 리스트로 변환하여 저장
        
        # 메타데이터 추가
        metadata = {
            "song_id": song_id,
            "task_id": self.request.id,
            "description": description,
            "has_midi": midi_bytes is not None
        }
        features["metadata"] = metadata
        
        # 9. DB에 저장 (95%)
        self.update_state(state='FINALIZING', meta={'progress': 95})
        doc_id = save_reference_features(song_id, features, midi_data)
        
        logger.info(f"레퍼런스 오디오 분석 완료 및 저장 (song_id: {song_id}, doc_id: {doc_id})")
        
        # 10. 결과 반환 (100%)
        self.update_state(state='FINALIZING', meta={'progress': 99})
        result = {
            "song_id": song_id,
            "features": features,
            "document_id": doc_id
        }
        
        if midi_data:
            result["has_midi"] = True
        
        return result
    
    except Exception as e:
        logger.exception(f"레퍼런스 오디오 분석 오류: {str(e)}")
        raise