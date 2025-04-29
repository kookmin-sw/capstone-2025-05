#include "ScoreComponent.h"
#include "LookAndFeel/MapleTheme.h"

ScoreComponent::ScoreComponent(TabPlayer& player)
    : tabPlayer(player)
{
    // 뷰포트 설정
    viewport.setViewedComponent(&scoreContent, false);
    
    // 배경색은 scoreContent에 적용
    scoreContent.setOpaque(true);
    
    viewport.setScrollBarsShown(true, false);  // 수평 스크롤바만 표시
    
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
    // 배경 채우기 - MapleTheme 컬러 사용
    g.fillAll(MapleTheme::getCardColour());
    
    // 배경에 테두리 추가
    g.setColour(MapleTheme::getAccentColour().withAlpha(0.3f));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 8.0f, 1.0f);

    if (!tabPlayer.getTabFile())
    {
        // 악보가 없을 때 메시지 표시
        g.setColour(MapleTheme::getSubTextColour());
        g.setFont(juce::Font(18.0f).italicised());
        g.drawText("No tab file loaded. Please select a song to view the sheet music.",
                  getLocalBounds().reduced(30), juce::Justification::centred, true);
    }
}

void ScoreComponent::ScoreContentComponent::paint(juce::Graphics& g)
{
    // 배경 채우기
    g.fillAll(MapleTheme::getBackgroundColour().brighter(0.05f));
    
    if (!tabPlayer.getTabFile()) return;
    
    const auto& tabFile = *tabPlayer.getTabFile();
    
    // 곡 정보 표시
    g.setColour(MapleTheme::getHighlightColour());
    g.setFont(juce::Font(24.0f).boldened());
    g.drawText(tabFile.title, owner.xOffset, 10, getWidth() - 40, 30, juce::Justification::left);
    
    g.setColour(MapleTheme::getTextColour());
    g.setFont(juce::Font(16.0f));
    g.drawText(tabFile.artist, owner.xOffset + 10, 35, getWidth() - 60, 20, juce::Justification::left);

    float currentY = owner.yOffset;

    // 모든 트랙 렌더링
    for (size_t trackIdx = 0; trackIdx < tabFile.tracks.size(); ++trackIdx)
    {
        const auto& track = tabFile.tracks[trackIdx];
        
        // 트랙 헤더 배경
        g.setColour(MapleTheme::getCardColour());
        g.fillRoundedRectangle(owner.xOffset - 10, currentY - 15, 220, 30, 5.0f);
        
        // 트랙 테두리
        g.setColour(MapleTheme::getAccentColour().withAlpha(0.3f));
        g.drawRoundedRectangle(owner.xOffset - 10, currentY - 15, 220, 30, 5.0f, 1.0f);
        
        // 트랙 제목에 스타일 적용
        g.setColour(MapleTheme::getTextColour());
        g.setFont(juce::Font(18.0f).boldened());
        g.drawText(track.name, owner.xOffset, currentY, 200, 20, juce::Justification::left);
        currentY += 30.0f;

        // 기타 현 그리기 - 현 별로 다른 색상 사용하여 가독성 개선
        float trackWidth = owner.xOffset;
        for (int string = 1; string <= 6; ++string)
        {
            float y = currentY + (string - 1) * owner.stringSpacing;
            // 현 색상 조정 - 저음 현은 더 두껍게
            float thickness = 1.0f + (6 - string) * 0.2f;
            juce::Colour stringColour = juce::Colour(
                juce::uint8(20 + (string * 30)), 
                juce::uint8(20 + (string * 20)), 
                juce::uint8(50 + (string * 30))
            ).withAlpha(0.7f);
            
            g.setColour(stringColour);
            g.drawLine(owner.xOffset, y, owner.xOffset + 2000, y, thickness);
        }

        // 마디를 구분하는 사각형 표시 추가
        float x = owner.xOffset;
        for (size_t measureIdx = 0; measureIdx < track.measures.size(); ++measureIdx)
        {
            const auto& measure = track.measures[measureIdx];
            
            // 마디 번호 표시 (작은 숫자로)
            g.setColour(MapleTheme::getSubTextColour());
            g.setFont(12.0f);
            g.drawText(juce::String(measureIdx + 1), 
                      x, currentY - 25, 
                      30, 20, 
                      juce::Justification::left);
            
            // 마디 시작 세로선
            g.setColour(MapleTheme::getSubTextColour());
            g.drawLine(x, currentY - 10, x, currentY + 5 * owner.stringSpacing + 10, 1.5f);
            
            // 마디 배경 - 짝수/홀수 마디 구분
            juce::Rectangle<float> measureRect(
                x, currentY - 10,
                measure.beats.size() * owner.noteSpacing, 5 * owner.stringSpacing + 20
            );
            
            if (measureIdx % 2 == 0)
                g.setColour(MapleTheme::getCardColour().withAlpha(0.5f));
            else
                g.setColour(MapleTheme::getSidebarColour().withAlpha(0.1f));
                
            g.fillRect(measureRect);
            
            // 비트 렌더링
            for (size_t beatIdx = 0; beatIdx < measure.beats.size(); ++beatIdx)
            {
                const auto& beat = measure.beats[beatIdx];
                for (const auto& voice : beat.voices)
                {
                    if (voice.empty) continue;
                    for (const auto& note : voice.notes)
                    {
                        float y = currentY + (note.string - 1) * owner.stringSpacing;
                        
                        // 노트 표시 개선 - 동그란 배경 추가
                        g.setColour(MapleTheme::getAccentColour().withAlpha(0.2f));
                        g.fillEllipse(x - 10, y - 10, 20, 20);
                        
                        g.setColour(MapleTheme::getTextColour());
                        g.setFont(juce::Font(14.0f).boldened());
                        g.drawText(juce::String(note.value), 
                                  x - 10, y - 10, 
                                  20, 20, 
                                  juce::Justification::centred);
                    }
                }
                x += owner.noteSpacing;
            }
            trackWidth = x;
            
            // 마디 끝 세로선
            g.setColour(MapleTheme::getSubTextColour());
            g.drawLine(x, currentY - 10, x, currentY + 5 * owner.stringSpacing + 10, 1.5f);
        }

        // 현재 재생 위치 커서 렌더링
        int currentTrack = tabPlayer.getCurrentTrack();
        if (trackIdx == static_cast<size_t>(currentTrack) && 
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
                float cursorX = owner.xOffset;

                // 현재 마디와 비트까지의 x좌표 계산
                int beatSum = 0;
                for (int m = 0; m < currentMeasure && m < static_cast<int>(track.measures.size()); ++m)
                    beatSum += track.measures[m].beats.size();
                
                // 현재 비트가 마디의 비트 수를 초과하지 않도록 함
                int safeCurrentBeat = juce::jmin(currentBeat, static_cast<int>(track.measures[currentMeasure].beats.size()) - 1);
                beatSum += safeCurrentBeat;

                cursorX += beatSum * owner.noteSpacing;

                // 커서가 화면 내에 있도록 Viewport 조정 (tabPlayer가 재생 중일 때만)
                if (tabPlayer.isPlaying())
                {
                    auto viewArea = owner.viewport.getViewArea();
                    if (cursorX < viewArea.getX() || cursorX > viewArea.getX() + viewArea.getWidth())
                    {
                        owner.viewport.setViewPosition(cursorX - viewArea.getWidth() / 4, viewArea.getY());
                    }
                }

                // 커서 렌더링 개선 - 반투명 배경과 컬러 사용
                g.setColour(MapleTheme::getHighlightColour().withAlpha(0.3f));
                juce::Rectangle<float> cursorRect(cursorX - 5, currentY - 20, 10, 5 * owner.stringSpacing + 30);
                g.fillRect(cursorRect);
                
                g.setColour(MapleTheme::getHighlightColour());
                g.drawLine(cursorX, currentY - 20, cursorX, currentY + 5 * owner.stringSpacing + 10, 2.0f);
            }
        }

        // 다음 트랙으로 이동
        currentY += 6 * owner.stringSpacing + 50.0f;
    }
}

