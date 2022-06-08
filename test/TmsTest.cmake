cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(TMS-Test
        src/LPC_Analysis/Autocorrelator.cpp
        test/AutocorrelatorTests.cpp

        src/Frame_Encoding/Frame.cpp
        test/FrameTests.cpp

        src/Frame_Encoding/FrameEncoder.cpp
        test/FrameEncoderTests.cpp
        )

target_link_libraries(
        TMS-Test
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(TMS-Test)