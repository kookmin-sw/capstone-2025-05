#include "AmpliTubeProcessor.h"

AmpliTubeProcessor::AmpliTubeProcessor()
{
    DBG("Starting AmpliTubeProcessor initialization...");

    formatManager.addDefaultFormats();
    DBG("Registered " + juce::String(formatManager.getNumFormats()) + " plugin formats");

    juce::StringArray possiblePaths;
    possiblePaths.add("C:/Program Files/Common Files/VST3/AmpliTube 5.vst3/Contents/x86_64-win/AmpliTube 5.vst3");
    // 실제 경로로 변경 필요

    juce::File pluginFile;
    bool foundValidPath = false;

    for (const auto& path : possiblePaths)
    {
        juce::File testFile(path);
        if (testFile.exists())
        {
            pluginFile = testFile;
            foundValidPath = true;
            DBG("Found AmpliTube 5 at: " + pluginFile.getFullPathName());
            break;
        }
    }

    if (!foundValidPath)
    {
        DBG("AmpliTube 5 not found.");
        return;
    }

    for (int i = 0; i < formatManager.getNumFormats(); ++i)
    {
        juce::AudioPluginFormat* format = formatManager.getFormat(i);
        DBG("Trying plugin format: " + format->getName());

        try
        {
            juce::OwnedArray<juce::PluginDescription> descriptions;
            format->findAllTypesForFile(descriptions, pluginFile.getFullPathName());

            if (!descriptions.isEmpty())
            {
                auto* desc = descriptions[0];
                juce::String errorMsg;
                plugin = formatManager.createPluginInstance(*desc, 44100.0, 512, errorMsg);
                if (plugin != nullptr)
                {
                    DBG("Successfully loaded AmpliTube 5: " + plugin->getName());
                    useVST = true;
                    break;
                }
                else
                {
                    DBG("Failed to load AmpliTube 5: " + errorMsg);
                }
            }
        }
        catch (const std::exception& e)
        {
            DBG("Exception in plugin loading: " + juce::String(e.what()));
        }
    }

    if (!useVST)
        DBG("AmpliTube 5 initialization failed.");
}

AmpliTubeProcessor::~AmpliTubeProcessor()
{
    releaseResources();
}

void AmpliTubeProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (plugin != nullptr)
    {
        plugin->prepareToPlay(sampleRate, samplesPerBlock);
        DBG("AmpliTubeProcessor prepared: SampleRate=" + juce::String(sampleRate) +
            ", BlockSize=" + juce::String(samplesPerBlock));
    }
}

void AmpliTubeProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!useVST || plugin == nullptr || buffer.getNumChannels() < 1)
    {
        buffer.clear();
        return;
    }

    plugin->processBlock(buffer, midiMessages);
    DBG("AmpliTube processed block: Magnitude=" + juce::String(buffer.getMagnitude(0, buffer.getNumSamples())));
}

void AmpliTubeProcessor::releaseResources()
{
    if (plugin != nullptr)
    {
        plugin->releaseResources();
        plugin = nullptr;
    }
    DBG("AmpliTubeProcessor resources released");
}

juce::AudioProcessorEditor* AmpliTubeProcessor::createEditor()
{
    try {
        // plugin이 nullptr인지 더 명확하게 검사하고 디버깅 메시지 추가
        if (plugin == nullptr)
        {
            DBG("Cannot create editor: plugin is nullptr");
            goto fallback;
        }

        // if (!useVST)
        // {
        //     DBG("Cannot create editor: useVST is false");
        //     goto fallback;
        // }

        // hasEditor 호출 전에 try-catch로 감싸서 안전하게 처리
        try {
            bool hasEditorResult = plugin->hasEditor();
            DBG("plugin->hasEditor() returned: " + juce::String(hasEditorResult ? "true" : "false"));
            
            if (hasEditorResult)
            {
                auto* editor = plugin->createEditorIfNeeded();
                if (editor != nullptr)
                {
                    DBG("AmpliTube 5 editor created successfully");
                    return editor;
                }
                else
                {
                    DBG("AmpliTube 5 createEditorIfNeeded returned nullptr");
                }
            }
            else
            {
                DBG("AmpliTube 5 plugin doesn't have an editor");
            }
        }
        catch (const std::exception& e)
        {
            DBG("Exception calling plugin->hasEditor(): " + juce::String(e.what()));
        }
        catch (...)
        {
            DBG("Unknown exception calling plugin->hasEditor()");
        }
    }
    catch (const std::exception& e)
    {
        DBG("Exception creating AmpliTube 5 editor: " + juce::String(e.what()));
    }
    catch (...)
    {
        DBG("Unknown exception creating AmpliTube 5 editor");
    }

fallback:
    // 에디터 생성에 실패했을 때 대체 에디터 반환
    DBG("Falling back to GenericAudioProcessorEditor");
    return new juce::GenericAudioProcessorEditor(*this);
}