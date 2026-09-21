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
| C++ compiler | C++20 | GCC / Clang / MSVC 2022 |
| Qt 6 | ≥ 6.8 | Core, Widgets, Svg, SvgWidgets, Sql, Xml, PrintSupport |
| KDE Frameworks 6 and extra-cmake-modules | ≥ 6.12 | CoreAddons, I18n, Config, XmlGui, ConfigWidgets, WidgetsAddons, StatusNotifierItem, DBusAddons; DocTools is optional (handbook) |
| PoDoFo | ≥ 0.10 | optional: *Tools → Extract fonts* (the fonts embedded in a PDF file). Without it, or with 0.9, whose API is another one, the application is built without that entry |
| HarfBuzz | ≥ 2.6.8 | built with FreeType support; Qt 6 depends on it already |
| FreeType2 | ≥ 2.3.5 | |
| Gettext | | `msgfmt`, to compile the translations |
| Fontconfig | | optional, Linux; without it fonts are managed but not switched on and off for other programs |

The Qt and KDE Frameworks versions are the oldest ones the project is built and run
against: Debian 13 (Qt 6.8.2, KF 6.13) and Ubuntu 25.04 (Qt 6.8.3, KF 6.12). CI covers the
`org.kde.Platform` 6.10 Flatpak runtime, Arch Linux and KDE Craft, all newer. Older versions
may work; nobody has tried, and CMake refuses them.

---

### Linux

On a distribution that has the versions above (Arch Linux is what CI uses):

```bash
sudo pacman -S --needed base-devel cmake ninja extra-cmake-modules gettext \
  qt6-base qt6-svg kcoreaddons ki18n kconfig kxmlgui kconfigwidgets kwidgetsaddons \
  kstatusnotifieritem kdbusaddons kiconthemes freetype2 harfbuzz podofo fontconfig
```

On Debian 13 and Ubuntu 25.04:

```bash
sudo apt install g++ cmake ninja-build pkg-config gettext extra-cmake-modules \
  qt6-base-dev qt6-svg-dev libkf6coreaddons-dev libkf6i18n-dev libkf6config-dev \
  libkf6xmlgui-dev libkf6configwidgets-dev libkf6widgetsaddons-dev \
  libkf6statusnotifieritem-dev libkf6dbusaddons-dev libkf6iconthemes-dev \
  libfreetype-dev libharfbuzz-dev libfontconfig-dev
```

Both ship PoDoFo 0.9.8, which is too old: Fontmatrix is built there without *Tools → Extract
fonts* and says so when CMake runs. The Flatpak has it.

Build:

```bash
git clone https://github.com/eniac111/fontmatrix.git
cd fontmatrix
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build          # installs to /usr/local by default
```

Configuring also installs a git pre-commit hook that checks the changed lines against
the KDE coding style; `ninja -C build clang-format` formats everything.

---

### Linux — Flatpak (build locally)

```bash
flatpak install org.kde.Platform//6.10 org.kde.Sdk//6.10
flatpak-builder --user --install --force-clean \
                build-flatpak com.github.fontmatrix.Fontmatrix.json
```

---

### Windows

Windows builds are made with [KDE Craft](https://community.kde.org/Craft), which provides
Qt, KDE Frameworks, HarfBuzz and the MSVC runtime from KDE's binary cache. The recipe is in
`craft-blueprints/` (Fontmatrix, plus PoDoFo, which Craft does not have), the Craft settings
in `.github/craft/CraftConfig.ini`, and the exact steps in the `windows` job ("Build Windows") of
`.github/workflows/build.yml`.

---

## Optional features

Pass these flags to the initial `cmake` command:

| Flag | Dependency | Description |
|---|---|---|
| `-DWANT_ICU=true` | `libicu-dev` / ICU | ICU-based text shaping |
| `-DWANT_M17N=true` | `libm17n-dev` / M17N | M17N multilingual shaping |
| `-DWANT_PYTHONQT=true` | PythonQt + `libpython3-dev` | In-app Python scripting console |

Fontconfig is detected automatically on Linux; `-DCMAKE_DISABLE_FIND_PACKAGE_Fontconfig=ON`
builds without it. `-DFONTMATRIX_WERROR=ON` turns warnings in the project's own sources into
errors, as CI does.

Example with ICU:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DWANT_ICU=true
cmake --build build --parallel
```
