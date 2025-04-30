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

// 리스너에게 재생 상태 변경 알림
void ScoreComponent::notifyPlaybackStateChanged(bool isPlaying)
{
    if (playbackListener != nullptr)
    {
        playbackListener->onPlaybackStateChanged(isPlaying);
    }
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
    
    // 클릭한 위치가 scoreContent 내부에 있는지 확인
    juce::Point<int> posInContent = event.position.toInt() - viewport.getViewPosition();
    
    float currentY = yOffset;
    int trackIndex = -1;
    int measureIndex = -1;
    int beatIndex = -1;

    // 클릭한 위치에 해당하는 트랙, 마디, 비트를 찾기
    for (size_t trackIdx = 0; trackIdx < tabFile.tracks.size(); ++trackIdx)
    {
        const auto& track = tabFile.tracks[trackIdx];
        
        // 각 트랙의 세로 범위 계산
        float trackTopY = currentY;
        float trackBottomY = currentY + 6 * stringSpacing + 30.0f;
        
        // 클릭한 Y좌표가 현재 트랙 범위 내에 있는지 확인
        if (posInContent.y >= trackTopY && posInContent.y <= trackBottomY)
        {
            trackIndex = static_cast<int>(trackIdx);
            
            // 클릭한 위치에 해당하는 마디와 비트 찾기
            float x = xOffset;
            for (size_t measureIdx = 0; measureIdx < track.measures.size(); ++measureIdx)
            {
                const auto& measure = track.measures[measureIdx];
                float measureWidth = measure.beats.size() * noteSpacing;
                
                // 클릭한 X좌표가 현재 마디 범위 내에 있는지 확인
                if (posInContent.x >= x && posInContent.x < x + measureWidth)
                {
                    measureIndex = static_cast<int>(measureIdx);
                    
                    // 비트 인덱스 계산
                    beatIndex = static_cast<int>((posInContent.x - x) / noteSpacing);
                    
                    // 비트 인덱스가 유효 범위 내에 있는지 확인하고 제한
                    beatIndex = juce::jlimit(0, static_cast<int>(measure.beats.size()) - 1, beatIndex);
                    break;
                }
                
                x += measureWidth;
            }
            
            break;
        }
        
        // 다음 트랙으로 이동
        currentY = trackBottomY + 20.0f;
    }
    
    // 유효한 위치가 찾아졌으면 TabPlayer에 재생 위치 설정
    if (trackIndex >= 0 && measureIndex >= 0 && beatIndex >= 0)
    {
        DBG("Click position: Track=" + juce::String(trackIndex) + 
            ", Measure=" + juce::String(measureIndex) + 
            ", Beat=" + juce::String(beatIndex));
        
        // 재생 위치 설정
        tabPlayer.setPlaybackPosition(trackIndex, measureIndex, beatIndex);
        
        // 이미 재생 중이면 중지했다가 새 위치에서 다시 시작
        bool wasPlaying = tabPlayer.isPlaying();
        
        // 한번 정지
        if (wasPlaying)
        {
            tabPlayer.stopPlaying();
        }
        
        // 중지된 상태에서 Ctrl 또는 Command 키를 누른 상태로 클릭하면 
        // 그 위치에서 재생 시작 (토글 동작)
        if (event.mods.isCtrlDown() || event.mods.isCommandDown())
        {
            wasPlaying = !wasPlaying;
        }
        
        // 원래 재생 중이었거나 Ctrl+클릭한 경우 재생 시작
        if (wasPlaying)
        {
            tabPlayer.startPlaying();
            notifyPlaybackStateChanged(true);
        }
        else
        {
            // 그대로 정지 상태 유지
            notifyPlaybackStateChanged(false);
        }
        
        // 항상 스코어를 업데이트하고 다시 그림
        updateScore();
        
        // 커서가 보이도록 뷰포트 위치 조정
        float cursorX = xOffset;
        int beatSum = 0;
        
        const auto& track = tabFile.tracks[trackIndex];
        for (int m = 0; m < measureIndex; ++m)
            beatSum += track.measures[m].beats.size();
        
        beatSum += beatIndex;
        cursorX += beatSum * noteSpacing;
        
        // 커서 위치로 스크롤
        viewport.setViewPosition(cursorX - viewport.getWidth() / 4, viewport.getViewPosition().getY());
    }
}

