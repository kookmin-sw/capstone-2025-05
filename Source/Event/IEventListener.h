#pragma once
#include "Event.h"

/**
 * 이벤트 리스너 인터페이스
 * 이벤트를 수신하고자 하는 클래스는 이 인터페이스를 구현해야 합니다.
 */
class IEventListener
{
public:
    virtual ~IEventListener() = default;
    
    /**
     * 이벤트가 발생했을 때 호출되는 메서드
     * @param event 발생한 이벤트
     * @return 이벤트가 처리되었으면 true, 그렇지 않으면 false
     */
    virtual bool onEvent(const Event& event) = 0;
};