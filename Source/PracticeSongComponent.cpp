#include "PracticeSongComponent.h"
#include "MainComponent.h"
#include "View/TopBar.h"
#include "View/CenterPanel.h"
#include "View/LeftPanel.h"
#include "View/RightPanel.h"

// 간소화된 RecordingThumbnail 클래스 구현
class RecordingThumbnail : public juce::Component,
                          private juce::ChangeListener
{
public:
    RecordingThumbnail()
    {
        formatManager.registerBasicFormats();
        thumbnail.addChangeListener(this);
    }

    ~RecordingThumbnail() override
    {
        thumbnail.removeChangeListener(this);
    }

    juce::AudioThumbnail& getAudioThumbnail() { return thumbnail; }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::lightgrey);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto endTime = thumbnail.getTotalLength();
            auto thumbArea = getLocalBounds();
            thumbnail.drawChannels(g, thumbArea.reduced(2), 0.0, endTime, 1.0f);
        }
        else
        {
            g.setFont(14.0f);
            g.drawFittedText("(No recorded file)", getLocalBounds(), juce::Justification::centred, 2);
        }
    }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache { 10 };
    juce::AudioThumbnail thumbnail { 512, formatManager, thumbnailCache };

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            repaint();
    }
};

// 간소화된 AudioRecorder 클래스 구현
class AudioRecorder : public juce::AudioIODeviceCallback
{
public:
    AudioRecorder(juce::AudioThumbnail& thumbnailToUpdate)
        : thumbnail(thumbnailToUpdate)
    {
        backgroundThread.startThread();
    }

    ~AudioRecorder() override
    {
        stop();
    }

    void startRecording(const juce::File& file)
    {
        stop();

        if (sampleRate > 0)
        {
            file.deleteFile();

            if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream()))
            {
                juce::WavAudioFormat wavFormat;

                if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 1, 16, {}, 0))
                {
                    fileStream.release();

                    threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));

                    thumbnail.reset(writer->getNumChannels(), writer->getSampleRate());
                    nextSampleNum = 0;

                    const juce::ScopedLock sl(writerLock);
                    activeWriter = threadedWriter.get();
                }
            }
        }
    }

    void stop()
    {
        {
            const juce::ScopedLock sl(writerLock);
            activeWriter = nullptr;
        }

        threadedWriter.reset();
    }

    bool isRecording() const
    {
        return activeWriter.load() != nullptr;
    }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        sampleRate = device->getCurrentSampleRate();
    }

    void audioDeviceStopped() override
    {
        sampleRate = 0;
    }

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                         float* const* outputChannelData, int numOutputChannels,
                                         int numSamples, const juce::AudioIODeviceCallbackContext& context) override
    {
        const juce::ScopedLock sl(writerLock);

        if (activeWriter.load() != nullptr && numInputChannels >= thumbnail.getNumChannels())
        {
            activeWriter.load()->write(inputChannelData, numSamples);

            juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), thumbnail.getNumChannels(), numSamples);
            thumbnail.addBlock(nextSampleNum, buffer, 0, numSamples);
            nextSampleNum += numSamples;
        }

        // 출력 버퍼 초기화
        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                juce::FloatVectorOperations::clear(outputChannelData[i], numSamples);
    }

private:
    juce::AudioThumbnail& thumbnail;
    juce::TimeSliceThread backgroundThread { "Audio Recorder Thread" };
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    double sampleRate = 0.0;
    juce::int64 nextSampleNum = 0;

    juce::CriticalSection writerLock;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
};

