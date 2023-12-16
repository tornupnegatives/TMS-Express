# Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

###############################################################################
# GoogleTest Framework ########################################################
###############################################################################

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "v1.14.0"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

###############################################################################
# Project Sources & Includes ##################################################
###############################################################################

include_directories(
    ${CMAKE_PROJECT_TOP_LEVEL_INCLUDES}
)

add_executable(TMS-Express-Test
    tms_express/LPC_Analysis/Autocorrelation.cpp
    test/AutocorrelatorTests.cpp

    tms_express/Frame_Encoding/Frame.cpp
    test/FrameTests.cpp

    tms_express/Frame_Encoding/FrameEncoder.cpp
    test/FrameEncoderTests.cpp
)

###############################################################################
# Project Dependencies ########################################################
###############################################################################

target_link_libraries(
    TMS-Express-Test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(TMS-Express-Test)
