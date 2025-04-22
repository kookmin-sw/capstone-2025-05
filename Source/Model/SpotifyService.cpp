#include "SpotifyService.h"

const juce::String SpotifyService::API_BASE_URL = "https://api.spotify.com/v1";

// 정적 멤버 변수 초기화
std::unordered_map<juce::String, std::shared_ptr<juce::Image>> SpotifyService::imageCache;
juce::Array<SpotifyService::Album> SpotifyService::cachedAlbums;
bool SpotifyService::isPreloaded = false;

juce::String SpotifyService::getAccessToken()
{
    static juce::String cachedToken;
    static juce::Time tokenExpiry;
    
    if (tokenExpiry > juce::Time::getCurrentTime())
    {
        // DBG("Using cached token: " + cachedToken.substring(0, 10) + "...");
        return cachedToken;
    }
    
    const auto clientId = getClientId();
    const auto clientSecret = getClientSecret();
    if (clientId.isEmpty() || clientSecret.isEmpty())
    {
        // DBG("Error: Client ID or Secret is missing");
        return {};
    }

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
        // DBG("Token API response: " + responseStr);
        
        if (auto response = juce::JSON::parse(responseStr))
        {
            cachedToken = response.getProperty("access_token", "").toString();
            tokenExpiry = juce::Time::getCurrentTime() + juce::RelativeTime::hours(1);
            // DBG("Token acquired: " + cachedToken.substring(0, 10) + "...");
            return cachedToken;
        }
        else
        {
            // DBG("Error: Failed to parse token response");
        }
    }
    else
    {
        // DBG("Error: Failed to connect to token endpoint");
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
    // DBG("Starting searchAlbumsAsync for query: " + query);
    getAccessTokenAsync([query, callback](const juce::String& token) {
        // DBG("Received token for search: " + (token.isEmpty() ? "empty" : token.substring(0, 10) + "..."));
        if (token.isEmpty())
        {
            // DBG("Token is empty, aborting search");
            juce::MessageManager::callAsync([callback]() { callback({}); });
            return;
        }

        std::thread([query, token, callback]() {
            // DBG("Inside search thread for query: " + query);
            juce::Array<Album> albums;
            
            juce::URL url(API_BASE_URL + "/search?type=album&q=" + juce::URL::addEscapeChars(query, true));
            // DBG("Search URL: " + url.toString(true));
            
            auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withExtraHeaders("Authorization: Bearer " + token);
            
            if (auto stream = url.createInputStream(options))
            {
                // DBG("Connected to search endpoint");
                auto responseStr = stream->readEntireStreamAsString();
                // DBG("Search API raw response: " + responseStr.substring(0, 200) + "..."); // 너무 길면 잘라서 출력
                
                if (auto response = juce::JSON::parse(responseStr))
                {
                    // DBG("Parsed JSON successfully");
                    if (auto* items = response.getProperty("albums", {})
                                         .getProperty("items", {}).getArray())
                    {
                        // DBG("Found " + juce::String(items->size()) + " items in response");
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
                                    album.coverUrl = images->getFirst().getProperty("url", "").toString();
                            }
                            albums.add(album);
                            // DBG("Added album: " + album.name);
                        }
                    }
                    else
                    {
                        // DBG("No 'albums.items' found in response");
                    }
                }
                else
                {
                    // DBG("Failed to parse search response");
                }
            }
            else
            {
                // DBG("Failed to connect to search endpoint");
            }
            
            // DBG("Calling search callback with " + juce::String(albums.size()) + " albums");
            juce::MessageManager::callAsync([callback, albums]() mutable {
                callback(std::move(albums));
            });
        }).detach();
    });
}

std::shared_ptr<juce::Image> SpotifyService::loadAlbumCoverInternal(const juce::String& url)
{
    // DBG("Loading album cover from URL: " + url);
    juce::URL imageUrl(url);
    
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withConnectionTimeoutMs(5000)
        .withExtraHeaders("User-Agent: Mozilla/5.0\r\n"
                         "Accept: image/jpeg,image/png,image/*");
    
    if (auto stream = imageUrl.createInputStream(options))
    {
        // DBG("Stream created successfully");
        
        // 바이너리 데이터로 읽기
        juce::MemoryBlock block;
        stream->readIntoMemoryBlock(block);
        
        // DBG("Read " + juce::String(block.getSize()) + " bytes");
        
        // 메모리 스트림으로 변환
        juce::MemoryInputStream memStream(block.getData(), block.getSize(), false);
        auto image = juce::ImageFileFormat::loadFrom(memStream);
        
        // DBG("Image loaded: " + juce::String(image.isValid() ? "success" : "failed") + " Size: " + juce::String(image.getWidth()) + "x" + juce::String(image.getHeight()));
            
        if (image.isValid())
            return std::make_shared<juce::Image>(image);
    }
    else
    {
        // DBG("Failed to create stream for image URL");
    }
    return nullptr;
}

void SpotifyService::preloadData(std::function<void()> onComplete)
{
    if (isPreloaded)
    {
        onComplete();
        return;
    }

    searchAlbumsAsync("Younha", [onComplete](juce::Array<Album> albums) {
        cachedAlbums = std::move(albums);
        
        // 공유 카운터를 위한 스마트 포인터
        auto remainingCounter = std::make_shared<int>(cachedAlbums.size());
        
        for (auto& album : cachedAlbums)
        {
            if (!album.coverUrl.isEmpty())
            {
                // 앨범 참조를 복사로 변경
                auto coverUrl = album.coverUrl;
                loadAlbumCoverAsync(coverUrl, 
                    [remainingCounter, onComplete, coverUrl](std::shared_ptr<juce::Image> image) {
                        if (image)
                        {
                            imageCache[coverUrl] = image;
                            
                            // 앨범 객체 업데이트는 여기서 하지 않음
                            // 대신 HomePage에서 getCachedImage를 통해 이미지를 가져오도록 함
                        }
                        
                        (*remainingCounter)--;
                        if (*remainingCounter == 0)
                        {
                            isPreloaded = true;
                            onComplete();
                        }
                    });
            }
            else
            {
                (*remainingCounter)--;
            }
        }
        
        // 모든 이미지가 비어있는 경우를 처리
        if (*remainingCounter == 0)
        {
            isPreloaded = true;
            onComplete();
        }
    });
}

std::shared_ptr<juce::Image> SpotifyService::getCachedImage(const juce::String& url)
{
    auto it = imageCache.find(url);
    return (it != imageCache.end()) ? it->second : nullptr;
}

void SpotifyService::loadAlbumCoverAsync(const juce::String& url,
                                       std::function<void(std::shared_ptr<juce::Image>)> callback)
{
    // 캐시 확인
    if (auto cachedImage = getCachedImage(url))
    {
        callback(cachedImage);
        return;
    }

    // 캐시에 없으면 로드
    std::thread([url, callback]() {
        auto image = loadAlbumCoverInternal(url);
        if (image)
        {
            imageCache[url] = image;  // 캐시에 저장
        }
        juce::MessageManager::callAsync([callback, image]() {
            callback(image);
        });
    }).detach();
} 