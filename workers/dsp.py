import os
import io
import numpy as np
import librosa
import tempfile
import pretty_midi
from sklearn.metrics import f1_score
from keras.models import load_model  # 독립 Keras 패키지 사용
from fastdtw import fastdtw
from scipy.spatial.distance import euclidean
import crepe
import scipy.signal

# SciPy 호환성 패치: 최신 버전에서는 scipy.signal.hann이 windows 모듈로 이동함
if not hasattr(scipy.signal, 'hann') and hasattr(scipy.signal, 'windows') and hasattr(scipy.signal.windows, 'hann'):
    scipy.signal.hann = scipy.signal.windows.hann


def load_audio_from_bytes(audio_bytes):
    """Load audio data from bytes."""
    with tempfile.NamedTemporaryFile(suffix='.wav', delete=True) as temp_file:
        temp_file.write(audio_bytes)
        temp_file.flush()
        y, sr = librosa.load(temp_file.name, sr=22050)
    return y, sr


def load_midi_from_bytes(midi_bytes):
    """Load MIDI data from bytes."""
    with tempfile.NamedTemporaryFile(suffix='.mid', delete=True) as temp_file:
        temp_file.write(midi_bytes)
        temp_file.flush()
        return parse_midi(temp_file.name)


def parse_midi(midi_path):
    """Parse MIDI file to extract notes, tempo and time information."""
    midi_data = pretty_midi.PrettyMIDI(midi_path)
    notes = []
    for instrument in midi_data.instruments:
        for note in instrument.notes:
            notes.append({
                'start': note.start,
                'end': note.end,
                'pitch': note.pitch,
                'velocity': note.velocity
            })
    tempo_changes = midi_data.get_tempo_changes()
    tempos = tempo_changes[1]
    tempo_times = tempo_changes[0]
    return notes, tempos, tempo_times


def extract_chroma(y, sr=22050):
    """Extract chroma features from audio."""
    chroma = librosa.feature.chroma_cqt(y=y, sr=sr, hop_length=2048)
    return chroma


def align_audio_with_dtw(user_y, orig_y, sr=22050):
    """Align user audio with reference audio using DTW."""
    user_chroma = extract_chroma(user_y, sr)
    orig_chroma = extract_chroma(orig_y, sr)
    distance, path = fastdtw(user_chroma.T, orig_chroma.T, dist=euclidean)
    
    # Calculate audio file durations
    user_duration = len(user_y) / sr
    orig_duration = len(orig_y) / sr
    
    # Create time arrays for chroma features
    user_times = librosa.frames_to_time(np.arange(user_chroma.shape[1]), sr=sr, hop_length=2048)
    orig_times = librosa.frames_to_time(np.arange(orig_chroma.shape[1]), sr=sr, hop_length=2048)
    
    # Create time mapping based on DTW path
    time_mapping = []
    for user_idx, orig_idx in path:
        user_time = user_times[user_idx] if user_idx < len(user_times) else user_duration
        orig_time = orig_times[orig_idx] if orig_idx < len(orig_times) else orig_duration
        time_mapping.append((user_time, orig_time))
    
    # Expand time mapping to cover full audio length
    if time_mapping:
        time_mapping = sorted(time_mapping, key=lambda x: x[1])
        new_time_mapping = []
        last_user_time, last_orig_time = time_mapping[0]
        new_time_mapping.append((last_user_time, last_orig_time))
        
        for user_time, orig_time in time_mapping[1:]:
            if orig_time > last_orig_time:
                new_time_mapping.append((user_time, orig_time))
                last_user_time, last_orig_time = user_time, orig_time
        
        # Linear interpolation from 0 to orig_duration
        time_mapping = []
        orig_times = np.linspace(0, orig_duration, num=1000)
        user_times = np.interp(orig_times, [t[1] for t in new_time_mapping], [t[0] for t in new_time_mapping])
        for user_time, orig_time in zip(user_times, orig_times):
            time_mapping.append((user_time, orig_time))
    
    return time_mapping


