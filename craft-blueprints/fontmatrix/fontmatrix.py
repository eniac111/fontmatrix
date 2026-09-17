# SPDX-License-Identifier: GPL-2.0-or-later
#
# KDE Craft blueprint for Fontmatrix.
#
# This is consumed on Windows (and optionally macOS) by KDE Craft to build
# Fontmatrix and produce installer artifacts. It is NOT used on Linux,
# where the system package manager + Flatpak handle dependencies.
#
# Blueprint deployment in CI: copied at install time to
#   <CraftRoot>/etc/blueprints/locations/fontmatrix/fontmatrix/fontmatrix.py
# See .github/workflows/build.yml for the exact wiring, and
# BUILDING-WINDOWS.md for local-developer setup.
#
# Pattern derived from the upstream Kirigami tutorial blueprint:
#   https://develop.kde.org/docs/getting-started/building/craft/

import os

import info
from CraftCore import CraftCore
from Package.CMakePackageBase import CMakePackageBase


class subinfo(info.infoclass):
    def setTargets(self):
        self.displayName = "Fontmatrix"
        self.description = "Cross-platform font management application built on Qt6 / KF6"
        self.webpage = "https://github.com/eniac111/fontmatrix"

        # CI passes --options fontmatrix.srcDir=<checkout>, so the URL is
        # never fetched — but Craft requires svnTargets to have at least
        # one entry. CRAFT_FONTMATRIX_VERSION_FULL is set by the workflow.
        version = os.environ.get("CRAFT_FONTMATRIX_VERSION_FULL", "master")
        self.svnTargets[version] = "https://github.com/eniac111/fontmatrix.git"
        self.defaultTarget = version

    def setDependencies(self):
        # virtual/base is what drags in libs/runtime — the package that
        # installs the MSVC redistributable DLLs (vcruntime140.dll,
        # msvcp140.dll, …) into bin/. Without it the packaged installer has
        # no CRT and the app only starts on machines that happen to have the
        # VC++ redist installed, which is why this has to be declared even
        # though it looks like build-only plumbing. Every upstream KDE
        # blueprint (kate, filelight, kcalc, …) declares it.
        self.runtimeDependencies["virtual/base"] = None
        self.buildDependencies["kde/frameworks/extra-cmake-modules"] = None

        # qttools / qtdeclarative are NOT runtime deps. qtdeclarative pulls
        # libs/llvm (~1 GB of clang tools); we don't ship Linguist/Designer
        # and translations flow through ki18n_install(po), not lrelease.
        self.runtimeDependencies["libs/qt/qtbase"] = None
        self.runtimeDependencies["libs/qt/qtsvg"] = None

        self.runtimeDependencies["kde/frameworks/tier1/kcoreaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/ki18n"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kconfig"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kdbusaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kwidgetsaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier2/kstatusnotifieritem"] = None
        self.runtimeDependencies["kde/frameworks/tier3/kxmlgui"] = None
        self.runtimeDependencies["kde/frameworks/tier3/kconfigwidgets"] = None

        # The UI loads its icons through QIcon::fromTheme(), so the theme and
        # the loader that reads it have to be in the package. Both arrive
        # transitively via kxmlgui → kiconthemes → breeze-icons, but they are
        # named here so a future dependency trim can't silently leave the app
        # with blank toolbars.
        self.runtimeDependencies["kde/frameworks/tier3/kiconthemes"] = None
        self.runtimeDependencies["kde/frameworks/tier1/breeze-icons"] = None

        self.runtimeDependencies["libs/freetype"] = None
        self.runtimeDependencies["libs/podofo"] = None


