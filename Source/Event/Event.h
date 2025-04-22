#pragma once
#include <JuceHeader.h>

/**
 * 기본 이벤트 클래스
 * 모든 이벤트 타입의 기본 클래스로 사용됩니다.
 */
class Event
{
public:
    enum class Type
    {
        AnalysisComplete,   // 분석 완료 이벤트
        AnalysisFailed,     // 분석 실패 이벤트 
        RecordingComplete,  // 녹음 완료 이벤트
        SongLoaded,         // 곡 로드 완료 이벤트
        SongLoadFailed,     // 곡 로드 실패 이벤트
        UIRequest,          // UI 관련 요청 이벤트
        Custom              // 사용자 정의 이벤트
    };
    
    Event(Type type) : eventType(type) {}
    virtual ~Event() = default;
    
    Type getType() const { return eventType; }
    
private:
    Type eventType;
};

/**
 * 분석 완료 이벤트
 * 녹음 분석이 완료되었을 때 발생하는 이벤트입니다.
 */
class AnalysisCompleteEvent : public Event
{
public:
    AnalysisCompleteEvent(const juce::var& resultData)
        : Event(Type::AnalysisComplete), result(resultData) {}
    
    const juce::var& getResult() const { return result; }
    
private:
    juce::var result;  // 분석 결과 JSON 데이터
};

/**
 * 분석 실패 이벤트
 * 녹음 분석이 실패했을 때 발생하는 이벤트입니다.
 */
class AnalysisFailedEvent : public Event
{
public:
    AnalysisFailedEvent(const juce::String& errorMsg)
        : Event(Type::AnalysisFailed), errorMessage(errorMsg) {}
    
    const juce::String& getErrorMessage() const { return errorMessage; }
    
private:
    juce::String errorMessage;
};

/**
 * 녹음 완료 이벤트
 * 녹음이 완료되었을 때 발생하는 이벤트입니다.
 */
class RecordingCompleteEvent : public Event
{
public:
    RecordingCompleteEvent(const juce::File& recordingFile)
        : Event(Type::RecordingComplete), file(recordingFile) {}
    
    const juce::File& getFile() const { return file; }
    
private:
    juce::File file;  // 녹음된 파일
};

/**
 * 곡 로드 완료 이벤트
 * 곡 로딩이 완료되었을 때 발생하는 이벤트입니다.
 */
class SongLoadedEvent : public Event
{
public:
    SongLoadedEvent(const juce::String& songId)
        : Event(Type::SongLoaded), id(songId) {}
    
    const juce::String& getSongId() const { return id; }
    
private:
    juce::String id;  // 로드된 곡 ID
};

/**
 * 곡 로드 실패 이벤트
 * 곡 로딩이 실패했을 때 발생하는 이벤트입니다.
 */
class SongLoadFailedEvent : public Event
{
public:
    SongLoadFailedEvent(const juce::String& songId, const juce::String& errorMsg)
        : Event(Type::SongLoadFailed), id(songId), errorMessage(errorMsg) {}
    
    const juce::String& getSongId() const { return id; }
    const juce::String& getErrorMessage() const { return errorMessage; }
    
private:
    juce::String id;  // 로드 시도한 곡 ID
    juce::String errorMessage;
};