#include "SongsAPIService.h"

SongsAPIService::SongsAPIService()
    : apiBaseUrl("http://localhost:8000"),
      defaultHeaders("Accept: application/json\r\n")
{
    DBG("SongsAPIService constructor - API base URL: " + apiBaseUrl);
    
    // 캐시 디렉토리 생성
    getCacheDirectory().createDirectory();
}

void SongsAPIService::makeGetRequest(const juce::String& endpoint, std::function<void(ApiResponse)> callback)
{
    DBG("SongsAPIService::makeGetRequest - start for endpoint: " + endpoint);
    DBG("SongsAPIService::makeGetRequest - URL: " + apiBaseUrl + endpoint);
    
    struct AsyncContext
    {
        std::function<void(ApiResponse)> callback;
        juce::URL url;
        juce::String extraHeaders;
    };
    
    auto context = std::make_shared<AsyncContext>();
    context->callback = callback;
    context->url = juce::URL(apiBaseUrl + endpoint);
    context->extraHeaders = defaultHeaders;
    
    // 비동기 HTTP 요청을 위한 스레드 생성
    juce::Thread::launch([context]()
    {
        int statusCode = 0;
        juce::StringPairArray responseHeaders;
        
        // HTTP 요청을 위한 옵션 설정
        auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withExtraHeaders(context->extraHeaders)
                .withConnectionTimeoutMs(5000)
                .withResponseHeaders(&responseHeaders)
                .withStatusCode(&statusCode);
        
        // 입력 스트림 생성
        std::unique_ptr<juce::InputStream> stream(context->url.createInputStream(options));
        
        ApiResponse apiResponse;
        
        if (stream != nullptr)
        {
            // 상태 코드 확인 (200번대가 아니면 에러로 처리)
            bool isSuccess = (statusCode >= 200 && statusCode < 300);
            
            apiResponse.success = isSuccess;
            
            // 데이터 읽기
            juce::String response = stream->readEntireStreamAsString();
            DBG("SongsAPIService::makeGetRequest - response (status " + juce::String(statusCode) + "): " + response);
            
            // JSON 파싱
            apiResponse.data = juce::JSON::parse(response);
            
            if (isSuccess)
            {
                DBG("SongsAPIService::makeGetRequest - request successful, status code: " + juce::String(statusCode));
                apiResponse.errorMessage = juce::String();
                
                // 파싱된 데이터 검증
                if (apiResponse.data.isArray())
                {
                    DBG("SongsAPIService::makeGetRequest - parsed as JSON array with " + 
                        juce::String(apiResponse.data.getArray()->size()) + " items");
                }
                else if (apiResponse.data.isObject())
                {
                    DBG("SongsAPIService::makeGetRequest - parsed as JSON object");
                    // 객체 내부 구조 확인
                    auto* obj = apiResponse.data.getDynamicObject();
                    if (obj != nullptr)
                    {
                        DBG("SongsAPIService::makeGetRequest - object has " + 
                            juce::String(obj->getProperties().size()) + " properties");
                        
                        // 처음 10개 프로퍼티만 출력
                        int count = 0;
                        for (const auto& prop : obj->getProperties())
                        {
                            DBG("  - " + prop.name.toString() + ": " + prop.value.toString());
                            if (++count >= 10)
                                break;
                        }
                    }
                }
                else
                {
                    DBG("SongsAPIService::makeGetRequest - parsed JSON is neither array nor object");
                }
            }
            else
            {
                // HTTP 에러 처리
                apiResponse.errorMessage = "HTTP error: " + juce::String(statusCode) + " - " + response;
                DBG("SongsAPIService::makeGetRequest - HTTP error: " + juce::String(statusCode));
            }
        }
        else
        {
            // 에러 처리
            apiResponse.success = false;
            apiResponse.errorMessage = "Failed to connect to server";
            apiResponse.data = juce::var();
            
            DBG("SongsAPIService::makeGetRequest - connection failed, status code: " + juce::String(statusCode));
            
            // StringPairArray 내용 출력 (범위 기반 for 루프 대신 다른 방식 사용)
            DBG("Response headers count: " + juce::String(responseHeaders.size()));
            for (int i = 0; i < responseHeaders.size(); ++i)
            {
                DBG("  Header: " + responseHeaders.getAllKeys()[i] + " = " + responseHeaders.getAllValues()[i]);
            }
        }
        
        // 메인 스레드에서 콜백 호출
        juce::MessageManager::callAsync([context, apiResponse]()
        {
            if (context->callback)
                context->callback(apiResponse);
        });
    });
}

