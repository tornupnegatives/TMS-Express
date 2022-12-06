###############################################################################
# Project: TMS-Express
#
# File: FindCLI11.cmake
#
# Description: Imports CLI11 via GitHub
#
# Author: Joseph Bellahcen <joeclb@icloud.com>
###############################################################################

include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

FetchContent_Declare(CLI11
        GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
        GIT_TAG "v2.2.0"
        )

FetchContent_MakeAvailable(CLI11)
