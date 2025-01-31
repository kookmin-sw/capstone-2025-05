#include "MapleColours.h"

namespace MapleColours
{
    Theme currentTheme = Dark::theme; // 기본값으로 다크 테마 사용

    void setTheme(bool isDark)
    {
        currentTheme = isDark ? Dark::theme : Light::theme;
    }
}