void SongsAPIService::downloadFile(const juce::String& url, const juce::File& destinationFile,
                                  std::function<void(bool success, juce::String filePath)> callback)
{
    // URL이 상대경로인지 확인하고 절대경로로 변환
    juce::String fullUrl = url.startsWith("http") ? url : apiBaseUrl + url;
    DBG("SongsAPIService::downloadFile - start for URL: " + fullUrl);
    
    juce::Thread::launch([fullUrl, destinationFile, callback]()
    {
        juce::URL url(fullUrl);
        int statusCode = 0;
        juce::StringPairArray responseHeaders;
        
        // 대상 파일의 디렉토리가 없으면 생성
        if (!destinationFile.getParentDirectory().exists())
            destinationFile.getParentDirectory().createDirectory();
        
        // HTTP 요청을 위한 옵션 설정
        auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withConnectionTimeoutMs(15000)
                .withResponseHeaders(&responseHeaders)
                .withStatusCode(&statusCode);
                
        // 입력 스트림 생성
        std::unique_ptr<juce::InputStream> stream(url.createInputStream(options));
        
        if (stream != nullptr)
        {
            // 헤더 확인
            DBG("SongsAPIService::downloadFile - response headers: ");
            for (int i = 0; i < responseHeaders.size(); ++i)
            {
                DBG("  " + responseHeaders.getAllKeys()[i] + ": " + responseHeaders.getAllValues()[i]);
            }
            
            // content-disposition 헤더에서 파일 이름 추출
            juce::File actualDestFile = destinationFile;
            if (responseHeaders.containsKey("content-disposition"))
            {
                juce::String disposition = responseHeaders["content-disposition"];
                DBG("SongsAPIService::downloadFile - Content-Disposition: " + disposition);
                
                if (disposition.containsIgnoreCase("filename="))
                {
                    int filenamePos = disposition.indexOfIgnoreCase("filename=");
                    if (filenamePos != -1)
                    {
                        // filename="example.mp3" 형식에서 파일명 추출
                        juce::String filenameSection = disposition.substring(filenamePos + 9);
                        juce::String filename;
                        
                        if (filenameSection.startsWith("\""))
                        {
                            // 따옴표로 둘러싸인 경우 (indexOf 대신 다른 방식으로 처리)
                            juce::String quotePart = filenameSection.substring(1);
                            int endQuotePos = quotePart.indexOf("\"");
                            if (endQuotePos != -1)
                                filename = filenameSection.substring(1, endQuotePos + 1);
                        }
                        else
                        {
                            // 따옴표 없는 경우
                            int endPos = filenameSection.indexOf(";");
                            filename = endPos == -1 ? filenameSection : filenameSection.substring(0, endPos);
                        }
                        
                        if (filename.isNotEmpty())
                        {
                            DBG("SongsAPIService::downloadFile - Extracted filename: " + filename);
                            // 원래 경로에 서버가 제공한 파일명을 사용
                            actualDestFile = destinationFile.getParentDirectory().getChildFile(filename);
                            DBG("SongsAPIService::downloadFile - Using path: " + actualDestFile.getFullPathName());
                        }
                    }
                }
            }
            
            // 파일로 저장
            juce::FileOutputStream outputStream(actualDestFile);
            
            if (outputStream.openedOk())
            {
                // 다운로드된 데이터를 파일에 기록
                outputStream.writeFromInputStream(*stream, -1);
                outputStream.flush();
                
                // 메인 스레드에서 콜백 호출
                juce::MessageManager::callAsync([callback, actualDestFile]()
                {
                    if (callback)
                        callback(true, actualDestFile.getFullPathName());
                });
            }
            else
            {
                // 파일 쓰기 오류
                juce::MessageManager::callAsync([callback]()
                {
                    if (callback)
                        callback(false, "Failed to write to file");
                });
            }
        }
        else
        {
            // 다운로드 오류
            DBG("SongsAPIService::downloadFile - failed to create stream, status: " + juce::String(statusCode));
            juce::MessageManager::callAsync([callback]()
            {
                if (callback)
                    callback(false, "Failed to download file");
            });
        }
    });
}

