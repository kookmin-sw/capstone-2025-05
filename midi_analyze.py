import mido
import matplotlib.pyplot as plt

# MIDI 파일 로드
def load_midi(file_path):
    midi = mido.MidiFile(file_path)
    events = []
    current_time = 0

    for track in midi.tracks:
        for msg in track:
            current_time += msg.time
            if msg.type == 'note_on' and msg.velocity > 0:
                events.append({
                    'time': current_time,
                    'note': msg.note,
                    'velocity': msg.velocity
                })
    return events

# MIDI 노트 -> 음계 이름 변환
def note_to_name(note_number):
    note_names = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
    octave = (note_number // 12) - 1
    note = note_number % 12
    return f"{note_names[note]}{octave}"

# MIDI 데이터를 시각화
def visualize_midi(events):
    times = [event['time'] for event in events]
    notes = [event['note'] for event in events]
    velocities = [event['velocity'] for event in events]

    note_names = [note_to_name(note) for note in notes]

    # 그래프 생성
    plt.figure(figsize=(12, 6))

    # 노트의 시간 변화
    plt.scatter(times, notes, c=velocities, cmap='viridis', s=50, label='Notes')
    plt.colorbar(label='Velocity')
    plt.title('MIDI Note Events')
    plt.xlabel('Time (ticks)')
    plt.ylabel('Note (MIDI Number)')
    plt.yticks(notes, note_names)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

# 테스트 실행
if __name__ == "__main__":
    midi_file = "audio/26.mid"  # MIDI 파일 경로
    events = load_midi(midi_file)
    print("Loaded MIDI Events:")
    for event in events[:10]:  # 첫 10개 이벤트 출력
        print(event)
    visualize_midi(events)
