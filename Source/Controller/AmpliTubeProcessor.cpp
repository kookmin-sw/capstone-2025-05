#include "AmpliTubeProcessor.h"

AmpliTubeProcessor::AmpliTubeProcessor()
{
    formatManager = std::make_unique<juce::AudioPluginFormatManager>();
    formatManager->addDefaultFormats();
}

AmpliTubeProcessor::~AmpliTubeProcessor()
{
    // 플러그인 에디터 먼저 정리
    editor = nullptr;
    // 그 다음 플러그인 정리
    plugin = nullptr;
}

bool AmpliTubeProcessor::init()
{
    // 플러그인 로드 시도
    plugin.reset(findAndLoadPlugin());
    
    if (plugin == nullptr)
    {
        DBG("AmpliTubeProcessor: Failed to load plugin");
        return false;
    }
    
    // 성공적으로 로드했을 경우 에디터 생성
    editor.reset(plugin->createEditor());
    
    if (editor == nullptr)
    {
        DBG("AmpliTubeProcessor: Failed to create plugin editor");
        return false;
    }
    
    // 초기 버퍼 크기 설정
    tempBuffer.setSize(2, currentBlockSize);
    
    // 플러그인 초기화
    plugin->prepareToPlay(currentSampleRate, currentBlockSize);
    
    // 초기화는 완료했지만 프로세싱은 기본적으로 비활성화 상태로 유지
    // 외부에서 명시적으로 setProcessingEnabled(true)를 호출해야 활성화됨
    processingEnabled = false;
    
    DBG("AmpliTubeProcessor: Initialized successfully, but processing is disabled by default");
    return true;
}

void AmpliTubeProcessor::processBlock(const float* const* inputChannelData, 
                                     int numInputChannels,
                                     float* const* outputChannelData, 
                                     int numOutputChannels,
                                     int numSamples)
{
    if (!processingEnabled || plugin == nullptr)
    {
        // 프로세싱이 비활성화되었거나 플러그인이 없는 경우 직접 복사
        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            if (channel < numInputChannels)
                std::memcpy(outputChannelData[channel], inputChannelData[channel], sizeof(float) * numSamples);
            else
                std::memset(outputChannelData[channel], 0, sizeof(float) * numSamples);
        }
        return; // 여기서 함수를 종료하여 아래 코드가 실행되지 않도록 함
    }
    
    // 필요한 경우 임시 버퍼 크기 조정
    if (tempBuffer.getNumSamples() < numSamples)
        tempBuffer.setSize(numOutputChannels, numSamples, false, true, true);
    
    // 입력 데이터를 버퍼에 복사
    for (int channel = 0; channel < numInputChannels && channel < tempBuffer.getNumChannels(); ++channel)
    {
        // 게인 적용하여 복사
        const float* input = inputChannelData[channel];
        float* output = tempBuffer.getWritePointer(channel);
        
        // 게인 적용
        for (int i = 0; i < numSamples; ++i)
        {
            output[i] = input[i] * inputGain;
        }
    }
    
    // 추가 채널 초기화
    for (int channel = numInputChannels; channel < tempBuffer.getNumChannels(); ++channel)
    {
        std::memset(tempBuffer.getWritePointer(channel), 0, sizeof(float) * numSamples);
    }
    
    // MIDI 버퍼 (비어있음)
    juce::MidiBuffer midiBuffer;
    
    // 플러그인으로 버퍼 처리
    juce::AudioBuffer<float> buffer(tempBuffer.getArrayOfWritePointers(), 
                                   tempBuffer.getNumChannels(), 
                                   numSamples);
    
    plugin->processBlock(buffer, midiBuffer);
    
    // 처리된 데이터를 출력으로 복사
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        if (channel < tempBuffer.getNumChannels())
            std::memcpy(outputChannelData[channel], 
                       tempBuffer.getReadPointer(channel), 
                       sizeof(float) * numSamples);
        else
            std::memset(outputChannelData[channel], 0, sizeof(float) * numSamples);
    }
}

juce::Component* AmpliTubeProcessor::getEditorComponent()
{
    return editor.get();
}

void AmpliTubeProcessor::setProcessingEnabled(bool shouldBeEnabled)
{
    processingEnabled = shouldBeEnabled;
}

void AmpliTubeProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    if (plugin != nullptr)
    {
        plugin->prepareToPlay(sampleRate, samplesPerBlock);
        
        // 임시 버퍼 크기 조정
        tempBuffer.setSize(2, samplesPerBlock);
    }
}

juce::AudioPluginInstance* AmpliTubeProcessor::findAndLoadPlugin()
{
    DBG("AmpliTubeProcessor: Attempting to load plugin...");
    
    // VST3 플러그인 형식 찾기
    juce::AudioPluginFormat* format = nullptr;
    for (int i = 0; i < formatManager->getNumFormats(); ++i)
    {
        auto* f = formatManager->getFormat(i);
        if (f->getName() == "VST3")
        {
            format = f;
            break;
        }
    }
    
    if (format == nullptr)
    {
        DBG("AmpliTubeProcessor: Cannot find VST3 plugin format!");
        return nullptr;
    }
    
    // 플러그인 검색 경로 (기본 경로와 사용자 지정 경로)
    juce::FileSearchPath searchPath;
    
    // Windows 기본 VST3 경로
    #if JUCE_WINDOWS
    searchPath.add(juce::File::getSpecialLocation(juce::File::globalApplicationsDirectory)
                  .getChildFile("Common Files")
                  .getChildFile("VST3"));
    #elif JUCE_MAC
    searchPath.add(juce::File::getSpecialLocation(juce::File::globalApplicationsDirectory)
                  .getChildFile("Audio")
                  .getChildFile("Plug-Ins")
                  .getChildFile("VST3"));
    #endif
    
    // 직접 특정 플러그인 경로 지정하는 방식으로 변경
    juce::File pluginFile;
    
    #if JUCE_WINDOWS
    // Windows의 일반적인 AmpliTube 경로
    pluginFile = juce::File("C:/Program Files/Common Files/VST3/AmpliTube 5.vst3/Contents/x86_64-win/AmpliTube 5.vst3");
    #elif JUCE_MAC
    // Mac의 일반적인 AmpliTube 경로 
    pluginFile = juce::File("/Library/Audio/Plug-Ins/VST3/AmpliTube 5.vst3");
    #endif
    
    if (!pluginFile.exists())
    {
        DBG("AmpliTubeProcessor: Cannot find AmpliTube plugin file!");
        return nullptr;
    }
    
    DBG("AmpliTubeProcessor: Found plugin at " + pluginFile.getFullPathName());
    
    // 플러그인 설명 배열 가져오기
    juce::OwnedArray<juce::PluginDescription> descriptions;
    juce::KnownPluginList tempList;
    
    format->findAllTypesForFile(descriptions, pluginFile.getFullPathName());
    
    if (descriptions.isEmpty())
    {
        DBG("AmpliTubeProcessor: No plugin descriptions found!");
        return nullptr;
    }
    
    DBG("AmpliTubeProcessor: Found " + juce::String(descriptions.size()) + " plugin descriptions");
    
    // 첫 번째 설명 사용
    juce::String errorMessage;
    auto pluginInstance = format->createInstanceFromDescription(*descriptions[0], currentSampleRate, currentBlockSize, errorMessage);
    
    if (pluginInstance == nullptr)
    {
        DBG("AmpliTubeProcessor: Failed to create plugin instance: " + errorMessage);
        return nullptr;
    }
    
    return pluginInstance.release(); // std::unique_ptr에서 원시 포인터를 반환
} 