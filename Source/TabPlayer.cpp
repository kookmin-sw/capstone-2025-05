#include "TabPlayer.h"

// Windows API를 사용하기 위한 헤더 추가
#ifdef _WIN32
#include <windows.h>
#include <combaseapi.h>  // COM 초기화에 필요
#endif

TabPlayer::TabPlayer()
{
    DBG("Starting TabPlayer initialization...");
    
    // #ifdef _WIN32
    // // COM 초기화 (VST 플러그인은 COM에 의존함)
    // HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    // if (SUCCEEDED(hr))
    //     DBG("COM initialized successfully");
    // else
    //     DBG("COM initialization failed: " + juce::String(static_cast<int>(hr)));
    // #endif
    
    formatManager.addDefaultFormats();
    // formatManager.addFormat(new juce::VSTPluginFormat());  // VST2 명시적 추가

    DBG("Registered " + juce::String(formatManager.getNumFormats()) + " plugin formats");
    
    // 다양한 경로에서 플러그인 파일 시도
    juce::StringArray possiblePaths;
    possiblePaths.add("C:/Program Files/Steinberg/VSTPlugins/AGLP.vst3");
    possiblePaths.add("C:/Program Files/VSTPlugins/AGLP.vst3");
    possiblePaths.add("C:/Program Files/Common Files/VST2/AGLP.vst3");
    possiblePaths.add("C:/Program Files/Common Files/VST3/AGLP.vst3");
    possiblePaths.add("C:/Program Files (x86)/Steinberg/VSTPlugins/AGLP.vst3");
    possiblePaths.add("C:/Program Files (x86)/VSTPlugins/AGLP.vst3");
    
    juce::File pluginFile;
    bool foundValidPath = false;
    
    for (const auto& path : possiblePaths)
    {
        juce::File testFile(path);
        if (testFile.exists())
        {
            pluginFile = testFile;
            foundValidPath = true;
            DBG("Found plugin at: " + pluginFile.getFullPathName());
            break;
        }
    }
    
    if (!foundValidPath)
    {
        DBG("Plugin file not found in any of the common paths. Using built-in audio generator instead.");
        return;
    }
    
    // 모든 등록된 플러그인 포맷에 대해 시도
    for (int i = 0; i < formatManager.getNumFormats(); ++i)
    {
        juce::AudioPluginFormat* format = formatManager.getFormat(i);
        juce::String formatName = format->getName();
        DBG("Trying plugin format: " + formatName);
        
        try {
            juce::OwnedArray<juce::PluginDescription> descriptions;
            format->findAllTypesForFile(descriptions, pluginFile.getFullPathName());
            
            if (!descriptions.isEmpty())
            {
                DBG("Found " + juce::String(descriptions.size()) + " plugin descriptions for format: " + formatName);
                
                for (auto* desc : descriptions)
                {
                    DBG("Attempting to load plugin: " + desc->name + " (" + desc->pluginFormatName + ")");
                    
                    try {
                        // 플러그인 인스턴스 생성
                        juce::String errorMsg;
                        plugin = formatManager.createPluginInstance(
                            *desc,
                            44100.0,  // 샘플레이트
                            512,      // 버퍼 크기
                            errorMsg
                        );
                        
                        if (plugin != nullptr)
                        {
                            DBG("Successfully loaded plugin: " + plugin->getName());
                            plugin->prepareToPlay(44100.0, 512);
                            useVST = true;  // VST 사용 플래그 설정
                            return;  // 성공적으로 로드되면 함수 종료
                        }
                        else
                        {
                            DBG("Failed to load plugin: " + errorMsg);
                            error = errorMsg;  // 에러 메시지 저장
                        }
                    }
                    catch (const std::exception& e)
                    {
                        DBG("Exception while creating plugin instance: " + juce::String(e.what()));
                    }
                    catch (...)
                    {
                        DBG("Unknown exception while creating plugin instance");
                    }
                }
            }
            else
            {
                DBG("No plugin descriptions found for format: " + formatName);
            }
        }
        catch (const std::exception& e)
        {
            DBG("Exception while finding plugin types: " + juce::String(e.what()));
        }
        catch (...)
        {
            DBG("Unknown exception while finding plugin types");
        }
    }
    
    // VST 로드 실패 시 로그
    if (!useVST)
    {
        DBG("Using built-in audio generator instead of VST plugin.");
    }
    
    DBG("TabPlayer initialization finished!");
}

void TabPlayer::setTabFile(const gp_parser::TabFile& file)
{
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
    DBG("Number of tracks: " + juce::String(tabFile->tracks.size()));
    DBG("Number of measures: " + juce::String(tabFile->measures));
    DBG("Tempo: " + juce::String(getBPM()) + " BPM");
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
    
    // 모든 활성 노트를 확실히 비우기
    activeNotes.clear();
    
    DBG("Playback started with complete reset of state");
}

