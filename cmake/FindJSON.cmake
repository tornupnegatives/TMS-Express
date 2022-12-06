###############################################################################
# Project: TMS-Express
#
# File: FindJSON.cmake
#
# Description: Imports nlohmann_json via GitHub
#
# Author: Joseph Bellahcen <joeclb@icloud.com>
###############################################################################

include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

FetchContent_Declare(json
        URL "https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz"
        )

FetchContent_MakeAvailable(json)
