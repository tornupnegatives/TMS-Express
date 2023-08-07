# Copyright (C) 2023 Joseph Bellahcen <joeclb@icloud.com>

###############################################################################
# CMake Configuration #########################################################
###############################################################################

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

###############################################################################
# Project Configuration #######################################################
###############################################################################

project(TMS-Test)

###############################################################################
# GoogleTest Framework ########################################################
###############################################################################

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG "release-1.12.1"
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

set (TEST_TARGET_DIR ${CMAKE_CURRENT_LIST_DIR}/../tms_express)
message(${TEST_TARGET_DIR})

add_executable(${PROJECT_NAME}
    ${TEST_TARGET_DIR}/LPC_Analysis/Autocorrelation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AutocorrelatorTests.cpp

    ${TEST_TARGET_DIR}/Frame_Encoding/Frame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FrameTests.cpp

    ${TEST_TARGET_DIR}/Frame_Encoding/FrameEncoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FrameEncoderTests.cpp
)

###############################################################################
# Project Dependencies ########################################################
###############################################################################

target_link_libraries(
    ${PROJECT_NAME}
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(TMS-Test)
