#include "HomePage.h"
#include "../../UI/Styles/MapleColours.h"
#include "../../UI/Styles/MapleTypography.h"
#include "../../Services/SpotifyService.h"

HomePage::HomePage()
{
    createProjectBtn = std::make_unique<MapleButton>(juce::String::fromUTF8(u8"+ 프로젝트 시작"));
    createProjectBtn->setColour(MapleButton::backgroundColourId, juce::Colours::black);
    createProjectBtn->setColour(MapleButton::textColourId, MapleColours::currentTheme.text);
    addAndMakeVisible(createProjectBtn.get());

    // 컴포넌트가 생성될 때 앨범 검색 시작
    searchAlbums("Younha");
    DBG("Started album search");

    // 페이드 애니메이션 타이머 설정
    fadeAnimator = std::make_unique<FadeTimer>(*this);
    fadeAnimator->startTimer(16);  // 항상 실행

    // Viewport와 컨테이너 생성
    albumViewport = std::make_unique<juce::Viewport>("album scroll");
    albumContainer = std::make_unique<AlbumContainer>(*this);
    
    albumViewport->setViewedComponent(albumContainer.get(), false);
    albumViewport->setScrollBarsShown(true, false);  // 가로 스크롤바만 표시
    albumViewport->setScrollOnDragEnabled(true);     // 드래그로 스크롤 가능
    albumViewport->setSingleStepSizes(20, 20);       // 스크롤 스텝 크기 설정
    addAndMakeVisible(albumViewport.get());

    // 네비게이션 버튼은 제거 (스크롤로 대체)
    prevAlbumBtn.reset();
    nextAlbumBtn.reset();

    // 스크롤 버튼 생성
    scrollLeftBtn = std::make_unique<MapleButton>(juce::String::fromUTF8(u8"<"));
    scrollRightBtn = std::make_unique<MapleButton>(juce::String::fromUTF8(u8">"));
    
    // 버튼 색상을 더 밝은 회색으로 변경
    auto buttonColour = juce::Colours::black.brighter(0.2f);
    scrollLeftBtn->setColour(MapleButton::backgroundColourId, buttonColour);
    scrollRightBtn->setColour(MapleButton::backgroundColourId, buttonColour);
    
    addAndMakeVisible(scrollLeftBtn.get());
    addAndMakeVisible(scrollRightBtn.get());
    
    scrollAnimator = std::make_unique<ScrollAnimator>(*this);
    
    scrollLeftBtn->setOnClick([this]() { 
        auto newPosition = albumViewport->getViewPositionX() - 660;  // 3개의 앨범씩 이동 (220 * 3)
        newPosition = juce::jmax(0, newPosition);
        scrollAnimator->startScrolling(newPosition);
    });
    
    scrollRightBtn->setOnClick([this]() {
        auto newPosition = albumViewport->getViewPositionX() + 660;  // 3개의 앨범씩 이동
        newPosition = juce::jmin(newPosition, 
                               albumContainer->getWidth() - albumViewport->getWidth());
        scrollAnimator->startScrolling(newPosition);
    });
}

HomePage::~HomePage()
{
}

void HomePage::paint(juce::Graphics &g)
{
    // 부모 클래스의 paint 호출 (배경 그리기)
    Page::paint(g);

    auto bounds = getLocalBounds().reduced(20);

    // 프로젝트 시작 버튼 영역
    auto projectButtonArea = bounds.removeFromTop(200);
    createProjectBtn->setFont(MapleTypography::getPretendardBold(24.0f));

    bounds.removeFromTop(40); // 프로젝트 버튼과 섹션 사이 간격

    // Recently 섹션
    auto recentlySection = bounds.removeFromTop(240);
    drawSection(g, juce::String::fromUTF8(u8"RECENTLY"), recentlySection);
    bounds.removeFromTop(40); // 섹션 간 간격

    // Recommended 섹션 (앨범 표시)
    auto recommendedSection = bounds.removeFromTop(240);
    drawRecommendedSection(g, recommendedSection);
    bounds.removeFromTop(40); // 섹션 간 간격

    // Practice Courses 섹션
    auto practiceSection = bounds;
    drawSection(g, juce::String::fromUTF8(u8"PRACTICE COURSES"), practiceSection, false);
}

