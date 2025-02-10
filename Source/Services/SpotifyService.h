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
            , coverImage(other.coverImage)
            , alpha(other.alpha)
        {}

        Album& operator=(const Album& other)
        {
            id = other.id;
            name = other.name;
            artist = other.artist;
            coverUrl = other.coverUrl;
            coverImage = other.coverImage;
            alpha = other.alpha;
            return *this;
        }

        Album() = default;
    };

    static juce::Array<Album> searchAlbums(const juce::String& query);
    static std::shared_ptr<juce::Image> loadAlbumCover(const juce::String& url);

private:
    static juce::String getAccessToken();
    static juce::String getClientId() { return AppConfig::getConfigValue("SPOTIFY_CLIENT_ID", ""); }
    static juce::String getClientSecret() { return AppConfig::getConfigValue("SPOTIFY_CLIENT_SECRET", ""); }
    static const juce::String API_BASE_URL;
    
    // 이미지 캐시에 추가로 검색 결과 캐시 추가
    static std::unordered_map<juce::String, std::shared_ptr<juce::Image>> imageCache;
    static std::unordered_map<juce::String, juce::Array<Album>> searchCache;
}; 