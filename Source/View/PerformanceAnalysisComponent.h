#pragma once
#include <JuceHeader.h>
#include "LookAndFeel/MapleTheme.h"
#include "Maple3DAudioVisualiserComponent.h"
#include "View/MapleHorizontalAudioVisualiserComponent.h"
#include "../Model/Song.h"
#include "../Controller/ContentController.h"

// Performance analysis display component
class PerformanceAnalysisComponent : public juce::Component,
                                    public juce::Timer
{
public:
    // Structure to hold note accuracy data
    struct NoteAccuracy
    {
        int time;           // Time position (could be tick or note index)
        float accuracy;     // Accuracy value (0-1)
        bool isCorrect;     // Whether the note was played correctly
    };

    PerformanceAnalysisComponent() : accuracyMeter(progressValue)
    {
        addAndMakeVisible(accuracyMeter);
        addAndMakeVisible(titleLabel);
        addAndMakeVisible(accuracyLabel);
        addAndMakeVisible(timingLabel);
        addAndMakeVisible(difficultyLabel);
        addAndMakeVisible(visualiserComponent); // 3D 시각화 컴포넌트 추가
        addAndMakeVisible(horizontalVisualiserComponent); // 수평 시각화 컴포넌트 추가
        addAndMakeVisible(albumThumbnail); // 앨범 썸네일 컴포넌트 추가
        
        // 수평 시각화 컴포넌트의 진폭 감도 조절
        horizontalVisualiserComponent.setDynamicScaleFactor(150.0f); // 기본값 270.0f에서 감소
        
        // 시각화 모드 초기화 - 바 모드로 고정
        horizontalVisualiserComponent.setVisualisationMode(MapleHorizontalAudioVisualiserComponent::BarSpectrum);
        
        // 기타 주파수 범위 초기화 (기타 주파수 범위로 제한)
        // 일반적인 기타 음역 범위: 표준 튜닝 E2(82Hz)~E6(1318Hz), 하모닉스 포함 ~2kHz
        const float minFreq = 80.0f;   // 최저 주파수 (Hz) - 가장 낮은 E 음 커버
        const float maxFreq = 2000.0f; // 최고 주파수 (Hz) - 하모닉스 포함
        horizontalVisualiserComponent.setFrequencyRange(minFreq, maxFreq);
        
        titleLabel.setText("Performance Analysis", juce::dontSendNotification);
        titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
        titleLabel.setJustificationType(juce::Justification::centred);
        
        accuracyLabel.setText("Accuracy: Needs analysis", juce::dontSendNotification);
        accuracyLabel.setJustificationType(juce::Justification::centredLeft);
        
        timingLabel.setText("Timing Accuracy: Needs analysis", juce::dontSendNotification);
        timingLabel.setJustificationType(juce::Justification::centredLeft);
        
        difficultyLabel.setText("Difficult sections: Needs analysis", juce::dontSendNotification);
        difficultyLabel.setJustificationType(juce::Justification::centredLeft);
        
        // Set demo data
        setPerformanceData(0.0f, 0.0f);
        
        // Initialize note accuracy data for demo
        for (int i = 0; i < 32; ++i)
        {
            // Generate random accuracy values for demo
            float accuracy = juce::Random::getSystemRandom().nextFloat();
            bool isCorrect = accuracy > 0.3f;
            
            NoteAccuracy note;
            note.time = i;
            note.accuracy = accuracy;
            note.isCorrect = isCorrect;
            
            noteAccuracyData.add(note);
        }
        
        startTimer(30); // Start animation timer
    }
    
    ~PerformanceAnalysisComponent() override
    {
        stopTimer();
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(MapleTheme::getCardColour());
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds().reduced(15);
        
        // Title
        titleLabel.setBounds(bounds.removeFromTop(30));
        
        // 수평 오디오 시각화 컴포넌트를 하단에 배치
        auto horizontalVisualiserHeight = 150;
        auto horizontalVisualiserArea = bounds.removeFromBottom(horizontalVisualiserHeight);
        
        // 수평 시각화 컴포넌트 배치 (전체 공간 사용)
        horizontalVisualiserComponent.setBounds(horizontalVisualiserArea.reduced(5));
        
        // 3D 시각화 컴포넌트를 오른쪽에 배치
        auto visualiserArea = bounds.removeFromRight(bounds.getWidth() * 0.6f);
        visualiserComponent.setBounds(visualiserArea.reduced(5));
        
        // 앨범 썸네일 영역 (왼쪽 영역에 배치)
        albumThumbnail.setBounds(bounds.reduced(5));
    }
    
    // ContentController 설정
    void setContentController(ContentController* controller)
    {
        contentController = controller;
        
        // 이미 로드된 곡이 있다면 썸네일 다시 로드 시도
        if (contentController != nullptr && !currentSongId.isEmpty())
        {
            loadAlbumThumbnailForSong(currentSongId);
        }
    }
    
    // 앨범 썸네일 설정 메서드
    void setAlbumThumbnail(const juce::Image& image)
    {
        albumThumbnail.setImage(image);
        repaint();
    }
    
    // 앨범 썸네일 설정 메서드 (파일 경로로부터)
    void setAlbumThumbnailFromFile(const juce::File& file)
    {
        if (file.existsAsFile())
        {
            juce::Image thumbnailImage = juce::ImageFileFormat::loadFrom(file);
            if (!thumbnailImage.isNull())
                setAlbumThumbnail(thumbnailImage);
        }
    }
    
    // 곡 ID로 앨범 썸네일 설정 (메인 컴포넌트에서 호출)
    void setAlbumThumbnailForSong(const juce::String& songId)
    {
        currentSongId = songId;
        
        // ContentController가 없으면 임시 이미지 표시
        if (contentController == nullptr)
        {
            DBG("PerformanceAnalysisComponent::setAlbumThumbnailForSong - ContentController is null, showing placeholder");
            createPlaceholderThumbnail(songId);
            return;
        }
        
        loadAlbumThumbnailForSong(songId);
    }
    
    // 곡 객체로 앨범 썸네일 설정
    void setAlbumThumbnailForSong(const Song& song)
    {
        currentSongId = song.getId();
        currentSong = song;
        
        // 이미 캐시된 이미지가 있는지 확인
        if (song.hasCachedCoverImage())
        {
            // 캐시된 이미지 사용
            DBG("PerformanceAnalysisComponent::setAlbumThumbnailForSong - using cached image");
            setAlbumThumbnail(song.getCachedCoverImage());
            return;
        }
        
        // ContentController가 없으면 로컬 파일 또는 임시 이미지 표시
        if (contentController == nullptr)
        {
            // 로컬 썸네일 시도
            juce::String thumbnailPath = song.getThumbnailPath();
            if (!thumbnailPath.isEmpty())
            {
                juce::File thumbnailFile(thumbnailPath);
                if (thumbnailFile.existsAsFile())
                {
                    setAlbumThumbnailFromFile(thumbnailFile);
                    return;
                }
            }
            
            // 로컬 썸네일 실패 시 임시 이미지 표시
            createPlaceholderThumbnail(song.getTitle(), song.getArtist());
            return;
        }
        
        // ContentController를 통해 이미지 로드
        loadAlbumThumbnailForSong(song.getId());
    }
    
    void timerCallback() override
    {
        // Update animation
        animationOffset += 1.0f;
        if (animationOffset > 1000.0f)
            animationOffset = 0.0f;
            
        // Refresh UI
        repaint();
    }
    
    void setPerformanceData(float noteAccuracy, float timingAccuracy)
    {
        this->noteAccuracy = noteAccuracy;
        this->timingAccuracy = timingAccuracy;
        
        // Update progress value that ProgressBar is watching
        progressValue = noteAccuracy;
        
        // In real implementation, update labels based on analysis results
        // For now, just display demo values
        accuracyLabel.setText("Accuracy: " + juce::String(int(noteAccuracy * 100.0f)) + "%", 
                             juce::dontSendNotification);
        
        timingLabel.setText("Timing Accuracy: " + juce::String(int(timingAccuracy * 100.0f)) + "%", 
                           juce::dontSendNotification);
                           
        // In real implementation this would be called after analysis
        startTimer(30);
    }
    
    // In real implementation, this would be called when note accuracy data is available
    void setNoteAccuracyData(const juce::Array<NoteAccuracy>& data)
    {
        noteAccuracyData = data;
        repaint();
    }
    
    // 오디오 데이터를 시각화 컴포넌트로 전달하는 메서드 (재생 데이터용)
    void pushAudioBuffer(const juce::AudioBuffer<float>& buffer)
    {
        DBG("pushAudioBuffer size: " + juce::String(buffer.getNumSamples()));
        visualiserComponent.pushBuffer(buffer);
    }
    
    // 마이크 입력 데이터를 수평 시각화 컴포넌트로 전달하는 메서드
    void pushMicrophoneBuffer(const juce::AudioBuffer<float>& buffer)
    {
        // 마이크 입력 데이터의 진폭을 조절하기 위한 임시 버퍼
        static juce::AudioBuffer<float> scaledBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        
        // 입력 버퍼 복사
        scaledBuffer.clear();
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            scaledBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());
        }
        
        // 진폭 스케일 조절 (값을 작게 만들어 최대치까지 올라가는 것을 방지)
        // 적당한 스케일 값을 찾아 조절. 필요에 따라 이 값을 변경할 수 있음
        float scaleFactor = 0.15f; // 0.3f에서 더 감소
        
        for (int ch = 0; ch < scaledBuffer.getNumChannels(); ++ch)
        {
            auto* channelData = scaledBuffer.getWritePointer(ch);
            for (int i = 0; i < scaledBuffer.getNumSamples(); ++i)
            {
                channelData[i] *= scaleFactor;
            }
        }
        
        // 스케일이 조절된 데이터를 시각화 컴포넌트로 전달
        horizontalVisualiserComponent.pushBuffer(scaledBuffer);
    }
    
    // 시각화 컴포넌트를 초기화하는 메서드 (재생 중지 시 호출)
    void clearVisualization()
    {
        // 두 시각화 컴포넌트를 모두 초기화
        clear3DVisualization();
        clearHorizontalVisualization();
    }
    
    // 3D 시각화만 초기화하는 메서드 (재생 중지 시 호출)
    void clear3DVisualization()
    {
        // 3D 시각화 초기화
        visualiserComponent.clear();
    }
    
    // 수평 시각화만 초기화하는 메서드 (마이크 모니터링 비활성화 시 호출)
    void clearHorizontalVisualization()
    {
        // 빈 버퍼를 생성하여 수평 시각화 초기화
        juce::AudioBuffer<float> emptyBuffer(2, 1024);
        emptyBuffer.clear(); // 모든 샘플을 0으로 설정
        
        // 수평 시각화 초기화
        horizontalVisualiserComponent.clear();
    }
    
    // 안전한 종료를 위한 메서드 (GuitarPracticeComponent 소멸자에서 호출)
    void safeShutdown()
    {
        // 내부 타이머 중지
        stopTimer();
        
        // 3D 시각화 안전 종료
        visualiserComponent.safeShutdown();
        
        // 수평 시각화 초기화
        horizontalVisualiserComponent.clear();
        horizontalVisualiserComponent.stopTimer();
        
        DBG("PerformanceAnalysisComponent: Safe shutdown complete");
    }
    
    // 내부 컴포넌트 타이머 중지
    void stopTimer()
    {
        // 내부 컴포넌트의 타이머 중지
        visualiserComponent.stopTimer();
        horizontalVisualiserComponent.stopTimer();
    }

