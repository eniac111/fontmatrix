# Building and Installing Fontmatrix

## Pre-built packages

### Flatpak (Linux — easiest)

A Flatpak is published on [Flathub](https://flathub.org/apps/com.github.fontmatrix.Fontmatrix):

```bash
flatpak install flathub com.github.fontmatrix.Fontmatrix
flatpak run com.github.fontmatrix.Fontmatrix
```

### Windows

A pre-built MSI installer and portable ZIP are attached to each
[GitHub Release](https://github.com/eniac111/fontmatrix/releases).
Download and run the MSI — Qt and FreeType runtime DLLs are bundled.

---

## Building from source

### Requirements (all platforms)

| Dependency | Version | Notes |
|---|---|---|
| CMake | ≥ 3.16 | |
| C++ compiler | C++17 capable | GCC / Clang / MSVC 2022 |
| Qt 6 | 6.6+ (tested with 6.8) | see modules below |
| FreeType2 | ≥ 2.3.5 | |

Required Qt 6 modules: Core, Widgets, Svg, SvgWidgets, Sql, Xml,
PrintSupport, WebEngineWidgets, LinguistTools.

---

### Linux — Debian / Ubuntu

Tested on Debian 12 (Bookworm) and Ubuntu 22.04+.

```bash
sudo apt install \
  cmake ninja-build build-essential pkg-config \
  qt6-base-dev qt6-base-dev-tools \
  libqt6svg6-dev \
  qt6-webengine-dev \
  qt6-tools-dev qt6-l10n-tools \
  libfreetype-dev \
  libfontconfig1-dev
```

> **Note:** `qt6-webengine-dev` is a large package that pulls in Chromium.
> On Ubuntu 22.04 it is available in the universe repository;
> on Debian 12 it is in the main archive.

Build:

```bash
git clone https://github.com/eniac111/fontmatrix.git
cd fontmatrix
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build          # installs to /usr/local by default
```

---

### Linux — Flatpak (build locally)

```bash
flatpak install org.kde.Platform//6.8 org.kde.Sdk//6.8 \
                io.qt.qtwebengine.BaseApp//6.8
flatpak-builder --user --install --force-clean \
                build-flatpak com.github.fontmatrix.Fontmatrix.json
```

---

### Windows

**Prerequisites:**
- Visual Studio 2022 (Desktop C++ workload)
- [Qt 6.8](https://www.qt.io/download-qt-installer) — install the
  `MSVC 2022 64-bit` component and add the extra modules
  **Qt WebEngine**, **Qt Positioning**, **Qt WebChannel**
- [vcpkg](https://github.com/microsoft/vcpkg) (for FreeType)
- [.NET SDK](https://dotnet.microsoft.com/download) (for WiX installer)

```powershell
# FreeType
vcpkg install freetype:x64-windows

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release `
      "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake"

# Build
cmake --build build --config Release --parallel

# Install to ./dist (bundles Qt DLLs via windeployqt6 automatically)
cmake --install build --config Release --prefix dist
```

**MSI installer** (optional):

```powershell
dotnet tool install --global wix --version 6.0.2
wix extension add --global WixToolset.UI.wixext/6.0.2
cd build && cpack -G WIX -C Release
```

---

## Optional features

Pass these flags to the initial `cmake` command:

| Flag | Dependency | Description |
|---|---|---|
| `-DWANT_HARFBUZZ=true` | (bundled in `harfbuzz/`) | HarfBuzz text shaper |
| `-DWANT_ICU=true` | `libicu-dev` / ICU | ICU-based text shaping |
| `-DWANT_M17N=true` | `libm17n-dev` / M17N | M17N multilingual shaping |
| `-DWANT_PYTHONQT=true` | PythonQt + `libpython3-dev` | In-app Python scripting console |
| `-DWANT_PODOFO=true` | `libpodofo-dev` / PoDoFo | Extract embedded fonts from PDFs |

FontConfig is detected and enabled automatically on Linux when
`libfontconfig1-dev` is present; no flag is needed.

Example with ICU and PoDoFo on Debian/Ubuntu:

```bash
sudo apt install libicu-dev libpodofo-dev
cmake -B build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DWANT_ICU=true \
      -DWANT_PODOFO=true
cmake --build build --parallel
```

---

## Updating a translation

```bash
cmake -B build -DTRANSLATOR=<locale>   # e.g. fr, de, uk, zh_CN
cmake --build build --target translation
# Edit build/src/messages/fontmatrix-<locale>.ts with Qt Linguist
```

---

## Reporting issues

Open an issue on [GitHub](https://github.com/eniac111/fontmatrix/issues).
