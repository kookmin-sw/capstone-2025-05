#include "SpotifyService.h"
#include "../Config/AppConfig.h"

const juce::String SpotifyService::API_BASE_URL = "https://api.spotify.com/v1";

juce::String SpotifyService::getAccessToken()
{
    static juce::String cachedToken;
    static juce::Time tokenExpiry;
    
    if (tokenExpiry > juce::Time::getCurrentTime())
        return cachedToken;
    
    const auto clientId = getClientId();
    const auto clientSecret = getClientSecret();
    const juce::String postData = "grant_type=client_credentials";
    const auto auth = juce::Base64::toBase64(clientId + ":" + clientSecret);
    
    juce::URL url("https://accounts.spotify.com/api/token");
    url = url.withPOSTData(postData);
    
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders("Authorization: Basic " + auth + "\r\n"
                         "Content-Type: application/x-www-form-urlencoded")
        .withConnectionTimeoutMs(5000);
        
    if (auto stream = url.createInputStream(options))
    {
        auto responseStr = stream->readEntireStreamAsString();
        
        if (auto response = juce::JSON::parse(responseStr))
        {
            cachedToken = response.getProperty("access_token", "").toString();
            tokenExpiry = juce::Time::getCurrentTime() + juce::RelativeTime::hours(1);
            return cachedToken;
        }
    }
    
    return {};
}

void SpotifyService::getAccessTokenAsync(std::function<void(juce::String)> callback)
{
    std::thread([callback]() {
        auto token = getAccessToken();  // 동기 함수 재사용
        
        juce::MessageManager::callAsync([callback, token]() {
            callback(token);
        });
    }).detach();
}

void SpotifyService::searchAlbumsAsync(const juce::String& query, 
                                     std::function<void(juce::Array<Album>)> callback)
{
    getAccessTokenAsync([query, callback](const juce::String& token) {
        std::thread([query, token, callback]() {
            juce::Array<Album> albums;
            
            juce::URL url(API_BASE_URL + "/search?type=album&q=" + 
                         juce::URL::addEscapeChars(query, true));
            
            auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withExtraHeaders("Authorization: Bearer " + token);
            
            if (auto stream = url.createInputStream(options))
            {
                auto response = juce::JSON::parse(stream->readEntireStreamAsString());
                
                if (auto* items = response.getProperty("albums", {})
                                         .getProperty("items", {}).getArray())
                {
                    for (auto& item : *items)
                    {
                        Album album;
                        album.id = item.getProperty("id", "").toString();
                        album.name = item.getProperty("name", "").toString();
                        album.artist = item.getProperty("artists", {})
                                         .getArray()->getFirst()
                                         .getProperty("name", "").toString();
                                         
                        if (auto* images = item.getProperty("images", {}).getArray())
                        {
                            if (!images->isEmpty())
                            {
                                album.coverUrl = images->getFirst()
                                                     .getProperty("url", "").toString();
                            }
                        }
                        
                        albums.add(album);
                    }
                }
            }
            
            juce::MessageManager::callAsync([callback, albums]() mutable {
                callback(std::move(albums));
            });
        }).detach();
    });
}

std::shared_ptr<juce::Image> SpotifyService::loadAlbumCoverInternal(const juce::String& url)
{
    DBG("Loading album cover from URL: " + url);
    juce::URL imageUrl(url);
    
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withConnectionTimeoutMs(5000)
        .withExtraHeaders("User-Agent: Mozilla/5.0\r\n"
                         "Accept: image/jpeg,image/png,image/*");
    
    if (auto stream = imageUrl.createInputStream(options))
    {
        DBG("Stream created successfully");
        
        // 바이너리 데이터로 읽기
        juce::MemoryBlock block;
        stream->readIntoMemoryBlock(block);
        
        DBG("Read " + juce::String(block.getSize()) + " bytes");
        
        // 메모리 스트림으로 변환
        juce::MemoryInputStream memStream(block.getData(), block.getSize(), false);
        auto image = juce::ImageFileFormat::loadFrom(memStream);
        
        DBG("Image loaded: " + juce::String(image.isValid() ? "success" : "failed") + 
            " Size: " + juce::String(image.getWidth()) + "x" + juce::String(image.getHeight()));
            
        if (image.isValid())
            return std::make_shared<juce::Image>(image);
    }
    else
    {
        DBG("Failed to create stream for image URL");
    }
    return nullptr;
}

void SpotifyService::loadAlbumCoverAsync(const juce::String& url,
                                       std::function<void(std::shared_ptr<juce::Image>)> callback)
{
    std::thread([url, callback]() {
        auto image = loadAlbumCoverInternal(url);
        juce::MessageManager::callAsync([callback, image]() {
            callback(image);
        });
    }).detach();
} 