void TabPlayer::stopPlaying()
{
    playing = false;
    
    // 모든 활성 노트 중지 및 초기화
    for (auto& note : activeNotes) {
        note.isPlaying = false;
    }
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
    midiMessages.clear();
    buffer.clear(); // 버퍼 초기화
    
    if (!playing || tabFile == nullptr || tabFile->tracks.empty()) {
        return;
    }
    
    const int numSamples = buffer.getNumSamples();
    
    // 활성화된 노트 처리 - 기존 코드 대신 더 안정적인 구현
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
                                        
                                        // 노트가 충돌하지 않도록 즉시 모든 노트 정리
                                        stopAllActiveNotes(midiMessages);
                                        
                                        // 트랙 끝 플래그 설정
                                        trackEndReached = true;
                                    }
                                    else {
                                        // 다음 트랙으로 넘어갈 때 모든 노트를 초기화 - 중요!
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
        
        // 무음 기간 동안 새 노트가 트리거되지 않도록 함
        
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
    
    // VST 플러그인이 로드되어 있으면 사용
    if (useVST && plugin != nullptr)
    {
        try {
            // VST 플러그인을 통해 오디오 처리
            plugin->processBlock(buffer, midiMessages);
        }
        catch (const std::exception& e) {
            DBG("Error in plugin processing: " + juce::String(e.what()));
            buffer.clear();
            useBuiltInSynthesizer(buffer);
        }
    }
    else
    {
        useBuiltInSynthesizer(buffer);
    }
}

// 사용자 정의 합성기 함수 추가
void TabPlayer::useBuiltInSynthesizer(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    
    // 각 채널의 오디오 데이터 처리
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        // 먼저 버퍼 초기화
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = 0.0f;
        }
        
        // 각 활성 노트별로 오디오 데이터 생성
        for (const auto& note : activeNotes)
        {
            if (note.isPlaying)
            {
                // 노트 주파수 계산
                float frequency = 440.0f * std::pow(2.0f, (note.midiNote - 69) / 12.0f);
                
                // 노트 지속 시간 기반 엔벨로프 생성
                int startSample = note.startSample;
                int noteAgeSamples = sampleCounter - startSample;
                int noteDurationSamples = note.duration * samplesPerTick;
                
                // 각 샘플에 대해 사운드 생성
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    // 현재 샘플의 절대 시간 위치
                    float time = static_cast<float>(sampleCounter - numSamples + sample) / currentSampleRate;
                    
                    // 현재 샘플에서의 노트 나이
                    int currentSampleAge = noteAgeSamples + sample;
                    
                    // ADSR 엔벨로프 적용 (Attack, Decay, Sustain, Release)
                    float envelope = 1.0f;
                    
                    // 어택 단계 (시작 부분)
                    int attackTime = juce::jmin(100, noteDurationSamples / 10);
                    if (currentSampleAge < attackTime)
                    {
                        envelope = static_cast<float>(currentSampleAge) / attackTime;
                    }
                    // 릴리즈 단계 (끝 부분)
                    else if (currentSampleAge > noteDurationSamples * 0.7f)
                    {
                        float releasePhase = (currentSampleAge - noteDurationSamples * 0.7f) 
                                            / (noteDurationSamples * 0.3f);
                        envelope = 1.0f - releasePhase;
                        envelope = juce::jmax(0.0f, envelope);
                    }
                    
                    // 기본 사인파
                    float sinValue = std::sin(2.0f * juce::float_Pi * frequency * time);
                    
                    // 기타 음색을 위한 고조파 추가 (더 자연스러운 소리)
                    float harmonics = 0.0f;
                    harmonics += 0.5f * std::sin(2.0f * 2.0f * juce::float_Pi * frequency * time); // 2차 고조파
                    harmonics += 0.3f * std::sin(3.0f * 2.0f * juce::float_Pi * frequency * time); // 3차 고조파
                    harmonics += 0.15f * std::sin(4.0f * 2.0f * juce::float_Pi * frequency * time); // 4차 고조파
                    
                    // 기본음과 고조파 믹스
                    float finalSound = (0.7f * sinValue + 0.3f * harmonics) * envelope;
                    
                    // 모든 노트가 함께 재생될 때 볼륨을 조절하기 위한 계수
                    float volumeScale = 0.2f / juce::jmax(1.0f, static_cast<float>(activeNotes.size()) * 0.5f);
                    
                    // 최종 출력 값을 버퍼에 추가
                    channelData[sample] += finalSound * volumeScale;
                }
            }
        }
        
        // 클리핑 방지 및 추가 노이즈 제거
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // 소프트 클리핑 적용
            channelData[sample] = juce::jlimit(-0.9f, 0.9f, channelData[sample]);
            
            // 값이 매우 작으면 노이즈 방지를 위해 0으로 설정
            if (std::abs(channelData[sample]) < 0.001f)
                channelData[sample] = 0.0f;
        }
    }
}

