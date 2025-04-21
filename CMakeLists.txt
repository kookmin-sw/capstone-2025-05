# Example GUI App CMakeLists.txt

# To get started on a new GUI app, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications. For other examples of CMakeLists for
# GUI apps, check `extras/Projucer` and `examples/DemoRunner` in the JUCE repo.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.15)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(MapleClientDesktop VERSION 0.1.0)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

# If your app depends the VST2 SDK, perhaps to host VST2 plugins, CMake needs to be told where
# to find the SDK on your system. This setup should be done before calling `juce_add_gui_app`.

# juce_set_vst2_sdk_path(...)

# ASIO SDK 경로를 external 폴더로 지정
set(ASIO_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/asio-sdk")

# `juce_add_gui_app` adds an executable target with the name passed as the first argument
# (MapleClientDesktop here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. This function accepts many optional arguments. Check the readme at
# `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_gui_app(MapleClientDesktop
    # App version info
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "KMU"    
    COMPANY_WEBSITE "lovelyoverflow.github.io"
    COMPANY_EMAIL "lovelyoverflow@gmail.com"
    
    # Icon files
    # ICON_BIG "Assets/icon.png"
    
    # Bundle identifier for macOS
    BUNDLE_ID "com.KMU.mapleclientdesktop"
)

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

juce_generate_juce_header(MapleClientDesktop)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

# Sources organized by MVC
target_sources(MapleClientDesktop PRIVATE
    # Main application files
    Source/Main.cpp
    Source/AmpliTubeProcessor.cpp
    Source/AmpliTubeProcessor.h
    Source/ContentPanelComponent.cpp
    Source/ContentPanelComponent.h
    Source/BottomComponent.cpp
    Source/BottomComponent.h
    Source/NewProjectDialog.cpp
    Source/NewProjectDialog.h
    Source/SpotifyService.cpp
    Source/SpotifyService.h
    Source/EnvLoader.h
    Source/MainActionComponent.cpp
    Source/MainActionComponent.h
    Source/MainComponent.cpp
    Source/MainComponent.h
    Source/ScoreComponent.cpp
    Source/ScoreComponent.h
    Source/TabPlayer.cpp
    Source/TabPlayer.h
    
    # Model
    Source/Model/AudioModel.cpp
    Source/Model/AudioModel.h
    Source/Model/ContentModel.cpp
    Source/Model/ContentModel.h
    Source/Model/Song.h
    
    # View
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
    Source/View/GuitarPracticeComponent.cpp
    Source/View/GuitarPracticeComponent.h
    
    # Controller
    Source/Controller/AudioController.cpp
    Source/Controller/AudioController.h
    Source/Controller/TransportController.cpp
    Source/Controller/TransportController.h
    Source/Controller/ContentController.cpp
    Source/Controller/ContentController.h
    Source/Controller/GuitarPracticeController.cpp
    Source/Controller/GuitarPracticeController.h

    Source/gp_parser.cpp
    Source/gp_parser.h
)

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(MapleClientDesktop
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
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
)

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