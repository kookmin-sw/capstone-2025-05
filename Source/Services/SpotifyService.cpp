#include "SpotifyService.h"
#include "../Config/AppConfig.h"

const juce::String SpotifyService::API_BASE_URL = "https://api.spotify.com/v1";

juce::Array<SpotifyService::Album> SpotifyService::searchAlbums(const juce::String& query)
{
    juce::Array<Album> albums;
    auto token = getAccessToken();
    
    DBG("Access Token: " + token);  // 토큰 확인
    
    juce::URL url(API_BASE_URL + "/search?type=album&q=" + juce::URL::addEscapeChars(query, true));
    DBG("Search URL: " + url.toString(false));  // URL 확인
    
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withExtraHeaders("Authorization: Bearer " + token);
    
    if (auto stream = url.createInputStream(options))
    {
        auto response = juce::JSON::parse(stream->readEntireStreamAsString());
        DBG("Search Response: " + response.toString());  // 응답 확인
        
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
                        DBG("Album Cover URL: " + album.coverUrl);  // URL 확인
                        album.coverImage = loadAlbumCover(album.coverUrl);
                        DBG(juce::String("Cover Image Loaded: ") + (album.coverImage != nullptr ? "true" : "false"));  // 이미지 로드 확인
                    }
                }
                

                albums.add(album);
            }
        }
    }
    
    return albums;
}

std::unique_ptr<juce::Image> SpotifyService::loadAlbumCover(const juce::String& url)
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
            return std::make_unique<juce::Image>(image);
    }
    else
    {
        DBG("Failed to create stream for image URL");
    }
    return nullptr;
}

juce::String SpotifyService::getAccessToken()
{
    static juce::String cachedToken;
    static juce::Time tokenExpiry;
    
    if (tokenExpiry > juce::Time::getCurrentTime())
        return cachedToken;
        
    const auto clientId = getClientId();
    const auto clientSecret = getClientSecret();
    
    DBG("Client ID: " + clientId);
    DBG("Client Secret length: " + juce::String(clientSecret.length()));
    
    const juce::String postData = "grant_type=client_credentials";
    const auto auth = juce::Base64::toBase64(clientId + ":" + clientSecret);
    
    juce::URL url("https://accounts.spotify.com/api/token");
    url = url.withPOSTData(postData);
    
    DBG("Token request URL: " + url.toString(false));
    DBG("Auth header: " + auth);
    
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders("Authorization: Basic " + auth + "\r\n"
                         "Content-Type: application/x-www-form-urlencoded")
        .withConnectionTimeoutMs(5000);
        
    if (auto stream = url.createInputStream(options))
    {
        auto responseStr = stream->readEntireStreamAsString();
        DBG("Token response: " + responseStr);  // 응답 확인
        
        if (auto response = juce::JSON::parse(responseStr))
        {
            cachedToken = response.getProperty("access_token", "").toString();
            return cachedToken;
        }
    }
    else
    {
        DBG("Failed to create stream for token request");
    }
    
    return {};
} 