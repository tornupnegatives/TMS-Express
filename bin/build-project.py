import os
import logging
import platform
import subprocess

# Script variables
MACOS_DEPS = ["cmake", "libsndfile", "pkg-config", "qt"]
LINUX_DEPS = ["cmake", "libsndfile1-dev", "pkg-config", "qt6-base-dev", "qt6-multimedia-dev", "libgl1-mesa-dev"]

PROJ_CONF_CMD = ["cmake", "-B", "build"]
PROJ_BUILD_CMD = ["cmake", "--build", "build", "--parallel"]

# Pacakage manager commands
# To be combined with one of the `*_DEPS` variables
MACOS_PKG_CMD = ["brew", "install"]
LINUX_PKG_CMD = ["sudo", "apt", "install"]

# Set up logging
logging.basicConfig(filename="tms-express-build.log", level=logging.INFO)
logger = logging.getLogger()

# Detect OS
system_os = platform.system()

if system_os in ["Windows", "Linux", "Darwin"]:
    logger.info(f"Detected OS: {system_os}")
else:
    logger.fatal(f"Unrecognized OS: {system_os}")
    exit(1)

def run_command_and_check_status(command: str):
    logger.info(f"Executing command: {command}")
    result = subprocess.run(command)
    return_code = result.returncode

    log_message = f"Command finished with code: {result.returncode}"
    if return_code == 0:
        logger.info(log_message)
        return
    else:
        logger.fatal(log_message)
        exit(1)

# Install dependencies from package manager
run_command_and_check_status(["brew", "install"] + MACOS_DEPS)
run_command_and_check_status(PROJ_CONF_CMD)
run_command_and_check_status(PROJ_BUILD_CMD)
