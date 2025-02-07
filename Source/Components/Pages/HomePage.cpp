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
    // 섹션 헤더 그리기
    auto headerBounds = bounds.removeFromTop(40);
    drawSectionHeader(g, juce::String::fromUTF8(u8"RECOMMENDED"), headerBounds, true);

    // 앨범 표시 영역
    auto contentBounds = bounds;
    const int albumSize = 200;  // 앨범 전체 크기
    const int spacing = 20;     // 앨범 간 간격
    const int textHeight = 30;  // 텍스트 영역 높이
    const int imageSize = albumSize - textHeight;  // 이미지 크기

    // 앨범 간 간격을 고려한 전체 너비 계산
    int totalWidth = (albumSize * 6) + (spacing * 5);
    int startX = (contentBounds.getWidth() - totalWidth) / 2;  // 중앙 정렬

    for (int i = 0; i < juce::jmin(6, albums.size()); ++i)
    {
        auto albumBounds = juce::Rectangle<int>(
            contentBounds.getX() + startX + (i * (albumSize + spacing)),
            contentBounds.getY(),
            albumSize,
            albumSize
        );

        if (albums[i].coverImage)
        {
            auto imageArea = albumBounds.removeFromTop(imageSize);
            
            // 각 앨범의 개별 알파값 사용
            g.setOpacity(albums[i].alpha);
            g.drawImage(*albums[i].coverImage,
                       imageArea.toFloat(),
                       juce::RectanglePlacement::centred);
            g.setOpacity(1.0f);

            // 텍스트는 항상 완전 불투명하게
            g.setColour(MapleColours::currentTheme.text);
            g.setFont(MapleTypography::getPretendardMedium(12.0f));
            g.drawText(albums[i].name,
                      albumBounds,
                      juce::Justification::centred, true);
        }
    }
}

void HomePage::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    // 프로젝트 시작 버튼 크기 및 위치 설정
    auto projectButtonArea = bounds.removeFromTop(200);
    createProjectBtn->setBounds(projectButtonArea.reduced(10).withHeight(160));
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
        
        juce::MessageManager::callAsync([this, results]() mutable {
            albums = std::move(results);
            repaint();
            
            // 모든 이미지를 병렬로 로드 시작
            for (int i = 0; i < albums.size(); ++i)
            {
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
                        comp->albums.getReference(index).coverImage = std::move(image);
                        comp->albums.getReference(index).alpha = 0.0f;
                        comp->repaint();
                    }
                });
            }
        }
    }).detach();
}