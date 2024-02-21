# Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

###############################################################################
# GoogleTest Framework ########################################################
###############################################################################

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "v1.14.0")

set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
enable_testing()

###############################################################################
# Project Sources & Includes ##################################################
###############################################################################

set(TMSEXPRESS_TEST_TARGET tmsexpress-test)

include_directories(${CMAKE_PROJECT_TOP_LEVEL_INCLUDES})

add_executable(
    ${TMSEXPRESS_TEST_TARGET}
    src/analysis/Autocorrelation.cpp
    test/AutocorrelatorTests.cpp
    src/encoding/Frame.cpp
    test/FrameTests.cpp
    src/encoding/FrameEncoder.cpp
    test/FrameEncoderTests.cpp)

###############################################################################
# Project Dependencies ########################################################
###############################################################################

target_link_libraries(${TMSEXPRESS_TEST_TARGET} gtest_main)

include(GoogleTest)
gtest_discover_tests(${TMSEXPRESS_TEST_TARGET})