void HomePage::drawSection(juce::Graphics &g, const juce::String &title,
                           juce::Rectangle<int> bounds, bool showMoreButton)
{
    // 섹션 헤더 그리기
    auto headerBounds = bounds.removeFromTop(40);
    drawSectionHeader(g, title, headerBounds, showMoreButton);

    // 섹션 컨텐츠 영역 (회색 박스들)
    auto contentBounds = bounds;

    if (title == juce::String::fromUTF8(u8"PRACTICE COURSES"))
    {
        // Practice Courses는 더 큰 박스 3개
        const int boxWidth = (contentBounds.getWidth() - 40) / 3;
        const int boxHeight = 160;

        for (int i = 0; i < 3; ++i)
        {
            auto boxBounds = contentBounds.removeFromLeft(boxWidth);
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(boxBounds.reduced(10).toFloat(), 8.0f);

            // 박스 내부에 텍스트 추가 (임시)
            g.setColour(MapleColours::currentTheme.text);
            g.setFont(MapleTypography::getPretendardMedium(16.0f));
            g.drawText(juce::String::fromUTF8(u8"연습 코스 ") + juce::String(i + 1),
                       boxBounds.reduced(20),
                       juce::Justification::centredTop);

            if (i < 2)
                contentBounds.removeFromLeft(20); // 간격
        }
    }
    else
    {
        // Recently와 Recommended는 작은 박스 6개
        const int boxWidth = (contentBounds.getWidth() - 100) / 6;
        const int boxHeight = 160;

        for (int i = 0; i < 6; ++i)
        {
            auto boxBounds = contentBounds.removeFromLeft(boxWidth);
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(boxBounds.reduced(10).toFloat(), 8.0f);

            // 박스 내부에 텍스트 추가 (임시)
            g.setColour(MapleColours::currentTheme.text);
            g.setFont(MapleTypography::getPretendardMedium(14.0f));
            g.drawText(title == juce::String::fromUTF8(u8"RECENTLY") ? juce::String::fromUTF8(u8"최근 항목 ") + juce::String(i + 1) : juce::String::fromUTF8(u8"추천 항목 ") + juce::String(i + 1),
                       boxBounds.reduced(15),
                       juce::Justification::centredTop);

            if (i < 5)
                contentBounds.removeFromLeft(20); // 간격
        }
    }
}

void HomePage::drawSectionHeader(juce::Graphics &g, const juce::String &title,
                                 juce::Rectangle<int> bounds, bool showMoreButton)
{
    // 섹션 제목
    g.setFont(MapleTypography::getMontserratBold(24.0f));
    g.setColour(MapleColours::currentTheme.text);
    g.drawText(title, bounds.removeFromLeft(bounds.getWidth() - 100),
               juce::Justification::centredLeft);

    // "모두 보기" 버튼
    if (showMoreButton)
    {
        g.setFont(MapleTypography::getPretendardMedium(14.0f));
        g.setColour(MapleColours::currentTheme.text.withAlpha(0.7f));
        g.drawText(juce::String::fromUTF8(u8"모두 보기"), bounds.removeFromRight(100),
                   juce::Justification::centredRight);
    }
}

void HomePage::drawRecommendedSection(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // 섹션 헤더만 그리기
    auto headerBounds = bounds.removeFromTop(40);
    drawSectionHeader(g, juce::String::fromUTF8(u8"RECOMMENDED"), headerBounds, true);
}

void HomePage::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    // 프로젝트 시작 버튼
    auto projectButtonArea = bounds.removeFromTop(200);
    createProjectBtn->setBounds(projectButtonArea.reduced(10).withHeight(160));

    bounds.removeFromTop(40);
    auto recentlySection = bounds.removeFromTop(240);
    bounds.removeFromTop(40);

    // Recommended 섹션의 Viewport 위치 설정
    auto recommendedSection = bounds.removeFromTop(240);
    auto viewportBounds = recommendedSection.removeFromTop(200);
    albumViewport->setBounds(viewportBounds.reduced(40, 0));  // 양쪽에 버튼 공간 확보
    
    // 스크롤 버튼 위치 설정
    const int buttonSize = 40;
    const int buttonY = viewportBounds.getY() + (viewportBounds.getHeight() - buttonSize) / 2;
    scrollLeftBtn->setBounds(viewportBounds.getX(), buttonY, buttonSize, buttonSize);
    scrollRightBtn->setBounds(viewportBounds.getRight() - buttonSize, buttonY, buttonSize, buttonSize);

    // 컨테이너 크기 설정
    const int albumSize = 200;
    const int spacing = 20;
    const int totalWidth = (albumSize + spacing) * albums.size();
    albumContainer->setBounds(0, 0, totalWidth, albumViewport->getHeight());
}