PracticeSongComponent::PracticeSongComponent(MainComponent &mainComp)
    : mainComponent(mainComp), deviceManager(mainComp.getDeviceManager())
{
    // parser는 여기서 초기화하지 않고, loadSong 메서드에서 필요할 때 생성
    
    auto result = deviceManager.initialiseWithDefaultDevices(0, 2);
    if (result.isEmpty())
        DBG("Audio device initialized successfully!");
    else
        DBG("Audio device initialization failed: " + result);

    // 재생 버튼 초기화
    playButton.setButtonText("Play");
    playButton.onClick = [this]() { togglePlayback(); };
    addAndMakeVisible(playButton);
    playButton.setEnabled(false); // 곡이 선택되기 전까지 비활성화
    
    // 녹음 관련 컴포넌트 초기화
    formatManager.registerBasicFormats();
    recordingThumbnail = std::make_unique<RecordingThumbnail>();
    audioRecorder = std::make_unique<AudioRecorder>(recordingThumbnail->getAudioThumbnail());
    
    // 녹음 버튼 초기화
    recordButton.setButtonText("Record");
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff5c5c));
    recordButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    recordButton.onClick = [this]() {
        if (isRecording())
            stopRecording();
        else
            startRecording();
    };
    addAndMakeVisible(recordButton);
    recordButton.setEnabled(false); // 곡이 선택되기 전까지 비활성화
    
    // 분석 버튼 초기화
    analyzeButton.setButtonText("Analyze");
    analyzeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff5c5cff));
    analyzeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    analyzeButton.onClick = [this]() { analyzeRecording(); };
    addAndMakeVisible(analyzeButton);
    analyzeButton.setEnabled(false); // 녹음된 파일이 없으면 비활성화

    // AudioRecorder를 오디오 콜백으로 등록
    deviceManager.addAudioCallback(audioRecorder.get());
    
    // TabPlayer를 오디오 시스템에 연결
    // Player에 현재 샘플 레이트와 버퍼 크기 설정
    auto currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        player.prepareToPlay(currentDevice->getCurrentSampleRate(), 
                            currentDevice->getCurrentBufferSizeSamples());
    }
    else
    {
        // 기본값 설정
        player.prepareToPlay(44100.0, 512);
    }
    
    // TabPlayer를 오디오 콜백으로 등록 - 중요!
    deviceManager.addAudioCallback(&player);
    
    // Create controllers
    audioController = std::make_unique<AudioController>(audioModel, deviceManager);
    transportController = std::make_unique<TransportController>(audioModel);
    
    // AudioModel에 리스너로 등록
    audioModel.addListener(this);

    // Create view components
    topBar = std::make_unique<TopBar>(*this);
    centerPanel = std::make_unique<CenterPanel>();
    leftPanel = std::make_unique<LeftPanel>(audioModel); // Pass model to view
    rightPanel = std::make_unique<RightPanel>();
    
    try {
        // ScoreComponent는 player가 초기화된 후에 생성
        scoreComponent = std::make_unique<ScoreComponent>(player);
        addAndMakeVisible(scoreComponent.get());
    }
    catch (const std::exception& e) {
        DBG("Error creating ScoreComponent: " + juce::String(e.what()));
        // 예외가 발생해도 계속 진행
    }

    // Add components to view
    addAndMakeVisible(topBar.get());
    addAndMakeVisible(centerPanel.get());
    addAndMakeVisible(leftPanel.get());
    addAndMakeVisible(rightPanel.get());
    addAndMakeVisible(recordingThumbnail.get());
}

PracticeSongComponent::~PracticeSongComponent()
{
    // AudioModel에서 리스너 제거
    audioModel.removeListener(this);
    
    // TabPlayer를 오디오 시스템에서 제거
    deviceManager.removeAudioCallback(&player);
    
    // 녹음기도 제거
    if (audioRecorder)
        deviceManager.removeAudioCallback(audioRecorder.get());
    
    // Controllers will clean up their resources in their destructors
}

void PracticeSongComponent::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PracticeSongComponent::resized()
{
    auto bounds = getLocalBounds();
    auto topBarHeight = 50;
    auto controlsHeight = 40;
    
    // 상단 바
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    
    // 하단에 악보 표시 영역
    auto scoreHeight = 300;
    auto scoreArea = bounds.removeFromBottom(scoreHeight);
    scoreComponent->setBounds(scoreArea);
    
    // 하단 컨트롤 영역 (재생, 녹음, 분석 버튼)
    auto controlsArea = bounds.removeFromBottom(controlsHeight);
    auto buttonWidth = 120;
    auto spacing = 10;
    
    playButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(spacing);
    recordButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    controlsArea.removeFromLeft(spacing);
    analyzeButton.setBounds(controlsArea.removeFromLeft(buttonWidth));
    
    // 녹음된 오디오 파형 표시 영역
    auto waveformHeight = 100;
    auto waveformArea = bounds.removeFromBottom(waveformHeight);
    recordingThumbnail->setBounds(waveformArea);
    
    // 좌우 패널
    leftPanel->setBounds(bounds.removeFromLeft(300));
    rightPanel->setBounds(bounds.removeFromRight(300));
    
    // 중앙 패널
    centerPanel->setBounds(bounds.reduced(50));
}

