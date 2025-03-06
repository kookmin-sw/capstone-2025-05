#pragma once

// #include "../Assets/DemoUtilities.h"
#include <JuceHeader.h>

class DemoThumbnailComp final : public Component,
                                public ChangeListener,
                                public FileDragAndDropTarget,
                                public ChangeBroadcaster,
                                private ScrollBar::Listener,
                                private Timer
{
public:
    DemoThumbnailComp (AudioFormatManager& formatManager,
                       AudioTransportSource& source,
                       Slider& slider)
        : transportSource (source),
          zoomSlider (slider),
          thumbnail (512, formatManager, thumbnailCache)
    {
        thumbnail.addChangeListener (this);

        addAndMakeVisible (scrollbar);
        scrollbar.setRangeLimits (visibleRange);
        scrollbar.setAutoHide (false);
        scrollbar.addListener (this);

        currentPositionMarker.setFill (Colours::white.withAlpha (0.85f));
        addAndMakeVisible (currentPositionMarker);
    }

    ~DemoThumbnailComp() override
    {
        scrollbar.removeListener (this);
        thumbnail.removeChangeListener (this);
    }

    void setURL (const URL& url)
    {
        if (auto inputSource = std::make_unique<URLInputSource>(url))
        {
            thumbnail.setSource (inputSource.release());

            Range<double> newRange (0.0, thumbnail.getTotalLength());
            scrollbar.setRangeLimits (newRange);
            setRange (newRange);

            startTimerHz (60);
        }
    }

    URL getLastDroppedFile() const noexcept { return lastFileDropped; }

    void setZoomFactor (double amount)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            auto newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            auto timeAtCentre = xToTime ((float) getWidth() / 2.0f);

            setRange ({ timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5 });
        }
    }

    void setRange (Range<double> newRange)
    {
        visibleRange = newRange;
        scrollbar.setCurrentRange (visibleRange);
        updateCursorPosition();
        repaint();
    }

    void setFollowsTransport (bool shouldFollow)
    {
        isFollowingTransport = shouldFollow;
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
        g.setColour (Colours::lightblue);

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto thumbArea = getLocalBounds();

            thumbArea.removeFromBottom (scrollbar.getHeight() + 4);
            thumbnail.drawChannels (g, thumbArea.reduced (2),
                                    visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
        }
        else
        {
            g.setFont (14.0f);
            g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
        }
    }

    void resized() override
    {
        scrollbar.setBounds (getLocalBounds().removeFromBottom (14).reduced (2));
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        // this method is called by the thumbnail when it has changed, so we should repaint it..
        repaint();
    }

    bool isInterestedInFileDrag (const StringArray& /*files*/) override
    {
        return true;
    }

    void filesDropped (const StringArray& files, int /*x*/, int /*y*/) override
    {
        lastFileDropped = URL (File (files[0]));
        sendChangeMessage();
    }

    void mouseDown (const MouseEvent& e) override
    {
        mouseDrag (e);
    }

    void mouseDrag (const MouseEvent& e) override
    {
        if (canMoveTransport())
            transportSource.setPosition (jmax (0.0, xToTime ((float) e.x)));
    }

    void mouseUp (const MouseEvent&) override
    {
        transportSource.start();
    }

    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel) override
    {
        if (thumbnail.getTotalLength() > 0.0)
        {
            auto newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
            newStart = jlimit (0.0, jmax (0.0, thumbnail.getTotalLength() - (visibleRange.getLength())), newStart);

            if (canMoveTransport())
                setRange ({ newStart, newStart + visibleRange.getLength() });

            if (! approximatelyEqual (wheel.deltaY, 0.0f))
                zoomSlider.setValue (zoomSlider.getValue() - wheel.deltaY);

            repaint();
        }
    }

