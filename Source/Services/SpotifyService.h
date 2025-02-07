#pragma once
#include <JuceHeader.h>
#include "../Config/AppConfig.h"

class SpotifyService
{
public:
    struct Album {
        juce::String id;
        juce::String name;
        juce::String artist;
        juce::String coverUrl;
        std::unique_ptr<juce::Image> coverImage;

        Album(const Album& other)
            : id(other.id)
            , name(other.name)
            , artist(other.artist)
            , coverUrl(other.coverUrl)
            , coverImage(other.coverImage ? std::make_unique<juce::Image>(*other.coverImage) : nullptr)
        {}

        Album& operator=(const Album& other)
        {
            id = other.id;
            name = other.name;
            artist = other.artist;
            coverUrl = other.coverUrl;
            coverImage = other.coverImage ? std::make_unique<juce::Image>(*other.coverImage) : nullptr;
            return *this;
        }

        Album() = default;
    };

    static juce::Array<Album> searchAlbums(const juce::String& query);
    static std::unique_ptr<juce::Image> loadAlbumCover(const juce::String& url);

private:
    static juce::String getAccessToken();
    static juce::String getClientId() { return AppConfig::getConfigValue("SPOTIFY_CLIENT_ID", ""); }
    static juce::String getClientSecret() { return AppConfig::getConfigValue("SPOTIFY_CLIENT_SECRET", ""); }
    static const juce::String API_BASE_URL;
}; 