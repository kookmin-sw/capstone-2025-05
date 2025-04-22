#pragma once
#include <JuceHeader.h>
#include "IEventListener.h"
#include <unordered_map>
#include <vector>

/**
 * 이벤트 버스 클래스
 * 이벤트 발행과 구독을 관리하는 중앙 허브입니다.
 * 싱글톤 패턴으로 구현되어 애플리케이션 전체에서 접근 가능합니다.
 */
class EventBus
{
public:
    /**
     * 싱글톤 인스턴스를 반환합니다.
     */
    static EventBus& getInstance()
    {
        static EventBus instance;
        return instance;
    }
    
    /**
     * 특정 이벤트 타입에 대한 리스너를 등록합니다.
     * @param type 이벤트 타입
     * @param listener 리스너 인스턴스
     */
    void subscribe(Event::Type type, IEventListener* listener)
    {
        if (listener == nullptr)
            return;
            
        // 중복 등록 방지
        auto& listeners = eventListeners[type];
        if (std::find(listeners.begin(), listeners.end(), listener) == listeners.end())
        {
            listeners.push_back(listener);
            DBG("EventBus: Listener registered for event type " + getEventTypeName(type));
        }
    }
    
    /**
     * 모든 이벤트 타입에 대한 리스너를 등록합니다.
     * @param listener 리스너 인스턴스
     */
    void subscribeToAll(IEventListener* listener)
    {
        if (listener == nullptr)
            return;
            
        for (int i = 0; i < static_cast<int>(Event::Type::Custom); ++i)
        {
            subscribe(static_cast<Event::Type>(i), listener);
        }
    }
    
    /**
     * 특정 이벤트 타입에서 리스너를 제거합니다.
     * @param type 이벤트 타입
     * @param listener 리스너 인스턴스
     */
    void unsubscribe(Event::Type type, IEventListener* listener)
    {
        if (listener == nullptr)
            return;
            
        auto it = eventListeners.find(type);
        if (it != eventListeners.end())
        {
            auto& listeners = it->second;
            listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
            DBG("EventBus: Listener unregistered from event type " + getEventTypeName(type));
        }
    }
    
    /**
     * 모든 이벤트 타입에서 리스너를 제거합니다.
     * @param listener 리스너 인스턴스
     */
    void unsubscribeFromAll(IEventListener* listener)
    {
        if (listener == nullptr)
            return;
            
        for (int i = 0; i < static_cast<int>(Event::Type::Custom); ++i)
        {
            unsubscribe(static_cast<Event::Type>(i), listener);
        }
    }
    
    /**
     * 이벤트를 발행합니다.
     * 해당 이벤트 타입을 구독한 모든 리스너에게 이벤트가 전달됩니다.
     * @param event 발행할 이벤트
     */
    void publish(const Event& event)
    {
        Event::Type type = event.getType();
        DBG("EventBus: Publishing event of type " + getEventTypeName(type));
        
        auto it = eventListeners.find(type);
        if (it != eventListeners.end())
        {
            // 이벤트 처리 중 리스너 목록이 변경될 수 있으므로 복사본 사용
            auto listenersCopy = it->second;
            for (auto listener : listenersCopy)
            {
                if (listener)
                {
                    bool handled = listener->onEvent(event);
                    if (handled)
                    {
                        DBG("EventBus: Event handled by a listener");
                    }
                }
            }
        }
    }
    
    /**
     * 메인 스레드에서 이벤트를 발행합니다.
     * 특히 UI 업데이트가 필요한 이벤트에 유용합니다.
     * @param event 발행할 이벤트
     */
    void publishOnMainThread(std::shared_ptr<Event> event)
    {
        juce::MessageManager::callAsync([this, event]() {
            this->publish(*event);
        });
    }
    
private:
    EventBus() = default;
    ~EventBus() = default;
    
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    // 이벤트 타입을 문자열로 변환
    juce::String getEventTypeName(Event::Type type)
    {
        switch (type)
        {
            case Event::Type::AnalysisComplete: return "AnalysisComplete";
            case Event::Type::AnalysisFailed: return "AnalysisFailed";
            case Event::Type::RecordingComplete: return "RecordingComplete";
            case Event::Type::SongLoaded: return "SongLoaded";
            case Event::Type::SongLoadFailed: return "SongLoadFailed";
            case Event::Type::UIRequest: return "UIRequest";
            case Event::Type::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    // 각 이벤트 타입별 리스너 목록
    std::unordered_map<Event::Type, std::vector<IEventListener*>> eventListeners;
};