private:
    AudioTransportSource& transportSource;
    Slider& zoomSlider;
    ScrollBar scrollbar  { false };

    AudioThumbnailCache thumbnailCache  { 5 };
    AudioThumbnail thumbnail;
    Range<double> visibleRange;
    bool isFollowingTransport = false;
    URL lastFileDropped;

    DrawableRectangle currentPositionMarker;

    float timeToX (const double time) const
    {
        if (visibleRange.getLength() <= 0)
            return 0;

        return (float) getWidth() * (float) ((time - visibleRange.getStart()) / visibleRange.getLength());
    }

    double xToTime (const float x) const
    {
        return (x / (float) getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }

    bool canMoveTransport() const noexcept
    {
        return ! (isFollowingTransport && transportSource.isPlaying());
    }

    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart) override
    {
        if (scrollBarThatHasMoved == &scrollbar)
            if (! (isFollowingTransport && transportSource.isPlaying()))
                setRange (visibleRange.movedToStartAt (newRangeStart));
    }

    void timerCallback() override
    {
        if (canMoveTransport())
            updateCursorPosition();
        else
            setRange (visibleRange.movedToStartAt (transportSource.getCurrentPosition() - (visibleRange.getLength() / 2.0)));
    }

    void updateCursorPosition()
    {
        currentPositionMarker.setVisible (transportSource.isPlaying() || isMouseButtonDown());

        currentPositionMarker.setRectangle (Rectangle<float> (timeToX (transportSource.getCurrentPosition()) - 0.75f, 0,
                                                              1.5f, (float) (getHeight() - scrollbar.getHeight())));
    }
};

