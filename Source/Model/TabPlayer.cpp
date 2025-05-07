#include "TabPlayer.h"

// Windows API를 사용하기 위한 헤더 추가
#ifdef _WIN32
#include <windows.h>
#include <combaseapi.h>  // COM 초기화에 필요
#endif

TabPlayer::TabPlayer()
{
    DBG("Starting TabPlayer initialization...");
    
    // 벌크 VST 관련 코드 제거 (COM 초기화, 플러그인 로드 등)
    
    // 기본 템포 설정
    calculateTickSamples();
    
    DBG("TabPlayer initialization finished!");
}

void TabPlayer::setTabFile(const gp_parser::TabFile& file)
{
    try {
        tabFile = std::make_unique<gp_parser::TabFile>(file);
        
        // 탭 파일이 로드되면 재생 초기화
        currentTrack = 0;
        currentMeasure = 0;
        currentBeat = 0;
        currentTick = 0;
        
        // 템포 계산
        calculateTickSamples();
        
        // 탭 파일 정보 출력
        DBG("Tab file loaded: " + juce::String(tabFile->title));
        
        // 트랙이 비어 있는지 확인
        if (tabFile->tracks.empty()) {
            DBG("Warning: No tracks in tab file");
        } else {
            DBG("Number of tracks: " + juce::String(tabFile->tracks.size()));
            
            // 첫 번째 트랙에 마디가 있는지 확인
            if (!tabFile->tracks[0].measures.empty()) {
                DBG("First track has " + juce::String(tabFile->tracks[0].measures.size()) + " measures");
                
                // 첫 번째 마디에 비트가 있는지 확인
                if (!tabFile->tracks[0].measures[0].beats.empty()) {
                    DBG("First measure has " + juce::String(tabFile->tracks[0].measures[0].beats.size()) + " beats");
                } else {
                    DBG("First measure has no beats");
                }
            } else {
                DBG("First track has no measures");
            }
        }
        
        DBG("Tempo: " + juce::String(getBPM()) + " BPM");
    }
    catch (const std::exception& e) {
        DBG("Exception in setTabFile: " + juce::String(e.what()));
        // 예외가 발생해도 기본 상태는 유지
        currentTrack = 0;
        currentMeasure = 0;
        currentBeat = 0;
        currentTick = 0;
    }
    catch (...) {
        DBG("Unknown exception in setTabFile");
        // 예외가 발생해도 기본 상태는 유지
        currentTrack = 0;
        currentMeasure = 0;
        currentBeat = 0;
        currentTick = 0;
    }
}

void TabPlayer::startPlaying()
{
    // 재생 시작
    if (tabFile == nullptr) {
        DBG("Cannot play: No tab file loaded");
        return;
    }
    
    playing = true;
    
    // 트랙 및 모든 상태 초기화
    currentTrack = 0;
    currentMeasure = 0;
    currentBeat = 0;
    currentTick = 0;
    tickCounter = 0;
    sampleCounter = 0;
    silenceCountdown = 0;
    trackEndReached = false;
    
    // 활성 노트 초기화
    activeNotes.clear();
    
    DBG("Playback started with complete reset of state");
}

void TabPlayer::stopPlaying()
{
    playing = false;
    
    // 활성 노트 초기화
    activeNotes.clear();
    
    // 상태 초기화
    silenceCountdown = 0;
    trackEndReached = false;
    
    DBG("Playback stopped with full cleanup");
}

void TabPlayer::prepareToPlay(double sampleRate, int samplesPerBlockIn)
{
    // 오디오 설정 저장
    currentSampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlockIn;
    
    // 템포 기반 샘플 계산 업데이트
    calculateTickSamples();
    
    DBG("Preparing player: Sample rate = " + juce::String(sampleRate) + 
        ", Block size = " + juce::String(samplesPerBlockIn));
}

