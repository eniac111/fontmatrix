# SPDX-License-Identifier: GPL-2.0-or-later
#
# KDE Craft blueprint for PoDoFo (https://github.com/podofo/podofo).
#
# Why this is in the Fontmatrix repo and not upstream `craft-blueprints-kde`:
# Fontmatrix needs PoDoFo for its PDF font-extraction feature, but
# craft-blueprints-kde does not ship a PoDoFo blueprint as of master at
# the time of writing. We carry it in-tree alongside our fontmatrix
# blueprint; if the KDE ecosystem starts needing PoDoFo more broadly
# (Skanpage, Okular's PDF backends, etc.), this is a candidate for
# upstream contribution.
#
# Version pinning: the project's CLAUDE.md mandates PoDoFo >= 0.10.x.
# Local Linux builds use 0.10.4 (Manjaro). We pin to 0.10.5 — newest
# patch in the 0.10 series, ABI-compatible with 0.10.4. PoDoFo 1.0.x
# has API changes; we deliberately do NOT track it until Fontmatrix's
# PDF code is verified to compile against it.
#
# CMake options (from PoDoFo 0.10.5's CMakeLists.txt):
#   REQUIRED   ZLIB, OpenSSL, Freetype, LibXml2
#   OPTIONAL   Libidn, JPEG, TIFF, PNG, Fontconfig
#
# We keep the optional deps minimal: PNG (already a small dep), no
# JPEG/TIFF (not used by Fontmatrix's font-extraction code), no Libidn
# (niche), no Fontconfig (no Windows blueprint anyway).

import info
from CraftCore import CraftCore
from Package.CMakePackageBase import CMakePackageBase
from Utils import CraftHash


class subinfo(info.infoclass):
    def setTargets(self):
        self.targets["0.10.5"] = "https://github.com/podofo/podofo/archive/refs/tags/0.10.5.tar.gz"
        self.targetInstSrc["0.10.5"] = "podofo-0.10.5"
        self.targetDigests["0.10.5"] = (
            ["49b159e88ba177ad2561b5cf6cbd68ecbe83272f5488bc527e44f97dbf695273"],
            CraftHash.HashAlgorithm.SHA256,
        )
        self.defaultTarget = "0.10.5"
        self.description = "C++ PDF parsing/manipulation library"
        self.webpage = "https://github.com/podofo/podofo"

    def setDependencies(self):
        # Mandatory PoDoFo deps (from its CMakeLists `find_package(... REQUIRED)`)
        self.runtimeDependencies["virtual/base"] = None
        self.runtimeDependencies["libs/zlib"] = None
        self.runtimeDependencies["libs/openssl"] = None
        self.runtimeDependencies["libs/freetype"] = None
        self.runtimeDependencies["libs/libxml2"] = None
        # Optional but small — gives PNG embedding support, no harm.
        self.runtimeDependencies["libs/libpng"] = None


class Package(CMakePackageBase):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.subinfo.options.configure.args += [
            "-DPODOFO_BUILD_STATIC=FALSE",
            # Skip optional deps Fontmatrix doesn't need.
            "-DCMAKE_DISABLE_FIND_PACKAGE_Libidn=TRUE",
            "-DCMAKE_DISABLE_FIND_PACKAGE_JPEG=TRUE",
            "-DCMAKE_DISABLE_FIND_PACKAGE_TIFF=TRUE",
            "-DCMAKE_DISABLE_FIND_PACKAGE_Fontconfig=TRUE",
            # PoDoFo's tests need a writable test-data directory and
            # are not useful in CI; skip them entirely.
            "-DPODOFO_BUILD_TEST=FALSE",
            "-DPODOFO_BUILD_EXAMPLES=FALSE",
        ]
