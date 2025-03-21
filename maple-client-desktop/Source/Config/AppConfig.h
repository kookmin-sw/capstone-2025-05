#pragma once
#include <JuceHeader.h>

class AppConfig
{
public:
    static void init()
    {
        loadEnvFile();
    }

    static juce::String getApiBaseUrl()
    {
        static const juce::String defaultUrl = "http://localhost:8000/api/v1";
        return getConfigValue("MAPLE_API_URL", defaultUrl);
    }

    static juce::String getConfigValue(const juce::String& key, const juce::String& defaultValue)
    {
        return configMap.contains(key) ? configMap[key] : defaultValue;
    }

private:
    static void loadEnvFile()
    {
        auto envFile = juce::File::getCurrentWorkingDirectory().getChildFile(".env");
        
        if (envFile.existsAsFile())
        {
            configMap.clear();
            auto content = envFile.loadFileAsString();
            DBG("Env file content: " + content);
            auto lines = juce::StringArray::fromLines(content);
            
            for (auto& line : lines)
            {
                if (line.isNotEmpty() && !line.startsWith("#"))
                {
                    auto parts = juce::StringArray::fromTokens(line, "=", "");
                    if (parts.size() == 2)
                    {
                        configMap.set(parts[0].trim(), parts[1].trim());
                    }
                }
            }
        }
    }

    static inline juce::HashMap<juce::String, juce::String> configMap;
}; 