private:
    // ContentController를 통해 앨범 썸네일 로드
    void loadAlbumThumbnailForSong(const juce::String& songId)
    {
        if (songId.isEmpty())
            return;
            
        DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Loading thumbnail for songId: " + songId);
        
        // 캐시 디렉토리에서 이미지 파일 찾기 시도
        juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                             .getChildFile("MapleClientDesktop/cache/images");
        
        // 캐시 디렉토리가 없다면 생성
        if (!cacheDir.exists())
        {
            bool dirCreated = cacheDir.createDirectory();
            
            // 실패했다면 재귀적으로 상위 디렉토리부터 생성 시도
            if (!dirCreated)
            {
                // 상위 경로 차례대로 생성
                juce::File parentDir = cacheDir.getParentDirectory();
                while (!parentDir.exists() && parentDir != juce::File())
                {
                    parentDir.createDirectory();
                    parentDir = parentDir.getParentDirectory();
                }
                
                // 다시 생성 시도
                dirCreated = cacheDir.createDirectory();
                
                // 하위 디렉토리 생성
                if (dirCreated)
                {
                    // 상위 폴더들이 생성되었으므로 다시 시도
                    cacheDir.createDirectory();
                }
            }
            
            DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Creating cache directory: " + 
                juce::String(dirCreated ? "success" : "failed") + " - " + cacheDir.getFullPathName());
        }
        
        // 가능한 이미지 확장자 확인
        juce::Array<juce::String> possibleExtensions = { ".jpg", ".jpeg", ".png", ".gif", ".bmp" };
        juce::File cachedImageFile;
        
        // songId_thumbnail 패턴의 파일 찾기
        for (const auto& ext : possibleExtensions)
        {
            juce::File testFile = cacheDir.getChildFile(songId + "_thumbnail" + ext);
            if (testFile.existsAsFile())
            {
                cachedImageFile = testFile;
                break;
            }
        }
        
        // 캐시된 이미지가 있으면 로드
        if (cachedImageFile.existsAsFile())
        {
            DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Found cached image: " + cachedImageFile.getFullPathName());
            juce::Image cachedImage = juce::ImageFileFormat::loadFrom(cachedImageFile);
            
            if (cachedImage.isValid())
            {
                DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Successfully loaded cached image");
                setAlbumThumbnail(cachedImage);
                return;
            }
            // 이미지 로드 실패 시 계속 진행
            DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Failed to load cached image, trying ContentController");
        }
        
        // ContentController가 없으면 기본 이미지 표시
        if (contentController == nullptr)
        {
            // 로딩 실패 시 기본 이미지 표시
            createPlaceholderThumbnail(songId);
            return;
        }
        
        // 로딩 상태 표시
        juce::Image loadingImage(juce::Image::RGB, 300, 300, true);
        juce::Graphics g(loadingImage);
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText("Loading thumbnail...", loadingImage.getBounds(), juce::Justification::centred, true);
        setAlbumThumbnail(loadingImage);
        
        // Song 객체 생성 또는 가져오기
        Song tempSong;
        if (currentSong.getId() == songId)
        {
            // 이미 현재 Song 객체가 있으면 사용
            tempSong = currentSong;
        }
        else
        {
            // 임시 Song 객체 생성 - ID만 설정
            tempSong = Song(songId, "", "", "");
        }
        
        // ContentController를 통해 이미지 로드
        contentController->loadSongCoverImage(tempSong, [this](bool success, const Song& updatedSong) {
            if (success && updatedSong.hasCachedCoverImage())
            {
                // 이미지 로드 성공
                DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Image loaded successfully");
                currentSong = updatedSong; // 업데이트된 Song 저장
                setAlbumThumbnail(updatedSong.getCachedCoverImage());
            }
            else
            {
                // 이미지 로드 실패 시 기본 이미지 표시
                DBG("PerformanceAnalysisComponent::loadAlbumThumbnailForSong - Failed to load image");
                createPlaceholderThumbnail(updatedSong.getTitle(), updatedSong.getArtist());
            }
        });
    }
    
    // 임시 이미지 생성 (ID만 있는 경우)
    void createPlaceholderThumbnail(const juce::String& songId)
    {
        juce::Image placeholderImage(juce::Image::RGB, 300, 300, true);
        juce::Graphics g(placeholderImage);
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawText("Song ID: " + songId, placeholderImage.getBounds().reduced(20), juce::Justification::centred, true);
        setAlbumThumbnail(placeholderImage);
    }
    
    // 임시 이미지 생성 (제목과 아티스트 정보가 있는 경우)
    void createPlaceholderThumbnail(const juce::String& title, const juce::String& artist)
    {
        juce::Image placeholderImage(juce::Image::RGB, 300, 300, true);
        juce::Graphics g(placeholderImage);
        g.fillAll(juce::Colours::darkgrey);
        
        // 테두리 그리기
        g.setColour(juce::Colours::lightgrey);
        g.drawRect(placeholderImage.getBounds().reduced(5), 2);
        
        // 제목 텍스트
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(24.0f, juce::Font::bold));
        g.drawText(title, placeholderImage.getBounds().reduced(15).removeFromTop(150), 
                  juce::Justification::centredBottom, true);
        
        // 아티스트 텍스트
        g.setColour(juce::Colours::lightgrey);
        g.setFont(juce::Font(18.0f));
        g.drawText(artist, placeholderImage.getBounds().reduced(15).removeFromBottom(150), 
                  juce::Justification::centredTop, true);
        
        setAlbumThumbnail(placeholderImage);
    }

    juce::Label titleLabel;
    juce::Label accuracyLabel;
    juce::Label timingLabel;
    juce::Label difficultyLabel;
    
    Maple3DAudioVisualiserComponent visualiserComponent; // 3D 시각화 컴포넌트
    MapleHorizontalAudioVisualiserComponent horizontalVisualiserComponent; // 수평 시각화 컴포넌트
    
    // 앨범 썸네일 이미지 컴포넌트
    class AlbumThumbnailComponent : public juce::Component
    {
    public:
        AlbumThumbnailComponent()
        {
            // 기본 이미지 생성
            image = juce::Image(juce::Image::RGB, 300, 300, true);
            juce::Graphics g(image);
            g.fillAll(juce::Colours::black);
            g.setColour(juce::Colours::white);
            g.drawText("No Album Art", 0, 0, 300, 300, juce::Justification::centred, true);
        }
        
        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat().reduced(5.0f);
            
            // 이미지 비율 유지하면서 컴포넌트에 맞게 그리기
            if (!image.isNull())
            {
                float imageRatio = image.getWidth() / (float)image.getHeight();
                float boundsRatio = bounds.getWidth() / bounds.getHeight();
                
                juce::Rectangle<float> targetBounds;
                
                if (imageRatio > boundsRatio)
                {
                    // 이미지가 더 넓은 비율
                    float height = bounds.getWidth() / imageRatio;
                    targetBounds = juce::Rectangle<float>(
                        bounds.getX(),
                        bounds.getCentreY() - height / 2.0f,
                        bounds.getWidth(),
                        height
                    );
                }
                else
                {
                    // 이미지가 더 좁은 비율
                    float width = bounds.getHeight() * imageRatio;
                    targetBounds = juce::Rectangle<float>(
                        bounds.getCentreX() - width / 2.0f,
                        bounds.getY(),
                        width,
                        bounds.getHeight()
                    );
                }
                
                // 이미지 테두리 그리기
                g.setColour(juce::Colours::white.withAlpha(0.5f));
                g.drawRoundedRectangle(targetBounds.expanded(2.0f), 3.0f, 1.5f);
                
                // 이미지 그리기
                g.drawImage(image, targetBounds);
            }
        }
        
        void setImage(const juce::Image& newImage)
        {
            if (!newImage.isNull())
            {
                image = newImage;
                repaint();
            }
        }
        
    private:
        juce::Image image;
    };
    
    AlbumThumbnailComponent albumThumbnail; // 앨범 썸네일 컴포넌트
    juce::String currentSongId; // 현재 표시 중인 곡 ID
    Song currentSong; // 현재 표시 중인 Song 객체
    ContentController* contentController = nullptr; // ContentController 참조
    
    double progressValue = 0.8; // Value that ProgressBar will watch
    juce::ProgressBar accuracyMeter; // ProgressBar initialized with progressValue reference in constructor
    
    float noteAccuracy = 0.8f;    // Note accuracy (0-1 range)
    float timingAccuracy = 0.7f;  // Timing accuracy (0-1 range)
    float animationOffset = 0.0f; // Animation offset
    
    juce::Array<NoteAccuracy> noteAccuracyData; // Store individual note accuracy data
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceAnalysisComponent)
}; 