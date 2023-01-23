# Author: Joseph Bellahcen <joeclb@icloud.com>

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(TMS-Express)

#  ====================================
#             PROGRAM FILES
#  ====================================
include_directories(inc/)
add_executable(${PROJECT_NAME}
        src/Audio/AudioBuffer.cpp
        src/Audio/AudioFilter.cpp

        src/LPC_Analysis/Autocorrelator.cpp
        src/LPC_Analysis/PitchEstimator.cpp
        src/LPC_Analysis/LinearPredictor.cpp

        src/Frame_Encoding/Frame.cpp
        src/Frame_Encoding/FrameEncoder.cpp
        src/Frame_Encoding/FramePostprocessor.cpp

        src/Interfaces/BitstreamGenerator.cpp
        src/Interfaces/PathUtils.cpp

        src/main.cpp
        )

#  ====================================
#              DEPENDENCIES
#  ====================================
include(cmake/CPM.cmake)

# Generate libsndfile DLL on Windows
if (WIN32)
    set(ENABLE_STATIC_RUNTIME ON)
endif()

CPMAddPackage(
        NAME nlohmann_json
        GITHUB_REPOSITORY nlohmann/json
        VERSION 3.11.2
        OPTIONS "JSON_BuildTests OFF"
)

CPMAddPackage(
        NAME CLI11
        GITHUB_REPOSITORY CLIUtils/CLI11
        VERSION 2.3.2
)

CPMAddPackage(
        NAME libsndfile
        GITHUB_REPOSITORY libsndfile/libsndfile
        GIT_TAG 1.2.0
        OPTIONS
        "BUILD_PROGRAMS OFF"
        "BUILD_EXAMPLES OFF"
        "BUILD_TESTING OFF"
        "ENABLE_PACKAGE_CONFIG OFF"
        "INSTALL_PKGCONFIG_MODULE OFF"
        "INSTALL_MANPAGES OFF"
)

CPMAddPackage(
        NAME libsamplerate
        GITHUB_REPOSITORY libsndfile/libsamplerate
        GIT_TAG 0.2.2
        OPTIONS
        "BUILD_TESTING OFF"
)

target_link_libraries(${PROJECT_NAME}
        CLI11
        nlohmann_json::nlohmann_json
        sndfile
        samplerate
        )

#  ====================================
#                  HACKS
#  ====================================
if (APPLE)
    # On macOS, suppress warnings regarding <experimental/filesystem>
    target_compile_definitions(${PROJECT_NAME} PRIVATE _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM)
else()
    # On other systems, link the filesystem library
    target_link_libraries(${PROJECT_NAME}
            stdc++fs
            )
endif(APPLE)

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)
