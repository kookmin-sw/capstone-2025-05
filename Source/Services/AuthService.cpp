#include "AuthService.h"
#include "../Config/AppConfig.h"

AuthService::LoginResponse AuthService::login(const LoginRequest& request)
{
    juce::URL url = createAuthUrl();
    
    // JSON 데이터 생성
    juce::var jsonData(new juce::DynamicObject());
    jsonData.getDynamicObject()->setProperty("email", request.email);
    jsonData.getDynamicObject()->setProperty("password", request.password);
    
    juce::String jsonString = juce::JSON::toString(jsonData);
    url = url.withPOSTData(jsonString);

    int statusCode = 0;
    juce::StringPairArray responseHeaders;

    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders(CONTENT_TYPE_HEADER)
        .withStatusCode(&statusCode)
        .withResponseHeaders(&responseHeaders);

    std::unique_ptr<juce::InputStream> stream = url.createInputStream(options);

    if (stream && statusCode == 200)
    {
        juce::String response = stream->readEntireStreamAsString();
        auto responseJson = juce::JSON::parse(response);
        return handleSuccessResponse(responseJson);
    }
    
    return handleErrorResponse(statusCode);
}

AuthService::LoginResponse AuthService::handleSuccessResponse(const juce::var& responseJson)
{
    if (auto* obj = responseJson.getDynamicObject())
    {
        auto accessToken = responseJson.getProperty("access_token", "").toString();
        if (accessToken.isNotEmpty())
        {
            return LoginResponse(true, ErrorMessages::SUCCESS, accessToken);
        }
    }
    return LoginResponse(false, ErrorMessages::GENERIC_ERROR, {});
}

AuthService::LoginResponse AuthService::handleErrorResponse(int statusCode)
{
    switch (statusCode)
    {
        case 401: return LoginResponse(false, ErrorMessages::INVALID_CREDENTIALS, {});
        case 422: return LoginResponse(false, ErrorMessages::INVALID_FORMAT, {});
        default:  return LoginResponse(false, ErrorMessages::GENERIC_ERROR, {});
    }
}

juce::URL AuthService::createAuthUrl()
{
    return juce::URL(AppConfig::getApiBaseUrl() + AUTH_ENDPOINT);
} 