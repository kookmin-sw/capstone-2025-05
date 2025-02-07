#pragma once
#include <JuceHeader.h>

class AuthService
{
public:
    static constexpr const char* AUTH_ENDPOINT = "/auth/login";
    static constexpr const char* CONTENT_TYPE_HEADER = "Content-Type: application/json\r\n";

    struct LoginResponse
    {
        bool success;
        juce::String message;
        juce::String token;

        LoginResponse(bool s, const juce::String& m, const juce::String& t)
            : success(s), message(m), token(t) {}
    };

    struct LoginRequest
    {
        juce::String email;
        juce::String password;
    };

    struct ErrorMessages
    {
        static constexpr const char* INVALID_CREDENTIALS = "Invalid username or password";
        static constexpr const char* INVALID_FORMAT = "Invalid input format";
        static constexpr const char* GENERIC_ERROR = "Login failed. Please try again later.";
        static constexpr const char* SUCCESS = "Login successful";
    };

    static LoginResponse login(const LoginRequest& request);

private:
    static LoginResponse handleSuccessResponse(const juce::var& responseJson);
    static LoginResponse handleErrorResponse(int statusCode);
    static juce::URL createAuthUrl();
}; 