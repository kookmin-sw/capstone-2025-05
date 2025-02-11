#pragma once
#include <JuceHeader.h>
#include "../Config/AppConfig.h"
#include <unordered_map>

class SpotifyService
{
public:
    struct Album {
        juce::String id;
        juce::String name;
        juce::String artist;
        juce::String coverUrl;
        std::shared_ptr<juce::Image> coverImage;
        float alpha = 0.0f;

        Album(const Album& other)
            : id(other.id)
            , name(other.name)
            , artist(other.artist)
            , coverUrl(other.coverUrl)
            , coverImage(other.coverImage ? std::make_shared<juce::Image>(*other.coverImage) : nullptr)
            , alpha(other.alpha)
        {}

        Album& operator=(const Album& other)
        {
            id = other.id;
            name = other.name;
            artist = other.artist;
            coverUrl = other.coverUrl;
            coverImage = other.coverImage ? std::make_shared<juce::Image>(*other.coverImage) : nullptr;
            alpha = other.alpha;
            return *this;
        }

        Album() = default;
    };

    // 이미지 로딩 - 비동기만 제공
    static void loadAlbumCoverAsync(const juce::String& url,
                                  std::function<void(std::shared_ptr<juce::Image>)> callback);

    // 앨범 검색 - 비동기만 제공
    static void searchAlbumsAsync(const juce::String& query, 
                                std::function<void(juce::Array<Album>)> callback);

    // 캐시 관련 함수 추가
    static void preloadData(std::function<void()> onComplete);
    static std::shared_ptr<juce::Image> getCachedImage(const juce::String& url);
    static const juce::Array<Album>& getCachedAlbums() { return cachedAlbums; }

    // 리소스 정리 함수 추가
    static void cleanup()
    {
        // 이미지 캐시 정리
        // for (auto& pair : imageCache)
        // {
        //     pair.second.reset();
        // }
        imageCache.clear();
        
        // // 앨범 데이터 정리
        // for (auto& album : cachedAlbums)
        // {
        //     album.coverImage.reset();
        // }
        // cachedAlbums.clear();
        
        isPreloaded = false;
    }

private:
    // 토큰 관리 - 내부용 동기/비동기 모두 제공
    static juce::String getAccessToken();
    static void getAccessTokenAsync(std::function<void(juce::String)> callback);
    
    static juce::String getClientId() { return AppConfig::getConfigValue("SPOTIFY_CLIENT_ID", ""); }
    static juce::String getClientSecret() { return AppConfig::getConfigValue("SPOTIFY_CLIENT_SECRET", ""); }
    static const juce::String API_BASE_URL;
    
    // 내부용 이미지 로딩 함수
    static std::shared_ptr<juce::Image> loadAlbumCoverInternal(const juce::String& url);
    
    static std::unordered_map<juce::String, std::shared_ptr<juce::Image>> imageCache;
    static juce::Array<Album> cachedAlbums;
    static bool isPreloaded;
}; 