def segment_audio_with_midi_notes(y, time_mapping, notes, sr=22050):
    """Segment audio based on MIDI note events."""
    segments = []
    timestamps = []
    user_times = [user_time for user_time, _ in time_mapping]
    orig_times = [orig_time for _, orig_time in time_mapping]
    
    for note in notes:
        start_time = note['start']
        end_time = note['end']
        
        # Map original time to user time
        if start_time < orig_times[0]:
            user_start_time = user_times[0]
        elif start_time > orig_times[-1]:
            user_start_time = user_times[-1]
        else:
            # Linear interpolation to calculate user_start_time
            idx = np.searchsorted(orig_times, start_time)
            if idx == 0:
                user_start_time = user_times[0]
            elif idx == len(orig_times):
                user_start_time = user_times[-1]
            else:
                t0, t1 = orig_times[idx-1], orig_times[idx]
                u0, u1 = user_times[idx-1], user_times[idx]
                user_start_time = u0 + (start_time - t0) * (u1 - u0) / (t1 - t0)
        
        duration = end_time - start_time
        user_end_time = user_start_time + duration
        start_sample = int(user_start_time * sr)
        end_sample = int(user_end_time * sr)
        
        if start_sample >= len(y):
            continue
        
        if end_sample > len(y):
            segment = np.pad(y[start_sample:], (0, end_sample - len(y)), mode='constant')
        else:
            segment = y[start_sample:end_sample]
        
        segments.append(segment)
        timestamps.append(user_start_time)
    
    return segments, timestamps


def extract_pitch_with_crepe(segments, sr=22050):
    """Extract pitch information using CREPE model."""
    pitches = []
    for segment in segments:
        if len(segment) < sr * 0.01:  # Skip segments that are too short
            pitches.append(0)
            continue
        time, frequency, confidence, activation = crepe.predict(segment, sr, viterbi=True)
        avg_freq = np.mean(frequency[confidence > 0.5]) if np.any(confidence > 0.5) else 0
        pitches.append(avg_freq if not np.isnan(avg_freq) else 0)
    return pitches


def extract_onsets(y, sr=22050):
    """Extract onset times from audio."""
    onset_frames = librosa.onset.onset_detect(y=y, sr=sr, backtrack=True)
    onset_times = librosa.frames_to_time(onset_frames, sr=sr)
    return onset_times.tolist()


def extract_tempo(y, sr=22050):
    """Extract tempo information from audio."""
    tempo, _ = librosa.beat.beat_track(y=y, sr=sr)
    return float(tempo)


def wav_to_spectrogram(y, sr=22050, n_fft=512, hop_length=20, n_mels=128, target_time_frames=960):
    """Convert audio to mel spectrogram."""
    S = librosa.feature.melspectrogram(y=y, sr=sr, n_fft=n_fft, hop_length=hop_length, n_mels=n_mels)
    S_db = librosa.power_to_db(S, ref=np.max)
    
    # Standardize spectrogram size
    if S_db.shape[1] < target_time_frames:
        S_db = np.pad(S_db, ((0, 0), (0, target_time_frames - S_db.shape[1])), mode='constant')
    elif S_db.shape[1] > target_time_frames:
        S_db = S_db[:, :target_time_frames]
    
    return S_db


def predict_techniques(segments, model_path, sr=22050):
    """Predict guitar techniques used in audio segments."""
    techniques = ["bend", "hammer", "normal", "pull", "slide", "vibrato"]
    model = load_model(model_path)
    
    predictions = []
    for segment in segments:
        if len(segment) < sr * 0.01:
            predictions.append(["unknown"])
            continue
        
        spec = wav_to_spectrogram(segment, sr=sr)
        spec = (spec - np.min(spec)) / (np.max(spec) - np.min(spec) + 1e-8)
        spec = spec[..., np.newaxis]
        spec = np.expand_dims(spec, axis=0)
        
        pred = model.predict(spec, verbose=0)
        pred_binary = (pred > 0.5).astype(int)
        predicted_techniques = [techniques[i] for i in range(len(pred_binary[0])) if pred_binary[0][i] == 1]
        
        predictions.append(predicted_techniques if predicted_techniques else ["normal"])
    
    return predictions