void ScoreComponent::resized()
{
    // 뷰포트가 전체 영역을 차지하도록 설정
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

    // 콘텐츠 크기 설정
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
                scoreContent.repaint();
            }
            break;
        }
        beatCount += measure.beats.size();
    }
}

bool ScoreComponent::keyPressed(const juce::KeyPress& key)
{
    if (!tabPlayer.getTabFile()) return false;
    const auto& tabFile = *tabPlayer.getTabFile();
    
    int currentTrack = tabPlayer.getCurrentTrack();
    int currentMeasure = tabPlayer.getCurrentMeasure();
    int currentBeat = tabPlayer.getCurrentBeat();
    
    if (currentTrack < 0 || currentTrack >= static_cast<int>(tabFile.tracks.size()))
        return false;
        
    const auto& track = tabFile.tracks[currentTrack];
    
    // 왼쪽 화살표 - 이전 비트로 이동
    if (key == juce::KeyPress::leftKey)
    {
        if (currentBeat > 0)
        {
            tabPlayer.setPlaybackPosition(currentTrack, currentMeasure, currentBeat - 1);
        }
        else if (currentMeasure > 0)
        {
            // 이전 마디의 마지막 비트로 이동
            int prevMeasureBeats = track.measures[currentMeasure - 1].beats.size();
            tabPlayer.setPlaybackPosition(currentTrack, currentMeasure - 1, prevMeasureBeats - 1);
        }
        return true;
    }
    
    // 오른쪽 화살표 - 다음 비트로 이동
    else if (key == juce::KeyPress::rightKey)
    {
        if (currentMeasure < static_cast<int>(track.measures.size()) && 
            currentBeat < static_cast<int>(track.measures[currentMeasure].beats.size()) - 1)
        {
            tabPlayer.setPlaybackPosition(currentTrack, currentMeasure, currentBeat + 1);
        }
        else if (currentMeasure < static_cast<int>(track.measures.size()) - 1)
        {
            // 다음 마디의 첫 비트로 이동
            tabPlayer.setPlaybackPosition(currentTrack, currentMeasure + 1, 0);
        }
        return true;
    }
    
    // 스페이스 - 재생/일시정지 토글
    else if (key == juce::KeyPress::spaceKey)
    {
        if (tabPlayer.isPlaying())
            tabPlayer.stopPlaying();
        else
            tabPlayer.startPlaying();
        return true;
    }
    
    return false;
}

void ScoreComponent::startPlayback()
{
    scoreContent.repaint();
}

void ScoreComponent::stopPlayback()
{
    scoreContent.repaint();
}

void ScoreComponent::updateScore()
{
    // 스코어 콘텐츠 크기 업데이트
    resized();
    
    // 스코어 다시 그리기
    scoreContent.repaint();
}

void ScoreComponent::timerCallback()
{
    // 재생 위치 업데이트를 위해 악보 다시 그리기
    if (tabPlayer.isPlaying())
    {
        scoreContent.repaint();
    }
}