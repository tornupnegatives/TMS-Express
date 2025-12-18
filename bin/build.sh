#!/usr/bin/env bash

set -e

function get_os() {
    case "$OSTYPE" in
    solaris*) echo "unknown ($OSTYPE)" ;;
    darwin*)  echo "Darwin" ;; 
    linux*)   echo "Linux" ;;
    bsd*)     echo "unknown ($OSTYPE)" ;;
    msys*)    echo "Windows" ;;
    *)        echo "unknown ($OSTYPE)" ;;
    esac
}

build_macos() {
    brew install cmake libsndfile pkg-config qt

    local build_dir="./build"
    local qt_path=$(brew --prefix qt)

    export CMAKE_PREFIX_PATH=$qt_path
    export LDFLAGS=-L/opt/homebrew/lib

    cmake -B $build_dir -D "CMAKE_POLICY_VERSION_MINIMUM=3.5" "-DTMSEXPRESS_BUILD_TESTS=OFF"
    cmake --build $build_dir

    local release_pkg="./build/tmsexpress.app"
    local release_archive="./build/TMS-Express-Darwin.zip"

    # Manually copy missing QtDBus framework that macdeployqt fails to detect
    echo "Manually copying QtDBus framework..."
    mkdir -p $release_pkg/Contents/Frameworks
    cp -R $qt_path/lib/QtDBus.framework $release_pkg/Contents/Frameworks/

    # Run macdeployqt (ignore rpath errors - they're harmless)
    macdeployqt $release_pkg 2>&1 | grep -v "ERROR: Cannot resolve rpath" || true

    # Sign the app
    codesign --deep --force --sign - $release_pkg
    #zip -r $release_archive $release_pkg
}

build_linux() {
    export DEBIAN_FRONTEND=noninteractive
    sudo apt update
    sudo apt upgrade -y
    sudo apt install -y \
        cmake pkg-config qmake6 \
        wget file patchelf \
        libsndfile1-dev \
        qt6-base-dev qt6-multimedia-dev qt6-wayland \
        libgl1-mesa-dev  \
        gstreamer1.0-plugins-base \
        gstreamer1.0-plugins-good \
        gstreamer1.0-alsa

    wget -nv \
        https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage \
        -O ./build/linuxdeploy-x86_64.AppImage
    chmod +x ./build/linuxdeploy-x86_64.AppImage

    wget -nv \
        https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage \
        -O ./build/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x ./build/linuxdeploy-plugin-qt-x86_64.AppImage

    wget -nv \
        https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gstreamer/refs/heads/master/linuxdeploy-plugin-gstreamer.sh \
        -O ./build/linuxdeploy-plugin-gstreamer.sh
    chmod +x ./build/linuxdeploy-plugin-gstreamer.sh

    cmake -B $build_dir -DTMSEXPRESS_BUILD_TESTS=OFF
    cmake --build $build_dir

    local appimage_dir="./build/AppDir"
    local binary_path="./build/AppDir/usr/bin/tmsexpress"
    local desktop_path="./build/AppDir/TMS-Express.desktop"

    #cp -r ./dist/linux $appimage_dir
    #mkdir -p $appimage_dir/usr/bin
    #cp ./build/tmsexpress $binary_path
    #cp "./bin/TMS-Express.desktop" $desktop_path    

    #local icon_path="./doc/icon.png"
    #local appimage_icon_path="$appimage_dir/tmsexpress.png"
    #cp $icon_path $appimage_icon_path

    cd ./build
    export APPIMAGE_EXTRACT_AND_RUN=1
    export QMAKE=qmake6
    export EXTRA_QT_PLUGINS="multimedia"
    export EXTRA_PLATFORM_PLUGINS="libqwayland-egl.so;libqwayland-generic.so"
    export EXTRA_QT_MODULES="waylandcompositor"
    ./linuxdeploy-x86_64.AppImage \
        --appdir $appimage_dir \
        --executable ./tmsexpress \
        --desktop-file ../dist/linux/TMS-Express.desktop \
        --icon-file ../dist/linux/tmsexpress.png \
        --plugin qt \
        --plugin gstreamer \
        --output appimage

    local release_archive="TMS-Express-Linux-x86_64.zip"
    zip -r $release_archive ./TMS_Express-x86_64.AppImage
}

function main() {
    echo "TMS Express Build Script"
    echo "------------------------"

    local os=$(get_os)
    local build_dir="./build"

    if [ $os == "unknown" ]; then
        echo "Cannot build TMS Express on OS: $os";
        return -1
    fi

    mkdir -p $build_dir

    if [ $os == "Darwin" ]; then
        build_macos
    elif [ $os == "Linux" ]; then
        build_linux
    else
        echo "Unsupported OS: $os"
        exit -1
    fi
}

main
