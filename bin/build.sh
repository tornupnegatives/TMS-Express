#!/usr/bin/env bash

set -euo pipefail

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

    local build_dir="$(pwd)/build"
    local release_pkg="$build_dir/tmsexpress.app"
    local release_archive="$build_dir/TMS-Express-Darwin.zip"
    local qt_plugin_path
    local qt_path
    qt_path="$(brew --prefix qt)"
    qt_plugin_path="$("$qt_path/bin/qtpaths" --plugin-dir)"

    export CMAKE_PREFIX_PATH="$qt_path"

    cmake -S . -B "$build_dir" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -DTMSEXPRESS_BUILD_GUI=ON \
        -DTMSEXPRESS_BUILD_TESTS=OFF

    # CMake replaces the executable but does not remove frameworks and plug-ins
    # left by an earlier deployment. Those stale files can make macdeployqt fail.
    rm -rf "$release_pkg"
    cmake --build "$build_dir" --config Release --parallel

    if [[ ! -d "$release_pkg" ]]; then
        echo "Expected application bundle was not created: $release_pkg" >&2
        return 1
    fi

    # The default macdeployqt plug-in sweep includes unrelated Homebrew Qt
    # modules. Copy only the plug-ins this application needs, then include them
    # as deployment roots so their framework dependencies are bundled too.
    mkdir -p \
        "$release_pkg/Contents/PlugIns/platforms" \
        "$release_pkg/Contents/PlugIns/multimedia" \
        "$release_pkg/Contents/PlugIns/styles"
    cp "$qt_plugin_path/platforms/libqcocoa.dylib" \
        "$release_pkg/Contents/PlugIns/platforms/"
    cp "$qt_plugin_path/multimedia/libdarwinmediaplugin.dylib" \
        "$release_pkg/Contents/PlugIns/multimedia/"
    cp "$qt_plugin_path/styles/libqmacstyle.dylib" \
        "$release_pkg/Contents/PlugIns/styles/"

    local cocoa_plugin="$release_pkg/Contents/PlugIns/platforms/libqcocoa.dylib"
    local multimedia_plugin="$release_pkg/Contents/PlugIns/multimedia/libdarwinmediaplugin.dylib"
    local style_plugin="$release_pkg/Contents/PlugIns/styles/libqmacstyle.dylib"

    # Do not hide deployment errors: an incomplete bundle often still signs but
    # crashes when launched by Finder on a machine without Homebrew installed.
    local deploy_output
    if ! deploy_output="$(
        "$qt_path/bin/macdeployqt" "$release_pkg" \
            -always-overwrite \
            -no-codesign \
            -no-plugins \
            "-executable=$cocoa_plugin" \
            "-executable=$multimedia_plugin" \
            "-executable=$style_plugin" 2>&1
    )"; then
        printf '%s\n' "$deploy_output" >&2
        return 1
    fi
    printf '%s\n' "$deploy_output"

    # Some macdeployqt versions print deployment failures but still return 0.
    if grep -q '^ERROR:' <<<"$deploy_output"; then
        echo "macdeployqt reported an incomplete bundle" >&2
        return 1
    fi

    # Ad-hoc signing makes every copied framework and plug-in internally
    # consistent. A Developer ID identity can replace "-" for distribution.
    codesign --deep --force --sign - "$release_pkg"
    codesign --verify --deep --strict --verbose=2 "$release_pkg"

    # ditto preserves the symlinks and resource metadata used by frameworks.
    ditto -c -k --sequesterRsrc --keepParent \
        "$release_pkg" "$release_archive"

    echo "Created $release_pkg"
    echo "Created $release_archive"
}

build_linux() {
    export DEBIAN_FRONTEND=noninteractive
    apt update
    apt upgrade -y
    apt install -y --fix-missing \
        cmake pkg-config qmake6 \
        build-essential \
        g++ \
        git \
        zip \
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

    local os
    os="$(get_os)"
    local project_dir
    project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
    cd "$project_dir"

    local build_dir="./build"

    if [[ "$os" == unknown* ]]; then
        echo "Cannot build TMS Express on OS: $os";
        return 1
    fi

    mkdir -p $build_dir

    if [[ "$os" == "Darwin" ]]; then
        build_macos
    elif [[ "$os" == "Linux" ]]; then
        build_linux
    else
        echo "Unsupported OS: $os"
        exit 1
    fi
}

main
