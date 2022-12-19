###############################################################################
# Project: TMS-Express
#
# File: FindIIR1.cmake
#
# Description: Imports IIR1 via GitHub
#
# Author: Joseph Bellahcen <joeclb@icloud.com>
###############################################################################

include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

FetchContent_Declare(IIR1
        GIT_REPOSITORY "https://github.com/berndporr/iir1.git"
        GIT_TAG "master"
        )

FetchContent_MakeAvailable(IIR1)
set_target_properties(ecg50hzfilt iirdemo rbj_update PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
