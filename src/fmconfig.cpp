#include "fmconfig.h"

#ifdef HAVE_KF6_CONFIG

#include <KSharedConfig>
#include <KConfigGroup>

namespace {

// Split "Group/Key" → ("Group", "Key").  No slash → ("", fullKey) for global group.
std::pair<QString, QString> splitKey(const QString &fullKey)
{
    const int slash = fullKey.indexOf(QLatin1Char('/'));
    if (slash == -1)
        return {QString{}, fullKey};
    return {fullKey.left(slash), fullKey.mid(slash + 1)};
}

} // namespace

QVariant FMConfig::value(const QString &fullKey, const QVariant &def)
{
    auto [grp, key] = splitKey(fullKey);
    return KSharedConfig::openConfig()->group(grp).readEntry(key, def);
}

void FMConfig::setValue(const QString &fullKey, const QVariant &val)
{
    auto [grp, key] = splitKey(fullKey);
    KSharedConfig::openConfig()->group(grp).writeEntry(key, val);
}

bool FMConfig::contains(const QString &fullKey)
{
    auto [grp, key] = splitKey(fullKey);
    return KSharedConfig::openConfig()->group(grp).hasKey(key);
}

void FMConfig::remove(const QString &fullKey)
{
    auto [grp, key] = splitKey(fullKey);
    KSharedConfig::openConfig()->group(grp).deleteEntry(key);
}

void FMConfig::sync()
{
    KSharedConfig::openConfig()->sync();
}

#else  // QSettings fallback (Windows / macOS)

// Use IniFormat + AppConfigLocation so the file lands in %APPDATA%\Fontmatrix\fontmatrix.ini
// on Windows (mirroring where KConfig would write on Windows) and in
// ~/Library/Application Support/Fontmatrix/fontmatrix.ini on macOS.
// NativeFormat is intentionally avoided: on Windows it writes to the registry, which is a
// completely different location from what KConfig uses, breaking any future migration.

#include <QSettings>
#include <QStandardPaths>
#include <QDir>

static QSettings &sharedSettings()
{
    static QSettings inst{QSettings::IniFormat, QSettings::UserScope,
                          QStringLiteral("Fontmatrix"),
                          QStringLiteral("fontmatrix")};
    return inst;
}

QVariant FMConfig::value(const QString &fullKey, const QVariant &def)
{
    return sharedSettings().value(fullKey, def);
}

void FMConfig::setValue(const QString &fullKey, const QVariant &val)
{
    sharedSettings().setValue(fullKey, val);
}

bool FMConfig::contains(const QString &fullKey)
{
    return sharedSettings().contains(fullKey);
}

void FMConfig::remove(const QString &fullKey)
{
    sharedSettings().remove(fullKey);
}

void FMConfig::sync()
{
    sharedSettings().sync();
}

#endif
