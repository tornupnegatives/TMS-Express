# Copyright (C) 2024 Joseph Bellahcen <joeclb@icloud.com>
# cmake -B build
# cd build
# cmake --build .
# cmake --install . --prefix .

#######################
# CMake Configuration #
########################

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

#########################
# Project Configuration #
#########################

project(tmsexpress)
option(TMSEXPRESS_BUILD_TESTS "Build test programs" ON)
option(TMSEXPRESS_BUILD_GUI "Build GUI frontend" ON)

################
# Dependencies #
################

find_package(Qt6 COMPONENTS Core Gui Multimedia Widgets REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SndFile REQUIRED IMPORTED_TARGET sndfile)

file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

cpmaddpackage(
    CLI11
    GITHUB_REPOSITORY
    CLIUtils/CLI11
    VERSION
    2.6.2)

cpmaddpackage(
    json
    GITHUB_REPOSITORY
    nlohmann/json
    VERSION
    3.12.0)

cpmaddpackage(
    samplerate
    GITHUB_REPOSITORY
    libsndfile/libsamplerate
    GIT_TAG
    0.2.2
    OPTIONS
    "BUILD_TESTING OFF")

#########################
# Project Configuration #
#########################

qt_standard_project_setup()

include_directories(${CMAKE_CURRENT_LIST_DIR} src)
include_directories(${json_SOURCE_DIR}/include)

qt_add_executable(
    ${PROJECT_NAME}
    src/audio/AudioBuffer.cpp
    src/audio/AudioFilter.cpp
    src/analysis/Autocorrelation.cpp
    src/analysis/PitchEstimator.cpp
    src/analysis/LinearPredictor.cpp
    src/encoding/Frame.cpp
    src/encoding/FrameEncoder.cpp
    src/encoding/FramePostprocessor.cpp
    src/encoding/Synthesizer.cpp
    src/bitstream/BitstreamGenerator.cpp
    src/bitstream/PathUtils.cpp
    src/ui/cli/CommandLineApp.cpp
    src/ui/gui/audiowaveform/AudioWaveform.cpp
    src/ui/gui/audiowaveform/AudioWaveformView.cpp
    src/ui/gui/controlpanels/ControlPanelView.cpp
    src/ui/gui/controlpanels/ControlPanelPitchView.cpp
    src/ui/gui/controlpanels/ControlPanelLpcView.cpp
    src/ui/gui/controlpanels/ControlPanelPostView.cpp
    src/ui/gui/MainWindow.cpp
    src/main.cpp
)

if(TMSEXPRESS_BUILD_GUI)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TMSEXPRESS_GUI)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Multimedia
    PkgConfig::SndFile
    CLI11
    samplerate
)

##############
# Deployment #
##############

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(deploy_tool_options_arg "")

if(APPLE)
    set(HOMEBREW_OPT "/opt/homebrew/opt")

    set(deploy_tool_options_arg
        -codesign=-
        -libpath=${HOMEBREW_OPT}/qtsvg/lib
        -libpath=${HOMEBREW_OPT}/qtvirtualkeyboard/lib
        -libpath=${HOMEBREW_OPT}/qtwebengine/lib
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
)
endif()

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
)
install(SCRIPT ${deploy_script})
