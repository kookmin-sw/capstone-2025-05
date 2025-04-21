#pragma once
#include <JuceHeader.h>

#if JUCE_WINDOWS
    #include <Windows.h>
#endif

class EnvLoader
{
public:
    static void load()
    {
        juce::File envFile = juce::File::getCurrentWorkingDirectory().getChildFile(".env");
        if (envFile.existsAsFile())
        {
            auto content = envFile.loadFileAsString();
            auto lines = juce::StringArray::fromLines(content);
            
            for (auto& line : lines)
            {
                if (line.isNotEmpty() && !line.startsWith("#"))
                {
                    auto parts = juce::StringArray::fromTokens(line, "=", "");
                    if (parts.size() == 2)
                    {
                        #if JUCE_WINDOWS
                            SetEnvironmentVariableA(parts[0].trim().toRawUTF8(),
                                                  parts[1].trim().toRawUTF8());
                        #else
                            setenv(parts[0].trim().toRawUTF8(),
                                  parts[1].trim().toRawUTF8(), 1);
                        #endif
                    }
                }
            }
        }
    }
    
    // 환경변수 값을 가져오는 유틸리티 함수
    static juce::String get(const juce::String& name)
    {
        #if JUCE_WINDOWS
            char buffer[32767] = { 0 }; // Windows 환경변수의 최대 크기
            DWORD size = GetEnvironmentVariableA(name.toRawUTF8(), buffer, sizeof(buffer));
            return size > 0 ? juce::String(buffer) : juce::String();
        #else
            const char* value = getenv(name.toRawUTF8());
            return value != nullptr ? juce::String(value) : juce::String();
        #endif
    }
    
    // 환경변수 값이 존재하는지 확인하는 유틸리티 함수
    static bool exists(const juce::String& name)
    {
        return !get(name).isEmpty();
    }
};