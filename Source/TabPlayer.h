class TabPlayer : public juce::AudioProcessor
{
public:
    TabPlayer()
    {
        formatManager.addDefaultFormats();
        OwnedArray<PluginDescription> pluginDescriptions;
        formatManager.createPluginDescriptionForFile("C:/Program Files/VSTPlugins/AGML2.dll", pluginDescriptions);
        if (!pluginDescriptions.isEmpty())
            plugin = formatManager.createPluginInstance(*pluginDescriptions[0], 44100.0, 512, error);
    }

    void setTabFile(const gp_parser::TabFile& file) { tabFile = file; }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        if (plugin) plugin->prepareToPlay(sampleRate, samplesPerBlock);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        midiMessages.clear();
        if (!tabFile.tracks.empty() && sampleCounter == 0)
        {
            auto& beat = tabFile.tracks[0].measures[0].beats[0];
            for (auto& voice : beat.voices)
                for (auto& note : voice.notes)
                {
                    int midiNote = stringToMidiNote(note.string, note.value);
                    midiMessages.addEvent(juce::MidiMessage::noteOn(1, midiNote, 0.8f), 0);
                }
        }
        sampleCounter += buffer.getNumSamples();
        if (plugin) plugin->processBlock(buffer, midiMessages);
    }

    // 필수 오버라이드 함수들
    const juce::String getName() const override { return "TabPlayer"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}
    void releaseResources() override { if (plugin) plugin->releaseResources(); }
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override { return plugin ? plugin->createEditor() : nullptr; }

private:
    juce::AudioPluginFormatManager formatManager;
    std::unique_ptr<juce::AudioPluginInstance> plugin;
    gp_parser::TabFile tabFile;
    int sampleCounter = 0;
    juce::String error;

    int stringToMidiNote(int string, int fret)
    {
        static const int openStrings[] = { 40, 45, 50, 55, 59, 64 }; // E2, A2, D3, G3, B3, E4
        return openStrings[string - 1] + fret;
    }
};