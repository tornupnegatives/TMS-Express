cmake_minimum_required(VERSION 3.21)
project(TMS-Express)
set(CMAKE_CXX_STANDARD 14)

# Set debug flags
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "_DEBUG")
else ()
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "NDEBUG")
endif ()

# Include libsndfile and libsamplerate
if(UNIX)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif(UNIX)

include_directories(inc/)
add_executable(${PROJECT_NAME}
        src/main.cpp

        src/Audio/AudioBuffer.cpp
        src/Audio/AudioPreprocessor.cpp

        src/LPC_Analysis/Autocorrelator.cpp
        src/LPC_Analysis/LowerVocalTractAnalyzer.cpp
        src/LPC_Analysis/UpperVocalTractAnalyzer.cpp

        src/Frame_Encoder/TmsFrame.cpp
        src/Frame_Encoder/Frame.cpp)

target_link_libraries(${PROJECT_NAME}
        sndfile
        samplerate)