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
from Package.CMakePackageBase import CMakePackageBase


class subinfo(info.infoclass):
    def setTargets(self):
        self.displayName = "Fontmatrix"
        self.description = "Cross-platform font management application built on Qt6 / KF6"
        self.webpage = "https://github.com/eniac111/fontmatrix"

        # We don't actually fetch source via Craft — CI invokes us with
        #   --options fontmatrix.srcDir=<checkout>
        # which overrides the source step entirely. But Craft still needs
        # *some* target declaration so it has a label to attach to build
        # artifacts and package filenames; the value of that label ends
        # up in the NSIS output as `fontmatrix-<target>-windows-cl-msvc2022-x86_64.exe`.
        #
        # CRAFT_FONTMATRIX_VERSION_FULL is exported by the GitHub Actions
        # workflow ("Determine version" step) and is either:
        #   - "1.2.3"          for tag builds (refs/tags/vX.Y.Z)
        #   - "git-<short-sha>" for untagged push/PR builds
        # For local invocations without that env var, we fall back to
        # "master" — same string a stock Craft build off the GitHub repo
        # would have used.
        #
        # The URL on svnTargets is never followed (srcDir wins), but
        # Craft expects a non-empty value, so we point it at the canonical
        # repo for documentation.
        version = os.environ.get("CRAFT_FONTMATRIX_VERSION_FULL", "master")
        self.svnTargets[version] = "https://github.com/eniac111/fontmatrix.git"
        self.defaultTarget = version

    def setDependencies(self):
        # Build-only: ECM (KDE's CMake helpers) is a configure-time dep.
        # Note: extra-cmake-modules is a peer of tier1/tier2/tier3 in
        # craft-blueprints-kde, not nested under any tier.
        self.buildDependencies["kde/frameworks/extra-cmake-modules"] = None

        # Qt 6 — application links against Core, Widgets, Svg, SvgWidgets,
        # Sql, Xml, PrintSupport, WebEngineWidgets. Craft's qtbase blueprint
        # pulls Core/Widgets/Sql/Xml/PrintSupport; svg+webengine are separate.
        self.runtimeDependencies["libs/qt/qtbase"] = None
        self.runtimeDependencies["libs/qt/qtsvg"] = None
        self.runtimeDependencies["libs/qt/qttools"] = None  # for lupdate/lrelease at build time
        self.runtimeDependencies["libs/qt/qtwebengine"] = None

        # KF 6 — same component set as src/CMakeLists.txt's find_package(KF6 ...).
        self.runtimeDependencies["kde/frameworks/tier1/kcoreaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/ki18n"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kconfig"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kdbusaddons"] = None
        self.runtimeDependencies["kde/frameworks/tier1/kwidgetsaddons"] = None
        # kstatusnotifieritem lives in tier2, not tier3, in craft-blueprints-kde.
        self.runtimeDependencies["kde/frameworks/tier2/kstatusnotifieritem"] = None
        self.runtimeDependencies["kde/frameworks/tier3/kxmlgui"] = None
        self.runtimeDependencies["kde/frameworks/tier3/kconfigwidgets"] = None
        # KDocTools is optional: handbook builds when present, otherwise skipped.
        # Leave it out of the dependency set on Windows to keep the runtime
        # smaller; the handbook is consumed by KHelpCenter, which has no
        # equivalent on Windows anyway.

        # Native deps — FreeType is mandatory; PoDoFo is required for PDF
        # font extraction. Both have Craft blueprints under libs/.
        self.runtimeDependencies["libs/freetype"] = None
        self.runtimeDependencies["libs/podofo"] = None


class Package(CMakePackageBase):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # Forward Fontmatrix's optional CMake feature flags. WANT_HARFBUZZ
        # is on by default because the source tree vendors HarfBuzz under
        # harfbuzz/ — Craft does not need to provide it. WANT_FONTCONFIG
        # is off on Windows: there is no system FontConfig.
        cmake_args = [
            "-DWANT_HARFBUZZ=true",
            "-DWANT_FONTCONFIG=false",
            "-DCMAKE_BUILD_TYPE=Release",
        ]

        # Forward an optional version triple from the environment. CI sets
        # CRAFT_FONTMATRIX_VERSION_{MAJOR,MINOR,PATCH} for tag builds; for
        # untagged builds the env vars are unset and CMakeLists falls back
        # to its in-tree defaults.
        for component in ("MAJOR", "MINOR", "PATCH"):
            value = os.environ.get(f"CRAFT_FONTMATRIX_VERSION_{component}")
            if value:
                cmake_args.append(f"-DVERSION_{component}={value}")

        self.subinfo.options.configure.args = " ".join(cmake_args)

    def createPackage(self):
        self.defines["appname"] = "fontmatrix"
        self.defines["company"] = "Fontmatrix"
        # NSIS template's `Caption "@{productname} @{version}"` and
        # registry `DisplayVersion` field both reference @{version}. Craft
        # only auto-populates this when the blueprint has a concrete
        # target/version; otherwise generateNSISInstaller() throws
        #   Failed to configure NullsoftInstaller.nsi: @{version} is not in variables
        # We declare svnTargets[CRAFT_FONTMATRIX_VERSION_FULL] in setTargets,
        # but set this explicitly too so the install never fails on it.
        self.defines["version"] = os.environ.get("CRAFT_FONTMATRIX_VERSION_FULL", "0.0.0")
        self.defines["shortcuts"] = [
            {"name": "Fontmatrix", "target": "bin/fontmatrix.exe"},
        ]
        self.ignoredPackages.append("binary/mysql")  # not needed for SQLite-only Sql usage
        return super().createPackage()
