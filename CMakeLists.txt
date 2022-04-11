cmake_minimum_required(VERSION 3.21)
project(TMS-Express)
set(CMAKE_CXX_STANDARD 14)

# Set debug flags
# https://stackoverflow.com/a/58784558/15709321
if (CMAKE_BUILD_TYPE STREQUAL "")
    # Build type is not set eg. command was "cmake .."
    message(STATUS "  Diag: Build type was unspecified, set to Debug")
    set(CMAKE_BUILD_TYPE Debug)
else ()
    message(STATUS "  Diag: Build type specified as '${CMAKE_BUILD_TYPE}'")
endif ()

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
        src/AudioBuffer.cpp
        src/PitchEstimator.cpp
        inc/debug.h)

target_link_libraries(${PROJECT_NAME}
        sndfile
        samplerate)