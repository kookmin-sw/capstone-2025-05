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
}; 