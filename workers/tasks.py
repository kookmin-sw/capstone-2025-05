import os
import tempfile
import numpy as np
import scipy.signal  # SciPy 패치를 위해 추가
from celery import Celery
from celery.utils.log import get_task_logger
import json
from datetime import datetime

# SciPy 호환성 패치 적용
# 최신 버전의 SciPy에서는 scipy.signal.hann이 scipy.signal.windows.hann으로 이동했습니다
if not hasattr(scipy.signal, 'hann') and hasattr(scipy.signal, 'windows') and hasattr(scipy.signal.windows, 'hann'):
    print("패치 적용 (tasks): scipy.signal.hann -> scipy.signal.windows.hann")
    scipy.signal.hann = scipy.signal.windows.hann

from workers.dsp import analyze_simple, compare_audio_with_reference
# dsp 모듈의 개별 함수들을 직접 가져와서 진행 상황을 추적할 수 있도록 합니다
from workers.dsp import (
    load_audio_from_bytes, load_midi_from_bytes, 
    extract_tempo, extract_onsets, extract_pitch_with_crepe, extract_pitch_with_pyin,
    predict_techniques, align_audio_with_dtw, align_audio_with_chromas, segment_audio_with_midi_notes,
    extract_chroma
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
def compare_audio(self, user_audio_bytes, user_id=None, song_id=None, generate_feedback=False):
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
    ref_features = get_reference_features(song_id)
    self.update_state(state='STARTED', meta={'progress': 5})
    
    # 참조 오디오와 비교
    
    # 1. 오디오 로드 (10%)
    self.update_state(state='PROCESSING', meta={'progress': 10})
    user_y, sr = load_audio_from_bytes(user_audio_bytes)
    # ref_y, _ = load_audio_from_bytes(reference_audio_bytes)
    
    # 2. DTW를 사용한 오디오 정렬 (20%)
    self.update_state(state='PROCESSING', meta={'progress': 20})
    # time_mapping = align_audio_with_dtw(user_y, ref_y, sr)
    user_chroma = extract_chroma(user_y, sr)
    
    # ref_features에서 chroma 데이터 추출
    if 'features' in ref_features and 'chroma' in ref_features['features']:
        ref_chroma_data = ref_features['features']['chroma']
        # 이미 numpy 배열인지 확인하고, 아니면 변환
        ref_chroma = ref_chroma_data if isinstance(ref_chroma_data, np.ndarray) else np.array(ref_chroma_data)
    else:
        logger.error(f"Reference features for song_id {song_id} does not contain chroma data")
        raise ValueError(f"Reference features missing chroma data for song_id: {song_id}")
        
    time_mapping = align_audio_with_chromas(user_chroma, ref_chroma, sr)
    
    # 3. MIDI 로드 및 세그먼트 생성 (30%)
    self.update_state(state='PROCESSING', meta={'progress': 30})
    midi_data = None
    midi_onsets = None
    has_midi = False
    
    if 'features' in ref_features and 'midi_data' in ref_features['features']:
        midi_data = ref_features['features']['midi_data']
        # MIDI 데이터가 있으면 처리
        if midi_data and isinstance(midi_data, dict) and 'notes' in midi_data:
            has_midi = True
            notes = midi_data.get('notes', [])
            tempos = midi_data.get('tempos', [])
            tempo_times = midi_data.get('tempo_times', [])
            
            self.update_state(state='PROCESSING', meta={'progress': 35})
            user_segments, user_timestamps = segment_audio_with_midi_notes(user_y, time_mapping, notes, sr)
            self.update_state(state='PROCESSING', meta={'progress': 40})
            
            # MIDI 오디오에서 발음 시작점 추출
            midi_onsets = [note['start'] for note in notes]
    else:
        # MIDI 없이 발음 시작점 기반 세그먼트 생성
        self.update_state(state='PROCESSING', meta={'progress': 35})
        user_onsets = extract_onsets(user_y, sr)
        ref_onsets = ref_features['features']['onsets']
        
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
        
        user_timestamps = user_onsets
    
    # 4. 템포 추출 (45%)
    self.update_state(state='PROCESSING', meta={'progress': 45})
    user_tempo = extract_tempo(user_y, sr)
    # ref_tempo = extract_tempo(ref_y, sr)
    ref_tempo = ref_features['features']['tempo']
    
    # 5. 음정 추출 (50-60%)
    self.update_state(state='PROCESSING', meta={'progress': 50})
    # user_pitches = extract_pitch_with_crepe(user_segments, sr)
    user_pitches = extract_pitch_with_pyin(user_segments, sr)
    self.update_state(state='PROCESSING', meta={'progress': 60})
    # ref_pitches = extract_pitch_with_crepe(ref_segments, sr)
    ref_pitches = ref_features['features']['pitches']
    
    # 6. 연주 기법 예측 (60-70%)
    self.update_state(state='PROCESSING', meta={'progress': 65})
    model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
    user_techniques = []
    ref_techniques = []
    
    if os.path.exists(model_path):
        user_techniques = predict_techniques(user_segments, model_path, sr)
        self.update_state(state='PROCESSING', meta={'progress': 70})
        # ref_techniques = predict_techniques(ref_segments, model_path, sr)
        ref_techniques = ref_features['features']['techniques']
    
    # 7. 발음 시작점 추출 및 타이밍 분석 (75%)
    self.update_state(state='PROCESSING', meta={'progress': 75})
    user_onsets = extract_onsets(user_y, sr)
    ref_onsets = ref_features['features']['onsets']
    
    user_onset_differences = []
    ref_onset_differences = []
    
    if has_midi and midi_onsets:
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
    
    # 리듬 매치 점수 계산
    absolute_rhythm_match = 0
    relative_rhythm_match = 0
    rhythm_match = 0  # 기본값 설정
    
    self.update_state(state='PROCESSING', meta={'progress': 85})
    
    # MIDI 기반 또는 기본 리듬 점수 계산
    if has_midi and midi_onsets and len(user_onset_differences) > 0 and len(ref_onset_differences) > 0:
        logger.info(f"MIDI 기반 리듬 점수 계산: user_onset_diffs={len(user_onset_differences)}, ref_onset_diffs={len(ref_onset_differences)}")
        # MIDI 기반 절대적 리듬 점수
        absolute_rhythm_match = 100 - min(100, np.mean(user_onset_differences) * 100)
        
        # 상대적 리듬 점수
        relative_diffs = []
        for user_diff, ref_diff in zip(user_onset_differences[:min(len(user_onset_differences), len(ref_onset_differences))], 
                                    ref_onset_differences[:min(len(user_onset_differences), len(ref_onset_differences))]):
            relative_diff = max(0, user_diff - ref_diff)
            relative_diffs.append(relative_diff)
        
        relative_rhythm_match = 100 - min(100, np.mean(relative_diffs) * 100 if relative_diffs else 0)
        
        # 가중 평균 (60% 상대적, 40% 절대적)
        rhythm_match = 0.4 * absolute_rhythm_match + 0.6 * relative_rhythm_match
        
        # 표현력 유사도 계산
        ref_avg_deviation = np.mean(ref_onset_differences)
        user_avg_deviation = np.mean(user_onset_differences)
        expression_similarity = 100 - min(100, abs(ref_avg_deviation - user_avg_deviation) * 200)
        
        logger.info(f"MIDI 기반 리듬 점수: absolute={absolute_rhythm_match}, relative={relative_rhythm_match}, overall={rhythm_match}")
    else:
        logger.info("기본 리듬 점수 계산 사용")
        # MIDI 없이 전통적인 리듬 점수 계산
        rhythm_diffs = []
        min_len = min(len(user_onsets), len(ref_onsets))
        
        if min_len > 0:
            for i in range(min_len):
                rhythm_diffs.append(abs(user_onsets[i] - ref_onsets[i]))
            
            rhythm_match = 100 - min(100, np.mean(rhythm_diffs) * 100 if rhythm_diffs else 0)
            absolute_rhythm_match = rhythm_match
            relative_rhythm_match = rhythm_match
            logger.info(f"기본 리듬 점수: rhythm_match={rhythm_match}, diffs_count={len(rhythm_diffs)}")
        else:
            logger.warning("리듬 비교를 위한 onset이 없습니다.")
            rhythm_match = 0
            absolute_rhythm_match = 0
            relative_rhythm_match = 0
        
        expression_similarity = None
    
    # 연주 기법 매치 점수 계산
    technique_matches = []
    for user_tech, ref_tech in zip(user_techniques, ref_techniques):
        match = 100 if set(user_tech) == set(ref_tech) else 0
        technique_matches.append(match)
    
    technique_match = np.mean(technique_matches) if technique_matches else 0
    
    # 종합 점수 계산
    overall_score = 0.4 * pitch_match + 0.3 * rhythm_match + 0.2 * technique_match + 0.1 * tempo_match
    
    # 9. 결과 정리 (90%)
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
            "overall_score": overall_score
        },
        "created_at": datetime.utcnow().isoformat()
    }
    
    # MIDI 관련 상세 리듬 분석 추가
    if has_midi and midi_onsets:
        result["scores"]["rhythm_absolute_match"] = absolute_rhythm_match
        result["scores"]["rhythm_relative_match"] = relative_rhythm_match
        
        if expression_similarity is not None:
            result["scores"]["expression_similarity"] = expression_similarity
            
        # 노트별 비교 데이터 생성
        note_comparisons = []
        for i in range(min(len(user_onset_differences), len(ref_onset_differences))):
            comparison = {
                "note_index": i,
                "user_timing_deviation": user_onset_differences[i],
                "reference_timing_deviation": ref_onset_differences[i],
                "timing_comparison": "better" if user_onset_differences[i] < ref_onset_differences[i] else 
                                    "same" if user_onset_differences[i] == ref_onset_differences[i] else "worse"
            }
            note_comparisons.append(comparison)
            
        result["note_comparisons"] = note_comparisons

    # 메타데이터 추가
    result['metadata'] = {
        'user_id': user_id,
        'song_id': song_id,
        'task_id': self.request.id,
        'has_reference': True,  # 항상 True로 설정 (ref_features에서 가져왔으므로)
        'has_midi': has_midi
    }
    
    # 피드백 생성 옵션이 활성화된 경우
    if generate_feedback:
        self.update_state(state='FINALIZING', meta={'progress': 97})
        logger.info(f"Generating feedback for comparison task {self.request.id}")
        
        try:
            # GROK API를 사용하여 피드백 생성
            feedback_generator = GrokFeedbackGenerator()
            feedback_result = feedback_generator.generate_feedback(
                result, 
                is_comparison=True  # 항상 True로 설정 (레퍼런스 비교이므로)
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
    
    # MongoDB에 결과 저장 (98%)
    self.update_state(state='FINALIZING', meta={'progress': 98})
    logger.info(f"Saving comparison results to MongoDB for task {self.request.id}")
    save_comparison_result(self.request.id, result)
    
    # 100% 진행
    self.update_state(state='FINALIZING', meta={'progress': 99})
    
    logger.info(f"Audio comparison task {self.request.id} completed successfully")
    return result

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
        
        # 4. MIDI 로드 및 세그먼트 생성 (40%)
        self.update_state(state='PROCESSING', meta={'progress': 40})
        midi_data = None
        segments = []
        
        if midi_bytes:
            # MIDI 파일이 제공된 경우
            try:
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
            except Exception as e:
                logger.warning(f"MIDI 파일 로드 중 오류 발생: {str(e)}")
                # MIDI 오류 시 발음 시작점 기반으로 세그먼트 생성
                segments = []
        
        # MIDI 오류나 MIDI 파일이 없는 경우 발음 시작점 기반 세그먼트 생성
        if not segments:
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
        
        # 5. 음정 추출 (60%)
        self.update_state(state='PROCESSING', meta={'progress': 60})
        pitches = []
        try:
            # pitches = extract_pitch_with_crepe(segments, sr)
            pitches = extract_pitch_with_pyin(segments, sr)
        except Exception as e:
            logger.error(f"음정 추출 중 오류 발생: {str(e)}")
            # 오류 발생 시 빈 값 할당
            pitches = [0.0] * len(segments)
        
        # 6. 연주 기법 예측 (80%)
        self.update_state(state='PROCESSING', meta={'progress': 80})
        model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
        techniques = []
        
        if os.path.exists(model_path):
            try:
                techniques = predict_techniques(segments, model_path, sr)
            except Exception as e:
                logger.error(f"기법 예측 중 오류 발생: {str(e)}")
                # 오류 발생 시 빈 값 할당
                techniques = [[] for _ in range(len(segments))]
        
        # 7. 결과 정리 (90%)
        self.update_state(state='FINALIZING', meta={'progress': 90})
        
        # 오디오 특성 정보 생성 (NumPy 배열을 Python 기본 타입으로 변환)
        features = {
            "tempo": float(tempo) if isinstance(tempo, (np.ndarray, np.generic)) else tempo,
            "onsets": onsets.tolist() if isinstance(onsets, np.ndarray) else onsets,
            "pitches": [float(p) if isinstance(p, (np.ndarray, np.generic)) else p for p in pitches],
            "techniques": techniques,
            "created_at": datetime.utcnow().isoformat()
        }
        
        # 크로마 특성 추출 및 저장
        try:
            chroma = extract_chroma(y, sr)
            # 크로마그램은 2D 넘파이 배열이므로 리스트로 변환하여 저장
            features["chroma"] = chroma.tolist() if isinstance(chroma, np.ndarray) else chroma
        except Exception as e:
            logger.error(f"크로마 추출 중 오류 발생: {str(e)}")
            # 오류 발생 시 빈 2D 배열 저장
            features["chroma"] = [[0.0] * 12 for _ in range(10)]
        
        # MIDI 데이터가 있으면 features에 직접 추가
        if midi_data:
            features["midi_data"] = midi_data
        
        # 메타데이터 추가
        metadata = {
            "song_id": song_id,
            "task_id": self.request.id,
            "description": description,
            "has_midi": midi_bytes is not None
        }
        features["metadata"] = metadata
        
        # 8. DB에 저장 (95%)
        self.update_state(state='FINALIZING', meta={'progress': 95})
        
        # MongoDB 저장 전에 NumPy 배열을 Python 기본 타입으로 완전히 변환
        def convert_numpy_to_python(obj):
            if isinstance(obj, np.ndarray):
                return obj.tolist()
            elif isinstance(obj, np.generic):
                return obj.item()
            elif isinstance(obj, dict):
                return {k: convert_numpy_to_python(v) for k, v in obj.items()}
            elif isinstance(obj, list) or isinstance(obj, tuple):
                return [convert_numpy_to_python(item) for item in obj]
            else:
                return obj
                
        # 모든 features의 중첩된 데이터를 완전히 변환
        features = convert_numpy_to_python(features)
        
        # midi_data를 features에 포함시켰으므로 별도로 전달하지 않음
        doc_id = save_reference_features(song_id, features, None)
        
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
        logger.exception(f"레퍼런스 오디오 분석 오류: {str(e)}")
        # 예외 발생 시 예외를 Celery에 다시 던져 작업 실패로 표시
        raise