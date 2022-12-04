cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(TMS-Express)

# Import libsndfile and libsamplerate
include(cmake/FindSamplerate.cmake)
include(cmake/FindCLI11.cmake)
include(test/TmsTest.cmake)

# Project files
include_directories(
        inc/
        ${LIBSNDFILE_INCLUDE_DIR}
        ${LIBSAMPLERATE_INCLUDE_DIR}
        )
        
add_executable(${PROJECT_NAME}
        src/Audio/AudioBuffer.cpp
        src/Audio/AudioPreprocessor.cpp

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

# Rename executable
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME tmsexpress)

# Link dependencies
target_link_libraries(${PROJECT_NAME}
        CLI11
        ${LIBSNDFILE_LIBRARY}
        ${LIBSAMPLERATE_LIBRARY}
        )

# On macOS, supress warnings regarding <experimental/filesystem
# On other systems, link the filesystem library
if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM)
else()
    target_link_libraries(${PROJECT_NAME}
        stdc++fs
        )
endif(APPLE)