bool ScoreComponent::keyPressed(const juce::KeyPress& key)
{
    // 스페이스바로 재생/정지 토글
    if (key == juce::KeyPress::spaceKey)
    {
        bool wasPlaying = tabPlayer.isPlaying();
        
        if (wasPlaying)
        {
            tabPlayer.stopPlaying();
        }
        else
        {
            tabPlayer.startPlaying();
        }
            
        // 재생 상태 변경 알림 (토글된 상태로)
        notifyPlaybackStateChanged(!wasPlaying);
        
        return true;
    }
    
    // 왼쪽/오른쪽 화살표 키로 비트 단위 이동
    else if (key == juce::KeyPress::leftKey || key == juce::KeyPress::rightKey)
    {
        if (!tabPlayer.getTabFile()) return false;
        
        int track = tabPlayer.getCurrentTrack();
        int measure = tabPlayer.getCurrentMeasure();
        int beat = tabPlayer.getCurrentBeat();
        
        const auto& tabFile = *tabPlayer.getTabFile();
        if (track < 0 || track >= static_cast<int>(tabFile.tracks.size()))
            return false;
            
        const auto& trackData = tabFile.tracks[track];
        
        // 왼쪽 화살표: 이전 비트로 이동
        if (key == juce::KeyPress::leftKey)
        {
            // 현재 마디의 첫 비트에서 왼쪽 화살표를 누르면 이전 마디의 마지막 비트로
            if (beat <= 0)
            {
                if (measure > 0)
                {
                    measure--;
                    beat = static_cast<int>(trackData.measures[measure].beats.size()) - 1;
                }
            }
            else
            {
                beat--;
            }
        }
        // 오른쪽 화살표: 다음 비트로 이동
        else
        {
            // 현재 마디의 마지막 비트에서 오른쪽 화살표를 누르면 다음 마디의 첫 비트로
            if (measure < static_cast<int>(trackData.measures.size()) && 
                beat >= static_cast<int>(trackData.measures[measure].beats.size()) - 1)
            {
                if (measure < static_cast<int>(trackData.measures.size()) - 1)
                {
                    measure++;
                    beat = 0;
                }
            }
            else
            {
                beat++;
            }
        }
        
        // 위치 설정 및 업데이트
        tabPlayer.setPlaybackPosition(track, measure, beat);
        updateScore();
        
        // 커서가 보이도록 뷰포트 조정
        float cursorX = xOffset;
        int beatSum = 0;
        
        for (int m = 0; m < measure; ++m)
            beatSum += trackData.measures[m].beats.size();
        
        beatSum += beat;
        cursorX += beatSum * noteSpacing;
        
        viewport.setViewPosition(cursorX - viewport.getWidth() / 4, viewport.getViewPosition().getY());
        
        return true;
    }
    
    // 위/아래 화살표 키로 트랙 간 이동
    else if (key == juce::KeyPress::upKey || key == juce::KeyPress::downKey)
    {
        if (!tabPlayer.getTabFile()) return false;
        
        int track = tabPlayer.getCurrentTrack();
        int measure = tabPlayer.getCurrentMeasure();
        int beat = tabPlayer.getCurrentBeat();
        
        const auto& tabFile = *tabPlayer.getTabFile();
        
        // 위 화살표: 이전 트랙으로
        if (key == juce::KeyPress::upKey && track > 0)
        {
            track--;
        }
        // 아래 화살표: 다음 트랙으로
        else if (key == juce::KeyPress::downKey && track < static_cast<int>(tabFile.tracks.size()) - 1)
        {
            track++;
        }
        else
        {
            return false;
        }
        
        // 새 트랙의 마디/비트 범위 확인
        if (track >= 0 && track < static_cast<int>(tabFile.tracks.size()))
        {
            const auto& trackData = tabFile.tracks[track];
            
            // 마디와 비트가 범위 내에 있는지 확인하고 조정
            measure = juce::jmin(measure, static_cast<int>(trackData.measures.size()) - 1);
            if (measure >= 0 && measure < static_cast<int>(trackData.measures.size()))
            {
                beat = juce::jmin(beat, static_cast<int>(trackData.measures[measure].beats.size()) - 1);
            }
            else
            {
                measure = 0;
                beat = 0;
            }
            
            // 위치 설정 및 업데이트
            tabPlayer.setPlaybackPosition(track, measure, beat);
            updateScore();
            return true;
        }
    }
    
    return false;
}

