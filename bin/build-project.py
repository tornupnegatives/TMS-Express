import os
import logging
import platform
import requests
import subprocess
import zipfile

# Script variables
MACOS_DEPS = ["cmake", "libsndfile", "pkg-config", "qt"]
LINUX_DEPS = ["cmake", "libsndfile1-dev", "pkg-config", "qt6-base-dev", "qt6-multimedia-dev", "libgl1-mesa-dev"]
WIN_DEPS = ["cmake", "pkgconfiglite", "qt6-base-dev"]

PROJ_CONF_CMD = ["cmake", "-B", "build"]
PROJ_BUILD_CMD = ["cmake", "--build", "build", "--parallel"]

# Pacakage manager commands
# To be combined with one of the `*_DEPS` variables
MACOS_PKG_CMD = ["brew", "install"]
LINUX_PKG_CMD = ["sudo", "apt", "install"]
WIN_PKG_CMD = ["choco", "install"]

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

import os
import requests

def download_github_release_asset(repo, tag, asset_name, output_dir):
    """
    Download a specific asset from a GitHub release.

    Parameters:
        repo (str): The GitHub repository in 'owner/repo' format.
        tag (str): The release tag (e.g., '1.2.2').
        asset_name (str): The exact name of the asset to download.
        output_dir (str): Directory to save the downloaded asset.

    Raises:
        RuntimeError: If the release or asset is not found.
    """
    os.makedirs(output_dir, exist_ok=True)
    releases_url = f"https://api.github.com/repos/{repo}/releases"
    headers = {
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28"
    }
    resp = requests.get(releases_url, headers=headers)
    resp.raise_for_status()
    releases = resp.json()

    # Find the release with the correct tag
    release = next((r for r in releases if r["tag_name"] == tag), None)
    if not release:
        raise RuntimeError(f"Release tag {tag} not found.")

    # Find the asset with the desired name
    asset = next((a for a in release["assets"] if a["name"] == asset_name), None)
    if not asset:
        raise RuntimeError(f"Asset {asset_name} not found in release {tag}.")

    # Download the asset
    asset_path = os.path.join(output_dir, asset_name)
    with requests.get(asset["browser_download_url"], stream=True) as r:
        r.raise_for_status()
        with open(asset_path, "wb") as f:
            for chunk in r.iter_content(chunk_size=8192):
                f.write(chunk)
    print(f"Downloaded {asset_name} to {asset_path}")

def windows_specific_post_config():
    if not os.path.exists("./build"):
        os.mkdir("./build")

    # Download and unzip `libsndfile`
    download_github_release_asset(
        repo="libsndfile/libsndfile",
        tag="1.2.2",
        asset_name="libsndfile-1.2.2-win64.zip",
        output_dir="./build"
    )

    # loading the temp.zip and creating a zip object 
    with zipfile.ZipFile("./build/libsndfile-1.2.2-win64.zip") as zip_file:
        zip_file.extractall(path="./build/.")

# Build install command
if system_os == "Darwin":
    install_cmd = MACOS_PKG_CMD + MACOS_DEPS
elif system_os == "Linux":
    install_cmd = LINUX_PKG_CMD + LINUX_DEPS
elif system_os == "Windows":
    install_cmd = WIN_PKG_CMD + WIN_DEPS
    PROJ_CONF_CMD += ["-G", "MinGW Makefiles"]

    windows_specific_post_config()

# Install dependencies from package manager
run_command_and_check_status(install_cmd)
run_command_and_check_status(PROJ_CONF_CMD)
run_command_and_check_status(PROJ_BUILD_CMD)
