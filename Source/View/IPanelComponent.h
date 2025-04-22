#pragma once
#include <JuceHeader.h>

/**
 * 패널 컴포넌트 인터페이스
 * 기타 연습 화면의 모든 패널 컴포넌트가 구현해야 하는 인터페이스입니다.
 */
class IPanelComponent
{
public:
    virtual ~IPanelComponent() = default;
    
    /**
     * 패널 초기화
     */
    virtual void initialize() = 0;
    
    /**
     * 패널 업데이트
     * 데이터 변경 시 UI를 새로고침합니다.
     */
    virtual void updatePanel() = 0;
    
    /**
     * 패널 리셋
     * 패널의 상태를 초기 상태로 되돌립니다.
     */
    virtual void resetPanel() = 0;
    
    /**
     * juce::Component로 변환
     * 실제 JUCE 컴포넌트를 반환합니다.
     */
    virtual juce::Component* asComponent() = 0;
}; 