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
    
    /** Component 오버라이드 */
    void paint(juce::Graphics& g) override;
    
    /** ChangeListener 인터페이스 구현 */
    void changeListenerCallback(juce::ChangeBroadcaster*) override;
    
private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioThumbnailCache> thumbnailCache;
    std::unique_ptr<juce::AudioThumbnail> thumbnail;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingThumbnail)
}; 