void TabPlayer::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // 소리 재생 대신 악보 진행만 처리
    midiMessages.clear();
    buffer.clear(); // 버퍼 초기화
    
    if (!playing || tabFile == nullptr || tabFile->tracks.empty()) {
        return;
    }
    
    const int numSamples = buffer.getNumSamples();
    
    // 활성화된 노트 관리 (소리는 재생하지 않음)
    processActiveNotes(midiMessages, numSamples);
    
    int samplePosition = 0;
    
    // 현재 버퍼에서 처리해야 할 샘플 수만큼 반복
    while (samplePosition < numSamples && !trackEndReached)
    {
        // 다음 틱까지 남은 샘플 수 계산
        int samplesToNextTick = static_cast<int>(samplesPerTick - std::fmod(sampleCounter, samplesPerTick));
        samplesToNextTick = juce::jmax(1, samplesToNextTick); // 0이나 음수가 되지 않도록 보호
        
        // 현재 버퍼의 남은 공간보다 많은 샘플이 필요하면 버퍼 끝까지만 처리
        if (samplePosition + samplesToNextTick > numSamples)
            samplesToNextTick = numSamples - samplePosition;
        
        // 샘플 카운터 업데이트
        sampleCounter += samplesToNextTick;
        
        // 다음 틱에 도달했는지 확인
        if (std::fmod(sampleCounter, samplesPerTick) < 0.001 || std::fmod(sampleCounter, samplesPerTick) > samplesPerTick - 0.001)
        {
            // 틱 카운터 증가
            tickCounter++;
            
            // 현재 트랙, 마디, 비트 확인
            if (currentTrack < static_cast<int>(tabFile->tracks.size()))
            {
                const auto& track = tabFile->tracks[currentTrack];
                
                if (currentMeasure < static_cast<int>(track.measures.size()))
                {
                    const auto& measure = track.measures[currentMeasure];
                    
                    // 현재 비트가 유효하면 처리
                    if (currentBeat < static_cast<int>(measure.beats.size()))
                    {
                        const auto& beat = measure.beats[currentBeat];
                        
                        // 현재 비트의 시작 위치가 현재 틱 위치와 일치하는지 확인
                        if (tickCounter == beat.start)
                        {
                            DBG("Processing beat at tick: " + juce::String(tickCounter));
                            processNextBeat(midiMessages, samplePosition);
                            currentBeat++;
                            
                            // 마디의 끝에 도달했는지 확인
                            if (currentBeat >= static_cast<int>(measure.beats.size()))
                            {
                                DBG("End of measure: " + juce::String(currentMeasure) + ", moving to next");
                                currentBeat = 0;
                                currentMeasure++;
                                
                                // 트랙의 끝에 도달했는지 확인
                                if (currentMeasure >= static_cast<int>(track.measures.size()))
                                {
                                    DBG("End of track: " + juce::String(currentTrack) + ", moving to next");
                                    currentMeasure = 0;
                                    currentTrack++;
                                    
                                    // 모든 트랙의 재생이 끝났는지 확인
                                    if (currentTrack >= static_cast<int>(tabFile->tracks.size()))
                                    {
                                        DBG("End of all tracks reached");
                                        // 마지막 노트가 충분히 재생될 수 있도록 지연 시간 추가
                                        silenceCountdown = static_cast<int>(currentSampleRate * 2.0); // 2초 지연
                                        
                                        // 노트 정리
                                        stopAllActiveNotes(midiMessages);
                                        
                                        // 트랙 끝 플래그 설정
                                        trackEndReached = true;
                                    }
                                    else {
                                        // 다음 트랙으로 넘어갈 때 모든 노트를 초기화
                                        stopAllActiveNotes(midiMessages);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        samplePosition += samplesToNextTick;
    }
    
    // 무음 카운트다운이 활성화되어 있으면 처리
    if (silenceCountdown > 0) {
        // 이 프레임에서 감소할 샘플 수 계산
        int samplesToDecrement = juce::jmin(silenceCountdown, numSamples);
        silenceCountdown -= samplesToDecrement;
        
        if (silenceCountdown <= 0) {
            DBG("Silence period ended, resetting playback state");
            
            // 트랙 끝 도달 후 완전한 상태 초기화
            if (trackEndReached) {
                // 재생 상태 완전히 초기화
                currentTrack = 0;
                currentMeasure = 0;
                currentBeat = 0;
                tickCounter = 0;
                sampleCounter = 0;
                
                // 안전을 위해 다시 한번 활성 노트 초기화
                activeNotes.clear();
                
                // 트랙 종료 플래그 초기화
                trackEndReached = false;
                
                DBG("Track end reached, playback state completely reset");
            }
        }
    }
    
    // 소리 재생 기능 제거 (VST 플러그인 및 내장 신디사이저 사용 안함)
}

// 사용자 정의 합성기 함수 제거 (useBuiltInSynthesizer 함수)

// 노트 관리 기능 간소화 - 실제로 소리는 재생하지 않음
void TabPlayer::stopAllActiveNotes(juce::MidiBuffer& midiMessages)
{
    DBG("Clearing all active notes: " + juce::String(activeNotes.size()) + " notes active");
    
    // 활성 노트 초기화
    activeNotes.clear();
}

void TabPlayer::processNextBeat(juce::MidiBuffer& midiMessages, int startSample)
{
    // 인덱스 범위 유효성 검사 추가
    if (currentTrack < 0 || currentTrack >= static_cast<int>(tabFile->tracks.size()))
    {
        DBG("Invalid track index in processNextBeat: " + juce::String(currentTrack));
        return;
    }
    
    if (currentMeasure < 0 || currentMeasure >= static_cast<int>(tabFile->tracks[currentTrack].measures.size()))
    {
        DBG("Invalid measure index in processNextBeat: " + juce::String(currentMeasure));
        return;
    }
    
    if (currentBeat < 0 || currentBeat >= static_cast<int>(tabFile->tracks[currentTrack].measures[currentMeasure].beats.size()))
    {
        DBG("Invalid beat index in processNextBeat: " + juce::String(currentBeat));
        return;
    }
    
    const auto& track = tabFile->tracks[currentTrack];
    const auto& measure = track.measures[currentMeasure];
    const auto& beat = measure.beats[currentBeat];
    
    DBG("Processing beat at measure " + juce::String(currentMeasure) + 
        ", beat " + juce::String(currentBeat) + 
        ", tick " + juce::String(beat.start));
    
    // 각 트랙/박자/마디 정보 로깅
    DBG("Track: " + juce::String(currentTrack) + 
        " (" + track.name + "), Measure: " + juce::String(currentMeasure) + 
        ", Beat: " + juce::String(currentBeat) + 
        ", Voices: " + juce::String(beat.voices.size()));
    
    // 현재 비트의 모든 보이스 처리 - 소리는 재생하지 않고 악보 위치만 업데이트
    for (const auto& voice : beat.voices)
    {
        // 비어있는 보이스는 건너뛰기
        if (voice.empty)
            continue;
        
        DBG("Processing voice with duration: " + juce::String(voice.duration) + 
            ", notes: " + juce::String(voice.notes.size()));
        
        // Voice의 지속 시간 가져오기
        float voiceDuration = voice.duration;
        
        // 모든 노트 처리 - 소리 재생 관련 부분 제거하고 정보만 처리
        for (const auto& note : voice.notes)
        {
            // 노트의 string 값이 유효한지 확인 (1~6 사이)
            if (note.string < 1 || note.string > 6)
            {
                DBG("Invalid string value: " + juce::String(note.string));
                continue;
            }
            
            // MIDI 노트 번호 계산
            int midiNote = stringToMidiNote(note.string, note.value);
            
            // 박자 단위의 지속 시간 계산
            float beatDuration = voiceDuration * 4.0f; // 온음표 = 4박자로 변환
            
            // 박자를 실제 틱으로 변환 (정확한 지속 시간 계산)
            int noteDuration = static_cast<int>(beatDuration * ticksPerBeat);
            
            // 최소 지속 시간 보장
            noteDuration = juce::jmax(noteDuration, ticksPerBeat / 16);
            
            // 음표가 너무 길면 최대값 제한
            noteDuration = juce::jmin(noteDuration, 2 * 4 * ticksPerBeat);
            
            // 활성 노트 목록에 추가 (소리는 재생하지 않지만 노트 상태는 추적)
            activeNotes.push_back(ActiveNote(midiNote, 1, sampleCounter, noteDuration));
            
            DBG("Tracking note - String: " + juce::String(note.string) + 
                ", Fret: " + juce::String(note.value) + 
                ", MIDI: " + juce::String(midiNote) + 
                ", Duration: " + juce::String(noteDuration) + " ticks");
        }
    }
}

void TabPlayer::processActiveNotes(juce::MidiBuffer& midiMessages, int numSamples)
{
    // 이 함수는 노트 상태를 관리합니다 (소리 재생 없음)
    std::vector<size_t> notesToRemove;
    
    for (size_t i = 0; i < activeNotes.size(); ++i)
    {
        auto& note = activeNotes[i];
        
        if (!note.isPlaying) {
            // 이미 종료된 노트는 제거 목록에 추가
            notesToRemove.push_back(i);
            continue;
        }
        
        // 노트가 끝났는지 확인
        int elapsedTicks = static_cast<int>((sampleCounter - note.startSample) / samplesPerTick);
        
        if (elapsedTicks >= note.duration)
        {
            note.isPlaying = false;
            
            // 제거할 노트로 표시
            notesToRemove.push_back(i);
            
            DBG("Note end - MIDI: " + juce::String(note.midiNote) + 
                ", Duration: " + juce::String(note.duration) + 
                " ticks, Elapsed: " + juce::String(elapsedTicks) + " ticks");
        }
    }
    
    // 끝난 노트는 뒤에서부터 제거 (인덱스 무결성 유지)
    for (int i = static_cast<int>(notesToRemove.size()) - 1; i >= 0; --i)
    {
        size_t indexToRemove = notesToRemove[i];
        if (indexToRemove < activeNotes.size()) // 안전 검사
        {
            activeNotes.erase(activeNotes.begin() + indexToRemove);
        }
    }
}

double TabPlayer::getBPM() const
{
    // GP5 파일에서 템포 가져오기
    if (tabFile)
        return tabFile->tempoValue;
    
    // 기본 템포
    return 120.0;
}

void TabPlayer::calculateTickSamples()
{
    // 분당 박자 수를 계산
    double bpm = getBPM();
    
    // 틱당 샘플 수 계산: (샘플레이트 * 60 초) / (BPM * 틱갯수)
    samplesPerTick = static_cast<int>((currentSampleRate * 60.0) / (bpm * ticksPerBeat));
    
    DBG("Calculated samples per tick: " + juce::String(samplesPerTick) + 
        " (BPM: " + juce::String(bpm) + ")");
}

void TabPlayer::releaseResources()
{
    DBG("Releasing TabPlayer resources");
    stopPlaying();
}

int TabPlayer::stringToMidiNote(int string, int fret)
{
    // 기타 현 번호 순서를 올바르게 수정 (1번 = 가장 높은 E4, 6번 = 가장 낮은 E2)
    static const int openStrings[] = { 64, 59, 55, 50, 45, 40 }; // E4, B3, G3, D3, A2, E2
    return openStrings[string - 1] + fret;
}

void TabPlayer::setPlaybackPosition(int track, int measure, int beat)
{
    if (!tabFile || track >= static_cast<int>(tabFile->tracks.size()) ||
        measure >= static_cast<int>(tabFile->tracks[track].measures.size()) ||
        beat >= static_cast<int>(tabFile->tracks[track].measures[measure].beats.size()))
    {
        DBG("Invalid playback position: Track " + juce::String(track) +
            ", Measure " + juce::String(measure) + ", Beat " + juce::String(beat));
        return;
    }

    bool wasPlaying = playing;
    if (playing)
        stopPlaying();

    currentTrack = track;
    currentMeasure = measure;
    currentBeat = beat;
    tickCounter = tabFile->tracks[track].measures[measure].beats[beat].start;
    sampleCounter = tickCounter * samplesPerTick;
    activeNotes.clear();

    DBG("Playback position set to Track " + juce::String(track) +
        ", Measure " + juce::String(measure) + ", Beat " + juce::String(beat));

    if (wasPlaying)
        startPlaying();
}

void TabPlayer::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                            int numInputChannels,
                                            float* const* outputChannelData,
                                            int numOutputChannels,
                                            int numSamples,
                                            const juce::AudioIODeviceCallbackContext& context)
{
    // 소리 재생 기능 제거 - 출력 버퍼만 초기화
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        juce::FloatVectorOperations::clear(outputChannelData[channel], numSamples);
    }
    
    // 악보 상태 업데이트만 처리
    if (playing && tabFile != nullptr)
    {
        // MIDI 메시지와 오디오 버퍼 생성 (실제 출력 없음)
        juce::MidiBuffer midiBuffer;
        juce::AudioBuffer<float> tempBuffer(numOutputChannels, numSamples);
        tempBuffer.clear();
        
        // 악보 위치 업데이트
        processBlock(tempBuffer, midiBuffer);
    }
}

void TabPlayer::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    // 오디오 장치가 시작되기 전에 호출됩니다
    double sampleRate = device->getCurrentSampleRate();
    int samplesPerBlock = device->getCurrentBufferSizeSamples();
    
    // AudioProcessor의 prepareToPlay 메서드 활용
    prepareToPlay(sampleRate, samplesPerBlock);
    
    DBG("TabPlayer::audioDeviceAboutToStart - Sample rate: " + juce::String(sampleRate) + 
        ", Buffer size: " + juce::String(samplesPerBlock));
}

void TabPlayer::audioDeviceStopped()
{
    // 오디오 장치가 정지될 때 호출됩니다
    if (playing)
    {
        stopPlaying();
    }
    
    // AudioProcessor의 releaseResources 메서드 활용
    releaseResources();
    
    DBG("TabPlayer::audioDeviceStopped");
}