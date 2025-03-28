#pragma once
#include <JuceHeader.h>

class ContentPanelComponent : public juce::Component
{
public:
    ContentPanelComponent();
    void resized() override;

private:
    class AlbumThumbnailComponent : public juce::Component
    {
    public:
        AlbumThumbnailComponent(const juce::String& title, const juce::Image& thumbnailImage = juce::Image())
            : albumTitle(title), thumbnail(thumbnailImage)
        {
            if (thumbnail.isNull())
                thumbnail = juce::Image(juce::Image::RGB, 150, 150, true);
        }

        void paint(juce::Graphics& g) override
        {
            // Draw thumbnail
            g.fillAll(juce::Colours::white);
            g.setColour(juce::Colours::lightgrey);
            g.drawRect(getLocalBounds(), 1);
            
            auto thumbnailBounds = getLocalBounds().withHeight(getHeight() - 30);
            g.drawImageWithin(thumbnail, thumbnailBounds.getX(), thumbnailBounds.getY(), 
                             thumbnailBounds.getWidth(), thumbnailBounds.getHeight(),
                             juce::RectanglePlacement::centred);
            
            // Draw title below thumbnail
            g.setColour(juce::Colours::black);
            g.drawText(albumTitle, 0, getHeight() - 30, getWidth(), 30, juce::Justification::centred);
        }
        
    private:
        juce::String albumTitle;
        juce::Image thumbnail;
    };
    
    class AlbumGridView : public juce::Component
    {
    public:
        AlbumGridView()
        {
            // Setup default properties
            numColumns = 3;
            thumbnailSize = 150;
            spacing = 10;
        }
        
        void addAlbum(const juce::String& title, const juce::Image& thumbnail = juce::Image())
        {
            auto* newItem = new AlbumThumbnailComponent(title, thumbnail);
            addAndMakeVisible(newItem);
            thumbnails.add(newItem);
            resized();
        }
        
        void clear()
        {
            thumbnails.clear();
        }
        
        void setNumColumns(int columns)
        {
            numColumns = jmax(1, columns);
            resized();
        }
        
        void setThumbnailSize(int size)
        {
            thumbnailSize = jmax(50, size);
            resized();
        }
        
        void setSpacing(int newSpacing)
        {
            spacing = jmax(0, newSpacing);
            resized();
        }
        
        void resized() override
        {
            auto bounds = getLocalBounds();
            
            // Calculate item width including spacing
            int itemWidth = thumbnailSize;
            int itemHeight = thumbnailSize + 30; // Adding space for text
            
            int row = 0;
            int col = 0;
            
            for (auto* thumbnail : thumbnails)
            {
                thumbnail->setBounds(col * (itemWidth + spacing), 
                                    row * (itemHeight + spacing),
                                    itemWidth, itemHeight);
                
                // Move to next column or row
                col++;
                if (col >= numColumns)
                {
                    col = 0;
                    row++;
                }
            }
            
            // Set component size based on content
            int minHeight = thumbnails.isEmpty() ? 0 : 
                            ((thumbnails.size() - 1) / numColumns + 1) * (itemHeight + spacing) - spacing;
            setSize(getWidth(), minHeight);
        }
        
    private:
        juce::OwnedArray<AlbumThumbnailComponent> thumbnails;
        int numColumns;
        int thumbnailSize;
        int spacing;
    };

    juce::Label recentTitle;
    AlbumGridView recentGrid;
    juce::TextButton viewAllRecent;

    juce::Label recommendTitle;
    AlbumGridView recommendGrid;
    juce::TextButton viewAllRecommend;
    
    // Add sample data initialization method
    void initializeSampleData();
};