void ScoreComponent::startPlayback()
{
    // UI 상태 업데이트 뿐만 아니라 재생 상태 확인
    if (!tabPlayer.isPlaying())
    {
        tabPlayer.startPlaying();
    }
    scoreContent.repaint();
}

void ScoreComponent::stopPlayback()
{
    // UI 상태 업데이트 뿐만 아니라 정지 상태 확인
    if (tabPlayer.isPlaying())
    {
        tabPlayer.stopPlaying();
    }
    scoreContent.repaint();
}

void ScoreComponent::updateScore()
{
    scoreContent.repaint();
    resized(); // 콘텐츠 크기 업데이트
}

void ScoreComponent::timerCallback()
{
    // TabPlayer 상태 확인
    bool isPlayerPlaying = tabPlayer.isPlaying();
    static bool lastPlayingState = false;
    
    // 이전 상태와 다른 경우 리스너에게 알림 (상태 변경 감지)
    if (isPlayerPlaying != lastPlayingState)
    {
        DBG("ScoreComponent detected playback state change: " + juce::String(isPlayerPlaying ? "playing" : "stopped"));
        notifyPlaybackStateChanged(isPlayerPlaying);
        lastPlayingState = isPlayerPlaying;
    }
    
    // 재생 상태와 관계없이 악보 커서 위치 표시를 위한 리페인트
    scoreContent.repaint();
    
    // 재생 중일 때 자동 스크롤 처리
    if (isPlayerPlaying && tabPlayer.getTabFile())
    {
        // 현재 재생 위치에 해당하는 커서 위치 계산
        int currentTrack = tabPlayer.getCurrentTrack();
        int currentMeasure = tabPlayer.getCurrentMeasure();
        int currentBeat = tabPlayer.getCurrentBeat();
        
        const auto& tabFile = *tabPlayer.getTabFile();
        
        // 위치가 유효한지 확인
        if (currentTrack >= 0 && currentTrack < static_cast<int>(tabFile.tracks.size()) &&
            currentMeasure >= 0 && currentMeasure < static_cast<int>(tabFile.tracks[currentTrack].measures.size()))
        {
            float cursorX = xOffset;
            int beatSum = 0;
            
            const auto& track = tabFile.tracks[currentTrack];
            for (int m = 0; m < currentMeasure; ++m)
                beatSum += track.measures[m].beats.size();
            
            beatSum += currentBeat;
            cursorX += beatSum * noteSpacing;
            
            // 현재 뷰포트 영역
            auto viewArea = viewport.getViewArea();
            
            // 커서가 뷰포트 영역의 오른쪽 1/3 지점을 넘어가면 자동 스크롤
            float threshold = viewArea.getX() + (viewArea.getWidth() * 2 / 3);
            if (cursorX > threshold)
            {
                // 부드러운 스크롤을 위해 현재 뷰포트 위치에서 약간씩 이동
                int currentX = viewport.getViewPositionX();
                int targetX = static_cast<int>(cursorX - viewArea.getWidth() / 3);
                int newX = currentX + juce::jmin(10, targetX - currentX); // 최대 10픽셀씩 이동
                
                viewport.setViewPosition(newX, viewport.getViewPositionY());
            }
        }
    }
}