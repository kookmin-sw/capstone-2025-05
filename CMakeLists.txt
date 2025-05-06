cmake_minimum_required(VERSION 3.15)

project(MapleClientDesktop VERSION 0.1.0)

add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

# ASIO SDK 경로를 external 폴더로 지정
set(ASIO_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/asio-sdk")

juce_add_gui_app(MapleClientDesktop
    # App version info
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "KMU"    
    COMPANY_WEBSITE "lovelyoverflow.github.io"
    COMPANY_EMAIL "lovelyoverflow@gmail.com"
    
    # Windows GUI 애플리케이션 설정
    IS_CONSOLE_APP FALSE
    
    # Icon files
    # ICON_BIG "Assets/icon.png"
    
    # Bundle identifier for macOS
    BUNDLE_ID "com.KMU.mapleclientdesktop"
)

juce_generate_juce_header(MapleClientDesktop)

# Sources organized by MVC
target_sources(MapleClientDesktop PRIVATE
    # Main application files
    Source/Main.cpp
    
    # Model
    Source/Model/AudioModel.cpp
    Source/Model/AudioModel.h
    Source/Model/ContentModel.cpp
    Source/Model/ContentModel.h
    Source/Model/Song.h
    Source/Model/IAudioModelListener.h
    Source/Model/TabPlayer.cpp
    Source/Model/TabPlayer.h
    Source/Model/gp_parser.cpp
    Source/Model/gp_parser.h
    Source/Model/SpotifyService.cpp
    Source/Model/SpotifyService.h
    Source/Model/RecordingThumbnail.cpp
    Source/Model/RecordingThumbnail.h
    Source/Model/AudioRecorder.cpp
    Source/Model/AudioRecorder.h
    
    # API
    Source/API/SongsAPIService.cpp
    Source/API/SongsAPIService.h
    
    # View
    Source/View/MainComponent.cpp
    Source/View/MainComponent.h
    Source/View/GuitarPracticeComponent.cpp
    Source/View/GuitarPracticeComponent.h
    Source/View/ContentPanelComponent.cpp
    Source/View/ContentPanelComponent.h
    Source/View/ScoreComponent.cpp
    Source/View/ScoreComponent.h
    Source/View/MainActionComponent.cpp
    Source/View/MainActionComponent.h
    Source/View/TopBar.cpp
    Source/View/TopBar.h
    Source/View/CenterPanel.cpp
    Source/View/CenterPanel.h
    Source/View/LeftPanel.cpp
    Source/View/LeftPanel.h
    Source/View/RightPanel.cpp
    Source/View/RightPanel.h
    Source/View/HeaderComponent.cpp
    Source/View/HeaderComponent.h
    Source/View/AlbumGridView.cpp
    Source/View/AlbumGridView.h
    Source/View/AlbumThumbnailComponent.cpp
    Source/View/AlbumThumbnailComponent.h
    Source/View/PracticeSettingsComponent.cpp
    Source/View/PracticeSettingsComponent.h
    
    # View/Dialog
    Source/View/Dialog/AudioSettingsDialog.h
    Source/View/Dialog/NewProjectDialog.cpp
    Source/View/Dialog/NewProjectDialog.h
    
    # Controller
    Source/Controller/AudioController.cpp
    Source/Controller/AudioController.h
    Source/Controller/TransportController.cpp
    Source/Controller/TransportController.h
    Source/Controller/ContentController.cpp
    Source/Controller/ContentController.h
    Source/Controller/GuitarPracticeController.cpp
    Source/Controller/GuitarPracticeController.h
    Source/Controller/AmpliTubeProcessor.cpp
    Source/Controller/AmpliTubeProcessor.h
    
    # Event
    Source/Event/Event.h
    Source/Event/EventBus.h
    Source/Event/IEventListener.h
    
    # Util
    Source/Util/AppConfig.h
    Source/Util/EnvLoader.h
    Source/Util/xml_module.cpp
    )

target_compile_definitions(MapleClientDesktop
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=1  # URL 기반 API 통신을 위해 활성화
        JUCE_USE_CURL=1     # HTTP 요청을 위해 CURL 활성화
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:MapleClientDesktop,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:MapleClientDesktop,JUCE_VERSION>"
        JUCE_ASIO=1  # ASIO 활성화
        JUCE_PLUGINHOST_VST3=1  # VST3 호스팅 활성화
)

# Set C++ standard
target_compile_features(MapleClientDesktop PRIVATE cxx_std_17)

# Include JUCE modules
target_link_libraries(MapleClientDesktop PRIVATE
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
)

# Windows 애플리케이션으로 빌드하도록 설정
if(WIN32)
    set_target_properties(MapleClientDesktop PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Set binary output directories
set_target_properties(MapleClientDesktop PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Operating system specific settings
if(MSVC)
    target_compile_options(MapleClientDesktop PRIVATE /W4)
else()
    target_compile_options(MapleClientDesktop PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Resource files (if needed)
# juce_add_binary_data(ProjectResources
#     SOURCES
#         # Assets/icon.png
#         # Add other assets here
# )

# target_link_libraries(MapleClientDesktop PRIVATE ProjectResources)

# ASIO SDK include 경로 추가
target_include_directories(MapleClientDesktop
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Source"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/asio-sdk/common"
        "${CMAKE_CURRENT_SOURCE_DIR}/external/asio-sdk/host"
)

# target_precompile_headers(MapleClientDesktop
#     PRIVATE
#         <JuceHeader.h>
#         <juce_audio_basics/juce_audio_basics.h>
#         <juce_audio_devices/juce_audio_devices.h>
# )
add_definitions(-DJUCE_ENABLE_ASSERTIONS=0)