def analyze_simple(audio_bytes):
    """Perform simple analysis on audio."""
    y, sr = load_audio_from_bytes(audio_bytes)
    
    # Extract basic features
    tempo = extract_tempo(y, sr)
    onsets = extract_onsets(y, sr)
    
    # Create segments based on onsets
    segments = []
    for i in range(len(onsets) - 1):
        start = int(onsets[i] * sr)
        end = int(onsets[i+1] * sr)
        segments.append(y[start:end])
    
    # Add final segment
    if onsets:
        start = int(onsets[-1] * sr)
        segments.append(y[start:])
    
    # Use model for technique prediction if available
    model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
    techniques = []
    
    if os.path.exists(model_path):
        techniques = predict_techniques(segments, model_path, sr)
    
    return {
        "tempo": tempo,
        "onsets": onsets,
        "number_of_notes": len(onsets),
        "duration": float(len(y) / sr),
        "techniques": techniques
    }


def compare_audio_with_reference(user_audio_bytes, reference_audio_bytes, midi_bytes=None):
    """Compare user audio with reference audio and MIDI if provided."""
    user_y, sr = load_audio_from_bytes(user_audio_bytes)
    ref_y, _ = load_audio_from_bytes(reference_audio_bytes)
    
    # Align audio using DTW
    time_mapping = align_audio_with_dtw(user_y, ref_y, sr)
    
    # Load MIDI if available
    if midi_bytes:
        notes, tempos, tempo_times = load_midi_from_bytes(midi_bytes)
        user_segments, user_timestamps = segment_audio_with_midi_notes(user_y, time_mapping, notes, sr)
        ref_segments, ref_timestamps = segment_audio_with_midi_notes(ref_y, time_mapping, notes, sr)
        
        # Get MIDI onsets for comparison with both performances
        midi_onsets = [note['start'] for note in notes]
    else:
        # Create segments based on onsets
        user_onsets = extract_onsets(user_y, sr)
        ref_onsets = extract_onsets(ref_y, sr)
        
        # Create segments from onsets
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
        midi_onsets = None  # No MIDI information available
    
    # Extract features
    user_tempo = extract_tempo(user_y, sr)
    ref_tempo = extract_tempo(ref_y, sr)
    
    user_pitches = extract_pitch_with_crepe(user_segments, sr)
    ref_pitches = extract_pitch_with_crepe(ref_segments, sr)
    
    # Use model for technique prediction if available
    model_path = os.path.join(os.environ.get('MODEL_DIR', 'models'), 'guitar_technique_classifier.keras')
    user_techniques = []
    ref_techniques = []
    
    if os.path.exists(model_path):
        user_techniques = predict_techniques(user_segments, model_path, sr)
        ref_techniques = predict_techniques(ref_segments, model_path, sr)
    
    # Extract onsets for timing analysis
    user_onsets = extract_onsets(user_y, sr)
    ref_onsets = extract_onsets(ref_y, sr)
    
    # Calculate timing differences against MIDI if available
    user_onset_differences = []
    ref_onset_differences = []
    
    if midi_bytes and midi_onsets:
        # Map onsets to compare with MIDI onsets
        for user_onset in user_onsets:
            # Find the closest MIDI onset
            closest_midi_idx = np.argmin(np.abs(np.array(midi_onsets) - user_onset))
            user_diff = abs(user_onset - midi_onsets[closest_midi_idx])
            user_onset_differences.append(user_diff)
            
        for ref_onset in ref_onsets:
            # Find the closest MIDI onset
            closest_midi_idx = np.argmin(np.abs(np.array(midi_onsets) - ref_onset))
            ref_diff = abs(ref_onset - midi_onsets[closest_midi_idx])
            ref_onset_differences.append(ref_diff)
    
    # Calculate scores
    tempo_match = 100 - min(100, abs(user_tempo - ref_tempo) * 2)
    
    # Calculate pitch match percentage
    pitch_diffs = []
    for user_pitch, ref_pitch in zip(user_pitches, ref_pitches):
        if user_pitch > 0 and ref_pitch > 0:
            # Calculate cents difference
            cents_diff = 1200 * np.log2(user_pitch / ref_pitch) if user_pitch > 0 and ref_pitch > 0 else 1200
            pitch_diffs.append(min(1200, abs(cents_diff)))
    
    pitch_match = 100 - (np.mean(pitch_diffs) / 1200 * 100 if pitch_diffs else 0)
    
    # Calculate rhythm match - both absolute and relative if MIDI is available
    absolute_rhythm_match = 0
    relative_rhythm_match = 0
    
    if midi_bytes and len(user_onset_differences) > 0 and len(ref_onset_differences) > 0:
        # Absolute rhythm score (compared directly to MIDI)
        absolute_rhythm_match = 100 - min(100, np.mean(user_onset_differences) * 100)
        
        # Relative rhythm score (taking into account reference performance's deviations)
        relative_diffs = []
        for user_diff, ref_diff in zip(user_onset_differences[:min(len(user_onset_differences), len(ref_onset_differences))], 
                                       ref_onset_differences[:min(len(user_onset_differences), len(ref_onset_differences))]):
            # If reference performer was off by X seconds, user is only penalized if they're worse than X
            relative_diff = max(0, user_diff - ref_diff)
            relative_diffs.append(relative_diff)
        
        relative_rhythm_match = 100 - min(100, np.mean(relative_diffs) * 100 if relative_diffs else 0)
        
        # Weighted average (60% relative, 40% absolute)
        rhythm_match = 0.4 * absolute_rhythm_match + 0.6 * relative_rhythm_match
        
        # Calculate expressiveness similarity
        ref_avg_deviation = np.mean(ref_onset_differences)
        user_avg_deviation = np.mean(user_onset_differences)
        expression_similarity = 100 - min(100, abs(ref_avg_deviation - user_avg_deviation) * 200)
    else:
        # Traditional rhythm match calculation when MIDI is not available
        rhythm_diffs = []
        min_len = min(len(user_onsets), len(ref_onsets))
        for i in range(min_len):
            rhythm_diffs.append(abs(user_onsets[i] - ref_onsets[i]))
        
        rhythm_match = 100 - min(100, np.mean(rhythm_diffs) * 100 if rhythm_diffs else 0)
        absolute_rhythm_match = rhythm_match
        relative_rhythm_match = rhythm_match
        expression_similarity = None
    
    # Calculate technique match
    technique_matches = []
    for user_tech, ref_tech in zip(user_techniques, ref_techniques):
        match = 100 if set(user_tech) == set(ref_tech) else 0
        technique_matches.append(match)
    
    technique_match = np.mean(technique_matches) if technique_matches else 0
    
    # Calculate overall score with weighted components
    overall_score = 0.4 * pitch_match + 0.3 * rhythm_match + 0.2 * technique_match + 0.1 * tempo_match
    
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
        }
    }
    
    # Add detailed rhythm analysis if MIDI was available
    if midi_bytes:
        result["scores"]["rhythm_absolute_match"] = absolute_rhythm_match
        result["scores"]["rhythm_relative_match"] = relative_rhythm_match
        
        if expression_similarity is not None:
            result["scores"]["expression_similarity"] = expression_similarity
            
        # Create note-by-note comparison data
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
    
    return result