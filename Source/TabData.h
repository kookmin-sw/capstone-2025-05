#include <JUCEHeader.h>
#include "gp_parser.h"

class TabData
{
public:
    TabData(const juce::String& filePath)
        : tabFile(gp_parser::Parser(filePath.toRawUTF8()).getTabFile())
    {
    }

    String getTitle() const { return tabFile.title; }
    String getArtist() const { return tabFile.artist; }
    const std::vector<gp_parser::Track>& getTracks() const { return tabFile.tracks; }

private:
    gp_parser::TabFile tabFile;
};