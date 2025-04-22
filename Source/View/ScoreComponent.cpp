#include "ScoreComponent.h"

ScoreComponent::ScoreComponent(TabPlayer& player)
    : tabPlayer(player)
{
    viewport.setViewedComponent(&scoreContent, false);
    addAndMakeVisible(viewport);
    setWantsKeyboardFocus(true);
    startTimerHz(60); // 60Hz로 부드러운 업데이트
    DBG("ScoreComponent initialized");
}

ScoreComponent::~ScoreComponent()
{
    stopTimer();
}

void ScoreComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);

    if (!tabPlayer.getTabFile())
    {
        g.setColour(juce::Colours::black);
        g.drawText("No tab file loaded", 20, 20, 200, 20, juce::Justification::left);
        return;
    }

    const auto& tabFile = *tabPlayer.getTabFile();
    g.setColour(juce::Colours::black);
    g.setFont(16.0f);

    float currentY = yOffset;

    for (size_t trackIdx = 0; trackIdx < tabFile.tracks.size(); ++trackIdx)
    {
        const auto& track = tabFile.tracks[trackIdx];
        g.drawText(track.name, xOffset, currentY, 200, 20, juce::Justification::left);
        currentY += 30.0f;

        // 기타 현 그리기
        float trackWidth = xOffset;
        for (int string = 1; string <= 6; ++string)
        {
            float y = currentY + (string - 1) * stringSpacing;
            g.drawLine(xOffset, y, xOffset + 1000, y, 1.0f); // 임시 고정 폭
        }

        // 마디와 비트 렌더링
        float x = xOffset;
        for (size_t measureIdx = 0; measureIdx < track.measures.size(); ++measureIdx)
        {
            const auto& measure = track.measures[measureIdx];
            for (size_t beatIdx = 0; beatIdx < measure.beats.size(); ++beatIdx)
            {
                const auto& beat = measure.beats[beatIdx];
                for (const auto& voice : beat.voices)
                {
                    if (voice.empty) continue;
                    for (const auto& note : voice.notes)
                    {
                        float y = currentY + (note.string - 1) * stringSpacing;
                        g.drawText(juce::String(note.value), x, y - 10, 20, 20, juce::Justification::centred);
                    }
                }
                x += noteSpacing;
            }
            trackWidth = x;
            g.drawLine(x, currentY - 10, x, currentY + 5 * stringSpacing + 10, 2.0f);
        }

        // 커서 렌더링 - 인덱스 범위 유효성 검사 추가
        int currentTrack = tabPlayer.getCurrentTrack();
        if (tabPlayer.isPlaying() && 
            trackIdx == static_cast<size_t>(currentTrack) && 
            currentTrack >= 0 && 
            currentTrack < static_cast<int>(tabFile.tracks.size()))
        {
            int currentMeasure = tabPlayer.getCurrentMeasure();
            int currentBeat = tabPlayer.getCurrentBeat();
            
            // 마디와 비트 인덱스가 유효한 범위인지 확인
            if (currentMeasure >= 0 && 
                currentMeasure < static_cast<int>(track.measures.size()) && 
                currentBeat >= 0)
            {
                float cursorX = xOffset;

                // 현재 마디와 비트까지의 x좌표 계산
                int beatSum = 0;
                for (int m = 0; m < currentMeasure && m < static_cast<int>(track.measures.size()); ++m)
                    beatSum += track.measures[m].beats.size();
                
                // 현재 비트가 마디의 비트 수를 초과하지 않도록 함
                int safeCurrentBeat = juce::jmin(currentBeat, static_cast<int>(track.measures[currentMeasure].beats.size()) - 1);
                beatSum += safeCurrentBeat;

                cursorX += beatSum * noteSpacing;

                // 커서가 화면 내에 있도록 Viewport 조정
                auto viewArea = viewport.getViewArea();
                if (cursorX < viewArea.getX() || cursorX > viewArea.getX() + viewArea.getWidth())
                {
                    viewport.setViewPosition(cursorX - viewArea.getWidth() / 4, viewArea.getY());
                }

                g.setColour(juce::Colours::red);
                g.drawLine(cursorX, currentY - 30, cursorX, currentY + 5 * stringSpacing + 10, 2.0f);

                DBG("Rendering cursor at Track: " + juce::String(trackIdx) +
                    ", Measure: " + juce::String(currentMeasure) +
                    ", Beat: " + juce::String(safeCurrentBeat) +
                    ", cursorX: " + juce::String(cursorX));
            }
        }

        currentY += 6 * stringSpacing + 50.0f;
    }
}

