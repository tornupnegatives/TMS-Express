#!/usr/bin/env python3
"""Cross-platform build script for TMS Express.

This script can be executed locally or within a CI environment to build
and package TMS Express for macOS, Linux and Windows.
"""

from __future__ import annotations

import argparse
import platform
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path


@dataclass
class BuildConfig:
    """Configuration for a build invocation."""

    root: Path = Path(__file__).parent.resolve()
    build_dir: Path = root / "build"
    dist_dir: Path = root / "dist"
    build_type: str = "Release"


class BuildError(RuntimeError):
    """Raised when a build step fails."""


def run(cmd: str) -> None:
    """Run a shell command and raise if it fails."""

    print(f"[RUN] {cmd}")
    subprocess.check_call(cmd, shell=True)


class Builder:
    """Platform aware build helper."""

    def __init__(self, cfg: BuildConfig) -> None:
        self.cfg = cfg
        self.system = platform.system()

    # ------------------------------------------------------------------
    # Dependency Installation
    # ------------------------------------------------------------------
    def install_dependencies(self) -> None:
        if self.system == "Darwin":
            self._install_macos()
        elif self.system == "Linux":
            self._install_linux()
        elif self.system == "Windows":
            self._install_windows()
        else:
            raise BuildError(f"Unsupported system: {self.system}")

    def _install_macos(self) -> None:
        run("brew install cmake libsndfile pkg-config qt")
        # The multimedia module ships with qt on macOS

    def _install_linux(self) -> None:
        run("apt-get update")
        run(
            "apt-get install -y cmake libsndfile1-dev pkg-config wget "
            "qt6-base-dev qt6-multimedia-dev libgl1-mesa-dev"
        )

        if shutil.which("linuxdeployqt") is None:
            url = (
                "https://github.com/probonopd/linuxdeployqt/releases/download/"
                "continuous/linuxdeployqt-continuous-x86_64.AppImage"
            )
            run(f"wget -nv {url} -O linuxdeployqt.AppImage")
            run("chmod +x linuxdeployqt.AppImage")
            run("./linuxdeployqt.AppImage --appimage-extract")
            run(
                "mv squashfs-root/usr/bin/linuxdeployqt /usr/local/bin/linuxdeployqt"
            )
            shutil.rmtree("squashfs-root")
            Path("linuxdeployqt.AppImage").unlink()

    def _install_windows(self) -> None:
        run("choco install -y cmake")
        run("choco install -y libsndfile")
        run("choco install -y pkgconfiglite")
        run("choco install -y qt6")

    # ------------------------------------------------------------------
    # Build Steps
    # ------------------------------------------------------------------
    def build(self) -> None:
        cfg = self.cfg
        run(
            f"cmake -B {cfg.build_dir} -DCMAKE_BUILD_TYPE={cfg.build_type} "
            "-DTMSEXPRESS_BUILD_TESTS=OFF "
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
        )
        run(f"cmake --build {cfg.build_dir} --config {cfg.build_type}")

    # ------------------------------------------------------------------
    # Packaging
    # ------------------------------------------------------------------
    def package(self) -> Path:
        self.cfg.dist_dir.mkdir(parents=True, exist_ok=True)
        if self.system == "Darwin":
            return self._package_macos()
        if self.system == "Linux":
            return self._package_linux()
        if self.system == "Windows":
            return self._package_windows()
        raise BuildError(f"Unsupported system: {self.system}")

    def _package_macos(self) -> Path:
        """Bundle the build as a .app and zip it."""
        cfg = self.cfg
        app_dir = cfg.dist_dir / "TMSExpress.app" / "Contents" / "MacOS"
        app_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy(cfg.build_dir / "tmsexpress", app_dir)

        # Ensure the executable has an rpath that points to the location
        # where macdeployqt will place the Qt frameworks.  Without this,
        # macdeployqt fails to locate the required frameworks and prints
        # numerous "Cannot resolve rpath" errors.
        exe = app_dir / "tmsexpress"
        run(f"install_name_tool -add_rpath @executable_path/../Frameworks {exe}")
    
        run(f"macdeployqt {cfg.dist_dir/'TMSExpress.app'} -verbose=1")
        run(f"codesign --deep --force --sign - {cfg.dist_dir/'TMSExpress.app'}")

        archive = shutil.make_archive(str(cfg.dist_dir / "tmsexpress-macos"), "zip", cfg.dist_dir, "TMSExpress.app")
        return Path(archive)

    def _package_linux(self) -> Path:
        """Bundle the build as an AppImage."""
        cfg = self.cfg
        app_dir = cfg.dist_dir / "AppDir"
        app_dir.mkdir(parents=True, exist_ok=True)

        exe = app_dir / "tmsexpress"
        shutil.copy(cfg.build_dir / "tmsexpress", exe)

        desktop = app_dir / "tmsexpress.desktop"
        desktop.write_text(
            "[Desktop Entry]\n"
            "Type=Application\n"
            "Name=TMS Express\n"
            "Exec=tmsexpress\n"
            "Icon=tmsexpress\n"
            "Categories=Utility;\n"
        )

        icon_src = cfg.root / "doc" / "screenshot.png"
        icon_dst = app_dir / "tmsexpress.png"
        if icon_src.exists():
            shutil.copy(icon_src, icon_dst)

        run(
        run(
            f"linuxdeployqt {app_dir/'tmsexpress'} -appimage -verbose=1 "
            f"linuxdeployqt {desktop} -appimage -verbose=1 "
            f"-qmldir={cfg.root / 'src'}"
            f"-qmldir={cfg.root / 'src'}"
        )
        )

        appimage = cfg.dist_dir / "tmsexpress-x86_64.AppImage"
        return appimage

    def _package_windows(self) -> Path:
        """Bundle the build for Windows."""
        cfg = self.cfg
        dist = cfg.dist_dir / "tmsexpress"
        dist.mkdir(parents=True, exist_ok=True)
        exe = cfg.build_dir / cfg.build_type / "tmsexpress.exe"
        if not exe.exists():
            # msbuild layout
            exe = cfg.build_dir / "tmsexpress.exe"
        shutil.copy(exe, dist)
        run(f"windeployqt {dist / 'tmsexpress.exe'}")
        archive = shutil.make_archive(str(cfg.dist_dir / "tmsexpress-windows"), "zip", dist)
        return Path(archive)


# ----------------------------------------------------------------------
# CLI Entrypoint
# ----------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Build and package TMS Express"
    )
    parser.add_argument(
        "--skip-deps",
        action="store_true",
        help="Skip installing dependencies",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="Skip project compilation",
    )
    parser.add_argument(
        "--skip-package",
        action="store_true",
        help="Skip packaging step",
    )
    args = parser.parse_args()

    cfg = BuildConfig()
    builder = Builder(cfg)

    if not args.skip_deps:
        builder.install_dependencies()
    if not args.skip_build:
        builder.build()
    archive = None
    if not args.skip_package:
        archive = builder.package()
    if archive:
        print(f"Created artifact: {archive}")


if __name__ == "__main__":
    main()