//==============================================================================
class AudioPlaybackDemo final : public Component,
                                private ChangeListener
{
public:
    AudioPlaybackDemo()
    {
        // 기존 코드 유지...
        
        // fileTreeComp 및 관련 코드는 제거하고
        // 대신 파일 열기 버튼 추가
        addAndMakeVisible(openButton);
        openButton.onClick = [this] { openButtonClicked(); };
        
        formatManager.registerBasicFormats();

        thread.startThread(Thread::Priority::normal);

       #ifndef JUCE_DEMO_RUNNER
        audioDeviceManager.initialise(0, 2, nullptr, true, {}, nullptr);
       #endif

        audioDeviceManager.addAudioCallback(&audioSourcePlayer);
        audioSourcePlayer.setSource(&transportSource);

        // 썸네일 컴포넌트 초기화
        thumbnail = std::make_unique<DemoThumbnailComp>(formatManager, transportSource, zoomSlider);
        addAndMakeVisible(thumbnail.get());
        thumbnail->addChangeListener(this);
        
        addAndMakeVisible(zoomLabel);
        addAndMakeVisible(zoomSlider);
        zoomSlider.setRange(0, 1, 0);
        zoomSlider.onValueChange = [this] { thumbnail->setZoomFactor(zoomSlider.getValue()); };
        zoomSlider.setSkewFactor(2);
        
        addAndMakeVisible(followTransportButton);
        followTransportButton.onClick = [this] { updateFollowTransportState(); };
        
        addAndMakeVisible(startStopButton);
        startStopButton.onClick = [this] { startOrStop(); };

        setOpaque(true);
        setSize(500, 500);
    }

    ~AudioPlaybackDemo() override
    {
        transportSource.setSource(nullptr);
        audioSourcePlayer.setSource(nullptr);
        audioDeviceManager.removeAudioCallback(&audioSourcePlayer);
        thumbnail->removeChangeListener(this);
    }

    void paint (Graphics& g) override
    {
        // g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground));
        g.fillAll (Colours::darkgrey);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (4);

        // 파일 열기 버튼을 상단에 배치
        openButton.setBounds(r.removeFromTop(30));
        r.removeFromTop(6);

        auto controls = r.removeFromBottom (90);

        auto controlRightBounds = controls.removeFromRight (controls.getWidth() / 3);

        // explanation.setBounds (controlRightBounds);

        auto zoom = controls.removeFromTop (25);
        zoomLabel .setBounds (zoom.removeFromLeft (50));
        zoomSlider.setBounds (zoom);

        followTransportButton.setBounds (controls.removeFromTop (25));
        startStopButton      .setBounds (controls);

        r.removeFromBottom (6);

        thumbnail->setBounds (r.removeFromBottom (140));
        r.removeFromBottom (6);
    }

private:
    void openButtonClicked()
    {
        chooser = std::make_unique<FileChooser>("Select an audio file...", 
                                              File::getSpecialLocation(File::userHomeDirectory),
                                              formatManager.getWildcardForAllFormats());
        
        auto chooserFlags = FileBrowserComponent::openMode | 
                           FileBrowserComponent::canSelectFiles;
                           
        chooser->launchAsync(chooserFlags, [this](const FileChooser& fc)
        {
            auto file = fc.getResult();
            
            if (file != File{})
            {
                URL fileURL(file);
                if (loadURLIntoTransport(fileURL))
                {
                    currentAudioFile = fileURL;
                    zoomSlider.setValue(0, dontSendNotification);
                    thumbnail->setURL(currentAudioFile);
                }
            }
        });
    }
    
    // AudioPlaybackDemo 클래스의 private 멤버에 추가
    TextButton openButton { "Open Audio File" };
    std::unique_ptr<FileChooser> chooser;
    
    // 나머지 기존 변수들...
    #ifndef JUCE_DEMO_RUNNER
    AudioDeviceManager audioDeviceManager;
    #else
    AudioDeviceManager& audioDeviceManager { getSharedAudioDeviceManager(0, 2) };
    #endif

    AudioFormatManager formatManager;
    TimeSliceThread thread { "audio file preview" };

    URL currentAudioFile;
    AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;
    std::unique_ptr<AudioFormatReaderSource> currentAudioFileSource;

    std::unique_ptr<DemoThumbnailComp> thumbnail;
    Label zoomLabel { {}, "zoom:" };
    Slider zoomSlider { Slider::LinearHorizontal, Slider::NoTextBox };
    ToggleButton followTransportButton { "Follow Transport" };
    TextButton startStopButton { "Play/Stop" };
    
    // 나머지 멤버 함수들...
    
public:
    bool loadURLIntoTransport (const URL& audioURL)
    {
        // unload the previous file source and delete it..
        transportSource.stop();
        transportSource.setSource (nullptr);
        currentAudioFileSource.reset();

        const auto source = std::make_unique<URLInputSource> (audioURL);

        if (source == nullptr)
            return false;

        auto stream = rawToUniquePtr (source->createInputStream());

        if (stream == nullptr)
            return false;

        auto reader = rawToUniquePtr (formatManager.createReaderFor (std::move (stream)));

        if (reader == nullptr)
            return false;

        currentAudioFileSource = std::make_unique<AudioFormatReaderSource> (reader.release(), true);

        // ..and plug it into our transport source
        transportSource.setSource (currentAudioFileSource.get(),
                                   32768,                   // tells it to buffer this many samples ahead
                                   &thread,                 // this is the background thread to use for reading-ahead
                                   currentAudioFileSource->getAudioFormatReader()->sampleRate);     // allows for sample rate correction

        return true;
    }

    void startOrStop()
    {
        if (transportSource.isPlaying())
        {
            DBG("Stopping playback");
            transportSource.stop();
        }
        else
        {
            DBG("Starting playback");
            transportSource.setPosition (0);
            transportSource.start();
        }
    }

    void updateFollowTransportState()
    {
        thumbnail->setFollowsTransport (followTransportButton.getToggleState());
    }

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (source == thumbnail.get())
            showAudioResource (URL (thumbnail->getLastDroppedFile()));
    }

    // showAudioResource 함수 수정
    void showAudioResource(URL resource)
    {
        if (!loadURLIntoTransport(resource))
        {
            // Failed to load the audio file!
            return;
        }

        currentAudioFile = std::move(resource);
        zoomSlider.setValue(0, dontSendNotification);
        thumbnail->setURL(currentAudioFile);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlaybackDemo)
};