void ScoreComponent::resized()
{
    viewport.setBounds(getLocalBounds());

    if (!tabPlayer.getTabFile()) return;

    const auto& tabFile = *tabPlayer.getTabFile();
    float totalWidth = xOffset;
    float totalHeight = yOffset;

    for (const auto& track : tabFile.tracks)
    {
        totalHeight += 30.0f;
        float trackWidth = xOffset;
        for (const auto& measure : track.measures)
            trackWidth += measure.beats.size() * noteSpacing;
        totalWidth = juce::jmax(totalWidth, trackWidth);
        totalHeight += 6 * stringSpacing + 50.0f;
    }

    scoreContent.setBounds(0, 0, totalWidth, totalHeight);
}

void ScoreComponent::mouseDown(const juce::MouseEvent& event)
{
    if (!tabPlayer.getTabFile()) return;

    const auto& tabFile = *tabPlayer.getTabFile();
    
    // 현재 트랙 인덱스가 유효한지 확인
    int currentTrack = tabPlayer.getCurrentTrack();
    if (currentTrack < 0 || currentTrack >= static_cast<int>(tabFile.tracks.size()))
    {
        DBG("Invalid track index: " + juce::String(currentTrack));
        return;
    }
    
    float clickX = event.getMouseDownX() + viewport.getViewPositionX();
    int beatIndex = static_cast<int>((clickX - xOffset) / noteSpacing);
    int measureIndex = 0, beatCount = 0;

    const auto& track = tabFile.tracks[currentTrack];
    for (size_t i = 0; i < track.measures.size(); ++i)
    {
        const auto& measure = track.measures[i];
        if (beatCount + measure.beats.size() > beatIndex)
        {
            int targetBeat = beatIndex - beatCount;
            // 비트 인덱스가 유효한지 확인
            if (targetBeat >= 0 && targetBeat < static_cast<int>(measure.beats.size()))
            {
                tabPlayer.setPlaybackPosition(currentTrack, static_cast<int>(i), targetBeat);
                DBG("Mouse click set position to Track: " + juce::String(currentTrack) +
                    ", Measure: " + juce::String(i) +
                    ", Beat: " + juce::String(targetBeat));
                repaint();
            }
            break;
        }
        beatCount += measure.beats.size();
        measureIndex++;
    }
}

bool ScoreComponent::keyPressed(const juce::KeyPress& key)
{
    if (!tabPlayer.getTabFile()) return false;

    int currentMeasure = tabPlayer.getCurrentMeasure();
    int currentTrack = tabPlayer.getCurrentTrack();
    
    // 현재 트랙 인덱스가 유효한지 확인
    if (currentTrack < 0 || currentTrack >= static_cast<int>(tabPlayer.getTabFile()->tracks.size()))
    {
        DBG("Invalid track index in keyPressed: " + juce::String(currentTrack));
        return false;
    }

    if (key == juce::KeyPress::leftKey)
    {
        if (currentMeasure > 0)
        {
            tabPlayer.setPlaybackPosition(currentTrack, currentMeasure - 1, 0);
            DBG("Key left: Moved to Measure " + juce::String(currentMeasure - 1));
            repaint();
            return true;
        }
    }
    else if (key == juce::KeyPress::rightKey)
    {
        // 마디 인덱스 범위 체크 추가
        if (currentMeasure < static_cast<int>(tabPlayer.getTabFile()->tracks[currentTrack].measures.size()) - 1)
        {
            tabPlayer.setPlaybackPosition(currentTrack, currentMeasure + 1, 0);
            DBG("Key right: Moved to Measure " + juce::String(currentMeasure + 1));
            repaint();
            return true;
        }
    }

    return false;
}

void ScoreComponent::timerCallback()
{
    if (tabPlayer.isPlaying())
    {
        static int lastMeasure = -1;
        static int lastBeat = -1;
        int currentMeasure = tabPlayer.getCurrentMeasure();
        int currentBeat = tabPlayer.getCurrentBeat();

        if (currentMeasure != lastMeasure || currentBeat != lastBeat)
        {
            repaint();
            lastMeasure = currentMeasure;
            lastBeat = currentBeat;
            DBG("Timer update: Measure " + juce::String(currentMeasure) +
                ", Beat " + juce::String(currentBeat));
        }
    }
}

void ScoreComponent::startPlayback()
{
    repaint();  // 재생 시작을 화면에 반영
    DBG("ScoreComponent: startPlayback called");
}

void ScoreComponent::stopPlayback()
{
    repaint();  // 재생 중지를 화면에 반영
    DBG("ScoreComponent: stopPlayback called");
}

// 악보 업데이트 메서드 구현
void ScoreComponent::updateScore()
{
    DBG("ScoreComponent: Updating score display");
    
    // 악보 데이터가 유효한지 확인
    if (!tabPlayer.getTabFile())
    {
        DBG("ScoreComponent: No tab file loaded");
        return;
    }
    
    // 뷰포트 위치 초기화
    viewport.setViewPosition(0, 0);
    
    // 컴포넌트 크기 재조정 (새 악보 크기에 맞게)
    resized();
    
    // 화면 갱신
    repaint();
    
    DBG("ScoreComponent: Score updated successfully");
}