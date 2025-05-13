#pragma once
#include <JuceHeader.h>
#include <functional>
#include "../Model/Song.h"

// API 응답 구조체
struct ApiResponse {
    bool success;
    juce::String errorMessage;
    juce::var data;
};

// 곡 정보 API 서비스 클래스
class SongsAPIService
{
public:
    SongsAPIService();
    ~SongsAPIService() = default;
    
    // 모든 곡 목록 가져오기
    void getAllSongs(std::function<void(ApiResponse)> callback);
    
    // 특정 곡 정보 가져오기
    void getSongById(const juce::String& songId, std::function<void(ApiResponse)> callback);
    
    // 썸네일 이미지 다운로드 메서드 추가
    void downloadSongThumbnail(const juce::String& songId, std::function<void(bool success, juce::Image image)> callback);
    
    // 이미지 다운로드 메서드
    void downloadImage(const juce::String& imageUrl, std::function<void(bool success, juce::Image image)> callback);
    
    // 오디오 파일 다운로드 메서드
    void downloadAudioFile(const juce::String& audioUrl, const juce::File& destinationFile, 
                          std::function<void(bool success, juce::String filePath)> callback);
    
    // 악보 데이터 다운로드 메서드
    void downloadScoreData(const juce::String& scoreDataUrl, 
                          std::function<void(bool success, juce::String scoreData)> callback);
    
    // Song 객체 배열로 변환
    juce::Array<Song> parseSongsFromResponse(const ApiResponse& response);
    
    // API 기본 URL 설정
    void setApiBaseUrl(const juce::String& url) { apiBaseUrl = url; }
    
    // API 기본 URL 가져오기
    juce::String getApiBaseUrl() const { return apiBaseUrl; }
    
    // 기본 HTTP 헤더 설정
    void setDefaultHeaders(const juce::String& headers) { defaultHeaders = headers; }
    
    // 기본 HTTP 헤더 가져오기
    juce::String getDefaultHeaders() const { return defaultHeaders; }
    
    // 오디오 파일 다운로드 메서드 추가
    void downloadSongAudio(const juce::String& audioUrl, const juce::File& destinationFile, 
                          std::function<void(bool success, juce::String filePath)> callback);
    
private:
    juce::String apiBaseUrl;
    juce::String defaultHeaders;
    
    // API 호출 공통 메서드
    void makeGetRequest(const juce::String& endpoint, std::function<void(ApiResponse)> callback);
    
    // 파일 다운로드 공통 메서드
    void downloadFile(const juce::String& url, const juce::File& destinationFile,
                     std::function<void(bool success, juce::String filePath)> callback);
    
    // 캐시 디렉토리 가져오기
    juce::File getCacheDirectory();
    
    // 환경 변수에서 API URL 가져오기
    juce::String getApiUrlFromEnvironment();
};