class Package(CMakePackageBase):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # No -DCMAKE_BUILD_TYPE here: Craft appends its own
        # -DCMAKE_BUILD_TYPE=<buildType()> after the blueprint's args, so
        # ours was being silently overridden anyway. The build type comes from
        # Craft's own setting (default RelWithDebInfo, which is what KDE keeps
        # a populated binary cache for).
        cmake_args = [
            "-DWANT_HARFBUZZ=true",
            "-DWANT_FONTCONFIG=false",
        ]
        for component in ("MAJOR", "MINOR", "PATCH"):
            value = os.environ.get(f"CRAFT_FONTMATRIX_VERSION_{component}")
            if value:
                cmake_args.append(f"-DVERSION_{component}={value}")
        self.subinfo.options.configure.args = " ".join(cmake_args)

        # Layer a custom blacklist on top of Craft's default
        # applications_blacklist.txt. Skip silently if the file isn't
        # alongside the blueprint (Craft's toRegExp() raises on missing
        # paths, which would abort packaging).
        blueprint_dir = os.path.dirname(os.path.abspath(__file__))
        blacklist_path = os.path.join(blueprint_dir, "blacklist.txt")
        if os.path.isfile(blacklist_path):
            self.blacklist_file.append(blacklist_path)

    def formatVersion(self, includeRevision, includeTimeStamp):
        # Default uses [git rev-parse --abbrev-ref HEAD, self.version],
        # which on a detached CI checkout becomes "HEAD-<version>". Collapse
        # it to a single label when the workflow exports a version.
        env_version = os.environ.get("CRAFT_FONTMATRIX_VERSION_FULL")
        if env_version:
            return env_version
        return super().formatVersion(includeRevision, includeTimeStamp)

    def createPackage(self):
        self.defines["appname"] = "fontmatrix"
        self.defines["company"] = "Fontmatrix"
        # NSIS template references @{version}; Craft only auto-fills it
        # when a concrete svnTarget version is set, so set it defensively.
        self.defines["version"] = os.environ.get("CRAFT_FONTMATRIX_VERSION_FULL", "0.0.0")
        self.defines["shortcuts"] = [
            {"name": "Fontmatrix", "target": "bin/fontmatrix.exe"},
        ]

        # Without these the NSIS packager falls back to Craft's own defaults,
        # which means the installer, the uninstaller and the Add/Remove
        # Programs entry all show the Craft mascot and no licence page.
        # Guarded: a packaging run must not die over an installer icon.
        try:
            src = self.sourceDir()
        except Exception as exc:  # noqa: BLE001 - cosmetic defines only
            CraftCore.log.warning(f"fontmatrix: no source dir for installer assets: {exc}")
            src = None
        if src:
            icon = src / "src/icons/fontmatrix.ico"
            if icon.exists():
                self.defines["icon"] = icon
            licence = src / "COPYING"
            if licence.exists():
                self.defines["license"] = licence

        # Drop random executables that came along with build deps but aren't
        # ours (e.g. qmldom.exe, androiddeployqt.exe, openssl.exe). Same
        # pattern as kate.py.
        self.addExecutableFilter(r"(bin|libexec)/(?!fontmatrix\.exe).*")

        self.ignoredPackages.append("binary/mysql")
        # KF6 kconfig/ki18n/kguiaddons declare qtdeclarative as a runtime
        # dep for their QML bindings (KConfig.QML, KLocalizedContext, …).
        # Fontmatrix is a pure QtWidgets app — strip the QML chain at
        # packaging time so the installer doesn't ship Qt6Quick/Qml/etc.
        #
        # Consequence to keep in mind: the KF6 *Qml wrapper libraries live in
        # their parent framework's image dir, so they are still collected and
        # would ship importing a Qt6Qml.dll that isn't there. blacklist.txt
        # drops them; check-windows-deps.py is what catches it if a new one
        # appears.
        self.ignoredPackages.extend([
            "libs/qt6/qtdeclarative",
            "libs/qt6/qtshadertools",
            "libs/qt6/qtlanguageserver",
            "libs/qt6/qttools",
            "libs/qt6/qtquick3d",
            "libs/llvm",
        ])
        # D-Bus is Linux-only for our use case; KDBusAddons gracefully no-ops
        # when D-Bus isn't installed on Windows. Mirrors kate.py.
        if not CraftCore.compiler.isLinux:
            self.ignoredPackages.append("libs/dbus")

        return super().createPackage()