// AudioModel::Listener 메서드 구현
void PracticeSongComponent::playStateChanged(bool isPlaying)
{
    // View 업데이트
    updatePlaybackState(isPlaying);
}

void PracticeSongComponent::volumeChanged(float newVolume)
{
    // View 업데이트
    updateVolumeDisplay(newVolume);
}

// View 업데이트 메서드
void PracticeSongComponent::updatePlaybackState(bool isNowPlaying)
{
    if (isNowPlaying)
    {
        playButton.setButtonText("Stop");
    }
    else
    {
        playButton.setButtonText("Play");
    }
}

void PracticeSongComponent::updateVolumeDisplay(float volume)
{
    // 볼륨 표시 UI 요소가 있을 경우 여기서 업데이트
}

void PracticeSongComponent::togglePlayback()
{
    // Controller를 통해 모델 상태 변경 (MVC 패턴)
    if (!audioModel.isPlaying())
    {
        transportController->startPlayback();
        player.startPlaying();
    }
    else
    {
        transportController->stopPlayback();
        player.stopPlaying();
    }
}

bool PracticeSongComponent::loadSong(const juce::String& songId)
{
    // 서버가 준비되지 않았으므로 로컬 파일 경로를 사용하여 테스트
    juce::String filePath;
    
    // 현재 디버깅 중인 위치와 현재 경로 로깅
    DBG("Current working directory: " + juce::File::getCurrentWorkingDirectory().getFullPathName());
    
    // 테스트용 곡 ID와 경로 매핑
    if (songId == "song1")
        filePath = "D:/audio_dataset/recording/homecoming/homecoming.gp5";
    else if (songId == "song2")
        filePath = "D:/audio_dataset/recording/song2/song2.gp5";
    else if (songId == "song3")
        filePath = "D:/audio_dataset/recording/song3/song3.gp5";
    else if (songId == "song4")
        filePath = "D:/audio_dataset/recording/song4/song4.gp5";
    else if (songId == "song5")
        filePath = "D:/audio_dataset/recording/song5/song5.gp5";
    else
        return false; // 지원하지 않는 곡 ID
    
    // 파일 존재 여부 확인
    juce::File gpFile(filePath);
    if (!gpFile.existsAsFile()) {
        DBG("GP file does not exist: " + filePath);
        
        // 테스트 목적으로 로드 성공으로 가정하고 다른 UI 요소는 활성화
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        // 빈 TabFile 설정
        player.setTabFile(gp_parser::TabFile(major, minor, title, subtitle, artist, album,
                                            lyricsAuthor, musicAuthor, copyright, tab, instructions,
                                            comments, lyric, tempoValue, globalKeySignature, channels,
                                            measures, trackCount, measureHeaders, tracks));
        
        if (scoreComponent != nullptr)
            scoreComponent->updateScore();
            
        return true;
    }
    
    resetParser();
    
    try {
        // 파서 생성 및 악보 로드 (포인터로 변경됨)
        DBG("Loading GP file from: " + filePath);
        parserPtr = std::make_unique<gp_parser::Parser>(filePath.toRawUTF8());
        
        try {
            // TabPlayer에 악보 데이터 설정
            // Parser로부터 TabFile 획득
            auto tabFile = parserPtr->getTabFile();
            
            // 벡터 요소 유효성 검사 (빈 트랙 검사)
            if (tabFile.tracks.empty()) {
                DBG("Warning: Loaded TabFile has no tracks");
                
                // 빈 트랙이 있는 경우 기본 TabFile 생성
                tracks.clear();
                measureHeaders.clear();
                channels.clear();
                // 디폴트 값으로 빈 TabFile 생성
                player.setTabFile(gp_parser::TabFile(major, minor, title, subtitle, artist, album,
                                                 lyricsAuthor, musicAuthor, copyright, tab, instructions,
                                                 comments, lyric, tempoValue, globalKeySignature, channels,
                                                 measures, trackCount, measureHeaders, tracks));
            } 
            else {
                DBG("TabFile loaded successfully with " + juce::String(tabFile.tracks.size()) + " tracks");
                
                // 첫 번째 트랙에 마디가 있는지 확인
                if (tabFile.tracks[0].measures.empty()) {
                    DBG("Warning: First track has no measures");
                }
                else {
                    DBG("First track has " + juce::String(tabFile.tracks[0].measures.size()) + " measures");
                }
                
                // TabPlayer에 TabFile 설정
                player.setTabFile(tabFile);
            }
        }
        catch (const std::exception& e) {
            DBG("Error in parser.getTabFile() or player.setTabFile(): " + juce::String(e.what()));
            
            // 오류 발생 시에도 UI 요소는 활성화
            playButton.setEnabled(true);
            recordButton.setEnabled(true);
            
            // ScoreComponent는 별도로 처리
            if (scoreComponent != nullptr) {
                try {
                    scoreComponent->updateScore();
                }
                catch (const std::exception& e) {
                    DBG("Error updating score component: " + juce::String(e.what()));
                }
            }
            
            return false;
        }
        
        // 재생 및 녹음 버튼 활성화
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        // ScoreComponent 업데이트
        if (scoreComponent != nullptr) {
            try {
                scoreComponent->updateScore();
            }
            catch (const std::exception& e) {
                DBG("Error updating score component: " + juce::String(e.what()));
            }
        }
        
        DBG("Song loaded successfully: " + songId);
        return true;
    }
    catch (const std::exception& e) {
        DBG("Error loading song: " + juce::String(e.what()));
        
        // 오류가 발생해도 UI 요소는 활성화
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        return false;
    }
    catch (...) {
        DBG("Unknown error loading song");
        
        // 오류가 발생해도 UI 요소는 활성화
        playButton.setEnabled(true);
        recordButton.setEnabled(true);
        
        return false;
    }
}