void SongsAPIService::downloadImage(const juce::String& imageUrl, std::function<void(bool success, juce::Image image)> callback)
{
    DBG("SongsAPIService::downloadImage - start for URL: " + imageUrl);
    
    // 이미지 URL이 상대경로인지 확인하고 절대경로로 변환
    juce::String fullUrl;
    if (imageUrl.startsWith("http"))
    {
        fullUrl = imageUrl;
    }
    else if (imageUrl.startsWith("/"))
    {
        // /로 시작하는 상대 경로 처리
        fullUrl = apiBaseUrl + imageUrl;
    }
    else
    {
        // 그 외 경로 처리
        fullUrl = apiBaseUrl + "/" + imageUrl;
    }
    
    DBG("SongsAPIService::downloadImage - full URL: " + fullUrl);
    
    juce::Thread::launch([fullUrl, callback]()
    {
        juce::URL url(fullUrl);
        int statusCode = 0;
        juce::StringPairArray responseHeaders;
        
        // HTTP 요청을 위한 옵션 설정
        auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withConnectionTimeoutMs(10000)
                .withResponseHeaders(&responseHeaders)
                .withStatusCode(&statusCode);
                
        // 입력 스트림 생성
        std::unique_ptr<juce::InputStream> stream(url.createInputStream(options));
        
        if (stream != nullptr)
        {
            DBG("SongsAPIService::downloadImage - stream created successfully, status: " + juce::String(statusCode));
            
            // 이미지 데이터를 읽어 메모리에 저장
            juce::MemoryBlock memoryBlock;
            stream->readIntoMemoryBlock(memoryBlock);
            
            // 메모리 블록에서 이미지 생성
            juce::MemoryInputStream memStream(memoryBlock, false);
            juce::Image image = juce::ImageFileFormat::loadFrom(memStream);
            
            if (image.isValid())
            {
                DBG("SongsAPIService::downloadImage - image loaded successfully, size: " + 
                    juce::String(image.getWidth()) + "x" + juce::String(image.getHeight()));
                
                // 메인 스레드에서 콜백 호출
                juce::MessageManager::callAsync([callback, image]()
                {
                    if (callback)
                        callback(true, image);
                });
            }
            else
            {
                DBG("SongsAPIService::downloadImage - failed to create valid image");
                juce::MessageManager::callAsync([callback]()
                {
                    if (callback)
                        callback(false, juce::Image());
                });
            }
        }
        else
        {
            // 에러 처리
            DBG("SongsAPIService::downloadImage - failed to create stream, status: " + juce::String(statusCode));
            juce::MessageManager::callAsync([callback]()
            {
                if (callback)
                    callback(false, juce::Image());
            });
        }
    });
}

void SongsAPIService::downloadAudioFile(const juce::String& audioUrl, const juce::File& destinationFile,
                                      std::function<void(bool success, juce::String filePath)> callback)
{
    // 오디오 파일은 다른 Accept 헤더를 사용해야 할 수 있음 (application/json 대신 audio/*)
    juce::String originalHeaders = defaultHeaders;
    juce::String audioHeaders = "Accept: audio/*\r\n";
    
    // 임시로 헤더 변경
    setDefaultHeaders(audioHeaders);
    
    // 파일 다운로드 수행
    downloadFile(audioUrl, destinationFile, [this, originalHeaders, callback](bool success, juce::String filePath) {
        // 헤더 복원
        setDefaultHeaders(originalHeaders);
        
        // 콜백 호출
        if (callback)
            callback(success, filePath);
    });
}

