#pragma once
#include <JuceHeader.h>

/**
 * 녹음된 오디오의 파형을 시각적으로 표시하는 컴포넌트
 * 오디오 파일이나 실시간 녹음을 시각화할 수 있습니다.
 */
class RecordingThumbnail : public juce::Component,
                          private juce::ChangeListener
{
public:
    RecordingThumbnail();
    ~RecordingThumbnail() override;
    
    /**
     * 오디오 파일을 썸네일 소스로 설정합니다.
     * @param audioFile 오디오 파일 경로
     */
    void setSource(const juce::File& audioFile);
    
    /**
     * 오디오 썸네일 객체에 대한 참조를 반환합니다.
     * 녹음기가 이 썸네일을 업데이트할 수 있도록 합니다.
     * @return 오디오 썸네일 참조
     */
    juce::AudioThumbnail& getAudioThumbnail();
    
    /**
     * 전체 썸네일을 표시할지 아니면 최근 부분만 표시할지 설정합니다.
     * @param displayFull true면 전체 썸네일 표시, false면 최근 부분만 표시
     */
    void setDisplayFullThumbnail(bool displayFull);
    
    /** Component 오버라이드 */
    void paint(juce::Graphics& g) override;
    
private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache {10};
    juce::AudioThumbnail thumbnail {512, formatManager, thumbnailCache};
    
    bool displayFullThumb = false;
    
    /** ChangeListener 인터페이스 구현 */
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingThumbnail)
}; 