void HomePage::updateFadeAnimation()
{
    bool needsRepaint = false;
    const float fadeSpeed = 0.08f;  // 페이드 속도 더 증가
    
    for (auto& album : albums)
    {
        if (album.coverImage && album.alpha < 1.0f)
        {
            album.alpha = juce::jmin(1.0f, album.alpha + fadeSpeed);
            needsRepaint = true;
        }
    }
    
    if (needsRepaint)
        repaint();
}

void HomePage::searchAlbums(const juce::String& query)
{
    DBG("Searching albums for query: " + query);
    std::thread([this, query]() {
        auto results = SpotifyService::searchAlbums(query);
        DBG("Found " + juce::String(results.size()) + " albums");
        
        juce::MessageManager::callAsync([this, results = std::move(results)]() {
            albums.clear();
            albums.reserve(results.size());
            for (auto& album : results) {
                albums.push_back(std::move(album));
            }
            
            // 컨테이너 크기 업데이트
            const int albumSize = 200;
            const int spacing = 20;
            const int totalWidth = (albumSize + spacing) * albums.size();
            albumContainer->setBounds(0, 0, totalWidth, albumViewport->getHeight());
            
            albumContainer->repaint();  // 컨테이너 다시 그리기
            
            for (int i = 0; i < albums.size(); ++i) {
                loadAlbumCover(i);
            }
        });
    }).detach();
}

void HomePage::loadAlbumCover(int index)
{
    if (index >= albums.size()) return;
    
    std::thread([this, index]() {
        if (!albums[index].coverUrl.isEmpty())
        {
            juce::Thread::sleep(index * 20);
            
            auto coverImage = SpotifyService::loadAlbumCover(albums[index].coverUrl);
            
            if (coverImage != nullptr)
            {
                auto* comp = this;
                auto* img = coverImage.release();
                
                juce::MessageManager::callAsync([comp, index, img]() {
                    std::unique_ptr<juce::Image> image(img);
                    if (index < comp->albums.size())
                    {
                        comp->albums[index].coverImage = std::move(image);
                        comp->albums[index].alpha = 0.0f;
                        comp->albumContainer->repaint();
                    }
                });
            }
        }
    }).detach();
}

void HomePage::paintAlbumContainer(juce::Graphics& g)
{
    const int albumSize = 200;
    const int spacing = 20;
    const int textHeight = 30;
    const int imageSize = albumSize - textHeight;

    for (int i = 0; i < albums.size(); ++i)
    {
        auto albumBounds = juce::Rectangle<int>(
            i * (albumSize + spacing), 0,
            albumSize, albumSize
        );

        auto imageArea = albumBounds.removeFromTop(imageSize);

        // 플레이스홀더 박스 그리기
        g.setColour(juce::Colours::black);
        g.fillRoundedRectangle(imageArea.toFloat(), 8.0f);

        if (albums[i].coverImage)
        {
            g.setOpacity(albums[i].alpha);
            g.drawImage(*albums[i].coverImage,
                       imageArea.toFloat(),
                       juce::RectanglePlacement::centred);
            g.setOpacity(1.0f);

            g.setColour(MapleColours::currentTheme.text);
            g.setFont(MapleTypography::getPretendardMedium(12.0f));
            g.drawText(albums[i].name,
                      albumBounds,
                      juce::Justification::centred, true);
        }
        else
        {
            g.setColour(MapleColours::currentTheme.text.withAlpha(0.5f));
            g.setFont(MapleTypography::getPretendardMedium(14.0f));
            g.drawText(juce::String::fromUTF8(u8"로딩중..."),
                      imageArea,
                      juce::Justification::centred, true);
        }
    }
}

void HomePage::scrollAlbums(bool scrollRight)
{
    const int scrollAmount = 220;  // 한 번에 스크롤할 픽셀 양 (앨범 크기 + 간격)
    auto currentPosition = albumViewport->getViewPositionX();
    auto targetPosition = scrollRight ? 
        currentPosition + scrollAmount : 
        currentPosition - scrollAmount;
    
    // 범위 제한
    targetPosition = juce::jlimit(0, 
                                albumContainer->getWidth() - albumViewport->getWidth(),
                                targetPosition);
    
    albumViewport->setViewPosition(targetPosition, albumViewport->getViewPositionY());
}