void SongsAPIService::downloadScoreData(const juce::String& scoreDataUrl,
                                      std::function<void(bool success, juce::String scoreData)> callback)
{
    // 악보 데이터는 텍스트로 직접 불러옴
    juce::String fullUrl = scoreDataUrl.startsWith("http") ? scoreDataUrl : apiBaseUrl + scoreDataUrl;
    DBG("SongsAPIService::downloadScoreData - start for URL: " + fullUrl);
    
    juce::Thread::launch([fullUrl, callback]()
    {
        juce::URL url(fullUrl);
        int statusCode = 0;
        juce::StringPairArray responseHeaders;
        
        // HTTP 요청을 위한 옵션 설정
        auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
                .withConnectionTimeoutMs(15000)  // 15초 타임아웃
                .withResponseHeaders(&responseHeaders)
                .withStatusCode(&statusCode);
                
        // 입력 스트림 생성
        std::unique_ptr<juce::InputStream> stream(url.createInputStream(options));
        
        if (stream != nullptr)
        {
            // 응답 헤더 로깅
            DBG("SongsAPIService::downloadScoreData - response headers: ");
            for (int i = 0; i < responseHeaders.size(); ++i)
            {
                DBG("  " + responseHeaders.getAllKeys()[i] + ": " + responseHeaders.getAllValues()[i]);
            }
            
            // Content-Type 확인
            juce::String contentType = responseHeaders["content-type"];
            DBG("SongsAPIService::downloadScoreData - Content-Type: " + contentType);
            
            // 서버에서 오는 모든 데이터를 Guitar Pro 파일로 취급
            DBG("SongsAPIService::downloadScoreData - treating all data as Guitar Pro file regardless of Content-Type");
            
            // 메모리에 데이터 로드
            juce::MemoryBlock memoryBlock;
            stream->readIntoMemoryBlock(memoryBlock);
            
            // 데이터 크기 확인
            DBG("SongsAPIService::downloadScoreData - received data size: " + juce::String(memoryBlock.getSize()) + " bytes");
            
            // 데이터의 첫 100바이트 확인 (16진수로 표시)
            juce::String hexDump;
            const char* data = static_cast<const char*>(memoryBlock.getData());
            int bytesToShow = juce::jmin(100, static_cast<int>(memoryBlock.getSize()));
            
            for (int i = 0; i < bytesToShow; ++i)
            {
                unsigned char byte = static_cast<unsigned char>(data[i]);
                hexDump += juce::String::formatted("%02X ", byte);
                
                // 10바이트마다 줄바꿈
                if ((i + 1) % 10 == 0)
                    hexDump += "\n";
            }
            
            DBG("SongsAPIService::downloadScoreData - First bytes of data:\n" + hexDump);
            
            // 텍스트 파일인지 확인하기 위해 첫 100바이트를 문자열로 표시
            juce::String textPreview;
            for (int i = 0; i < bytesToShow; ++i)
            {
                char c = data[i];
                // 출력 가능한 ASCII 문자만 표시
                if (c >= 32 && c <= 126)
                    textPreview += c;
                else
                    textPreview += '.'; // 비ASCII 문자는 점으로 표시
            }
            
            DBG("SongsAPIService::downloadScoreData - Text preview:\n" + textPreview);
            
            // GP5 헤더 확인 및 수정
            juce::MemoryBlock cleanedBlock;
            
            // 첫 바이트가 "FICHIER"의 ASCII 값이 아니면 스킵해야 함
            bool needsCleaning = false;
            int offsetToSkip = 0;
            
            // "FICHIER " 문자열 탐색 (ASCII: 70 73 67 72 73 69 82 32)
            for (size_t i = 0; i < memoryBlock.getSize() - 8; ++i) {
                const unsigned char* bytes = static_cast<const unsigned char*>(memoryBlock.getData());
                
                // "FICHIER" 시작 부분 검색
                if (bytes[i] == 'F' && bytes[i+1] == 'I' && bytes[i+2] == 'C' && 
                    bytes[i+3] == 'H' && bytes[i+4] == 'I' && bytes[i+5] == 'E' && 
                    bytes[i+6] == 'R' && bytes[i+7] == ' ') {
                    needsCleaning = (i > 0); // 0이 아닌 위치에서 발견되면 정리 필요
                    offsetToSkip = static_cast<int>(i);
                    DBG("SongsAPIService::downloadScoreData - Found GP5 header at offset: " + juce::String(offsetToSkip));
                    break;
                }
            }
            
            if (needsCleaning && offsetToSkip > 0) {
                // 오프셋 이후의 데이터만 사용
                const char* rawData = static_cast<const char*>(memoryBlock.getData());
                size_t newSize = memoryBlock.getSize() - offsetToSkip;
                
                cleanedBlock.setSize(newSize);
                cleanedBlock.copyFrom(rawData + offsetToSkip, 0, newSize);
                
                DBG("SongsAPIService::downloadScoreData - Cleaned data by removing " + juce::String(offsetToSkip) + 
                    " bytes from the beginning, new size: " + juce::String(cleanedBlock.getSize()) + " bytes");
                
                // 정리된 데이터의 헤더 확인
                if (cleanedBlock.getSize() > 20) {
                    juce::String cleanedHeader;
                    const char* cleanedData = static_cast<const char*>(cleanedBlock.getData());
                    
                    for (int i = 0; i < 20; ++i) {
                        unsigned char byte = static_cast<unsigned char>(cleanedData[i]);
                        cleanedHeader += juce::String::formatted("%02X ", byte);
                    }
                    
                    DBG("SongsAPIService::downloadScoreData - Cleaned data header: " + cleanedHeader);
                }
            } else {
                // 정리가 필요 없으면 원본 데이터 사용
                cleanedBlock = memoryBlock;
            }
            
            // Base64로 인코딩하여 Guitar Pro 파일 MIME 타입으로 지정
            juce::String base64Data = cleanedBlock.toBase64Encoding();
            juce::String scoreData = "data:application/x-guitar-pro;base64," + base64Data;
            
            DBG("SongsAPIService::downloadScoreData - encoded data as Guitar Pro file, length: " + 
                juce::String(scoreData.length()) + " chars");
            
            // 메인 스레드에서 콜백 호출
            juce::MessageManager::callAsync([callback, scoreData]()
            {
                if (callback)
                    callback(true, scoreData);
            });
        }
        else
        {
            // 다운로드 오류
            DBG("SongsAPIService::downloadScoreData - failed to create stream, status: " + juce::String(statusCode));
            
            // 응답 헤더 확인 (존재하는 경우)
            if (responseHeaders.size() > 0)
            {
                DBG("SongsAPIService::downloadScoreData - response headers: ");
                for (int i = 0; i < responseHeaders.size(); ++i)
                {
                    DBG("  " + responseHeaders.getAllKeys()[i] + ": " + responseHeaders.getAllValues()[i]);
                }
            }
            
            juce::MessageManager::callAsync([callback]()
            {
                if (callback)
                    callback(false, "Failed to download score data");
            });
        }
    });
}

juce::File SongsAPIService::getCacheDirectory()
{
    // 앱의 로컬 데이터 디렉토리 내에 캐시 폴더 생성
    juce::File cacheDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                          .getChildFile("MapleClientDesktop/cache");
    return cacheDir;
}

void SongsAPIService::getAllSongs(std::function<void(ApiResponse)> callback)
{
    makeGetRequest("/songs", callback);
}

void SongsAPIService::getSongById(const juce::String& songId, std::function<void(ApiResponse)> callback)
{
    makeGetRequest("/songs/" + songId, callback);
}

juce::Array<Song> SongsAPIService::parseSongsFromResponse(const ApiResponse& response)
{
    if (!response.success || !response.data.isArray())
        return juce::Array<Song>();
        
    return Song::fromJsonArray(response.data);
} 