// 트랙 변경 시 모든 활성 노트를 확실히 종료하도록 개선
void TabPlayer::stopAllActiveNotes(juce::MidiBuffer& midiMessages)
{
    DBG("Stopping ALL active notes: " + juce::String(activeNotes.size()) + " notes active");
    
    // 모든 활성 노트에 대해 Note Off 이벤트 생성
    for (auto& note : activeNotes)
    {
        if (note.isPlaying)
        {
            midiMessages.addEvent(juce::MidiMessage::noteOff(note.channel, note.midiNote), 0);
            note.isPlaying = false;
            
            DBG("Stopping note: " + juce::String(note.midiNote));
        }
    }
    
    // 모든 활성 노트 제거 - 초기화
    activeNotes.clear();
    
    // VST 플러그인을 사용 중이라면 모든 노트 초기화 추가 조치
    if (useVST && plugin != nullptr)
    {
        try {
            for (int channel = 1; channel <= 16; ++channel)
            {
                midiMessages.addEvent(juce::MidiMessage::allNotesOff(channel), 0);
                midiMessages.addEvent(juce::MidiMessage::allSoundOff(channel), 0);
                midiMessages.addEvent(juce::MidiMessage::controllerEvent(channel, 123, 0), 0);
            }
        }
        catch (const std::exception& e) {
            DBG("Error stopping notes in plugin: " + juce::String(e.what()));
        }
    }
}

void TabPlayer::processNextBeat(juce::MidiBuffer& midiMessages, int startSample)
{
    if (currentTrack >= static_cast<int>(tabFile->tracks.size()) ||
        currentMeasure >= static_cast<int>(tabFile->tracks[currentTrack].measures.size()) ||
        currentBeat >= static_cast<int>(tabFile->tracks[currentTrack].measures[currentMeasure].beats.size()))
        return;
    
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
    
    // 현재 비트의 모든 보이스 처리
    for (const auto& voice : beat.voices)
    {
        // 비어있는 보이스는 건너뛰기
        if (voice.empty)
            continue;
        
        DBG("Processing voice with duration: " + juce::String(voice.duration) + 
            ", notes: " + juce::String(voice.notes.size()));
        
        // Voice의 지속 시간 가져오기
        float voiceDuration = voice.duration;
        
        // 모든 노트 처리
        for (const auto& note : voice.notes)
        {
            // MIDI 노트 번호 계산
            int midiNote = stringToMidiNote(note.string, note.value);
            
            // 박자 단위의 지속 시간 계산
            float beatDuration = voiceDuration * 4.0f; // 온음표 = 4박자로 변환
            
            // 박자를 실제 틱으로 변환 (정확한 지속 시간 계산)
            int noteDuration = static_cast<int>(beatDuration * ticksPerBeat);
            
            // 최소 지속 시간 보장 (음이 너무 짧아지는 현상 방지)
            noteDuration = juce::jmax(noteDuration, ticksPerBeat / 16);
            
            // 음표가 너무 길면 최대값 제한 (2마디 이상 지속되지 않도록)
            noteDuration = juce::jmin(noteDuration, 2 * 4 * ticksPerBeat);
            
            // 지속 노트 이펙트 처리
            if (note.effect.letRing) {
                // Let ring 효과가 있으면 지속 시간 증가 (최대 2배)
                noteDuration = static_cast<int>(noteDuration * 1.5f);
                DBG("Applied let ring effect: duration extended to " + juce::String(noteDuration));
            }
            
            // 노트 온 이벤트 추가
            int velocity = 100; // 기본 볼륨
            
            // MIDI 노트 온 메시지 생성 (1/127 볼륨 스케일)
            midiMessages.addEvent(juce::MidiMessage::noteOn(1, midiNote, static_cast<float>(velocity) / 127.0f), startSample);
            
            // 활성 노트 목록에 추가
            activeNotes.push_back(ActiveNote(midiNote, 1, sampleCounter, noteDuration));
            
            DBG("Playing note - String: " + juce::String(note.string) + 
                ", Fret: " + juce::String(note.value) + 
                ", MIDI: " + juce::String(midiNote) + 
                ", Velocity: " + juce::String(velocity) + 
                ", Duration: " + juce::String(noteDuration) + " ticks");
        }
    }
}

void TabPlayer::processActiveNotes(juce::MidiBuffer& midiMessages, int numSamples)
{
    // 이 함수는 재생 중인 노트를 관리하고 필요한 경우 노트 오프 이벤트를 생성합니다.
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
            // 노트 오프 이벤트 추가
            midiMessages.addEvent(juce::MidiMessage::noteOff(note.channel, note.midiNote), 0);
            note.isPlaying = false;
            
            // 제거할 노트로 표시
            notesToRemove.push_back(i);
            
            DBG("Note off - MIDI: " + juce::String(note.midiNote) + 
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
    
    // VST 플러그인 리소스 해제
    if (plugin != nullptr)
    {
        plugin = nullptr;
    }
    
    #ifdef _WIN32
    // COM 정리
    CoUninitialize();
    DBG("COM uninitialized");
    #endif
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