void PracticeSongComponent::resetParser()
{
    // 재생 중이면 중지
    if (audioModel.isPlaying()) {
        transportController->stopPlayback();
        player.stopPlaying();
    }
    
    // 재생 버튼 비활성화
    playButton.setEnabled(false);
}

void PracticeSongComponent::startRecording()
{
    // 재생 중이면 중지
    if (audioModel.isPlaying())
        togglePlayback();
    
    // 임시 녹음 파일 경로 설정
    auto parentDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    lastRecording = parentDir.getNonexistentChildFile("Recording", ".wav");
    
    // 녹음 시작
    audioRecorder->startRecording(lastRecording);
    
    // UI 업데이트
    recordButton.setButtonText("Stop");
    analyzeButton.setEnabled(false);
    
    DBG("Recording started: " + lastRecording.getFullPathName());
}

void PracticeSongComponent::stopRecording()
{
    // 녹음 중지
    audioRecorder->stop();
    
    // UI 업데이트
    recordButton.setButtonText("Record");
    analyzeButton.setEnabled(true);
    
    DBG("Recording finished: " + lastRecording.getFullPathName());
}

bool PracticeSongComponent::isRecording() const
{
    return audioRecorder && audioRecorder->isRecording();
}

void PracticeSongComponent::analyzeRecording()
{
    if (!lastRecording.existsAsFile())
    {
        DBG("No recorded file to analyze.");
        return;
    }
    
    // 서버에 분석 요청할 URL
    juce::String serverUrl = "http://localhost:8000/analyze";
    
    // The server is not ready yet, so just print a message for now
    DBG("Audio analysis request: " + lastRecording.getFullPathName());
    DBG("Server URL: " + serverUrl);
    DBG("Actual analysis request will be implemented when the server is ready.");

    // You can add UI elements to display the analysis result.
    // For now, just show a simple message box
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                         "Audio Analysis",
                                         "The recorded audio file will be sent to the server for analysis.\n"
                                         "This feature will be enabled when the server is ready.",
                                         "OK");
}