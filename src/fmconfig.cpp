#include "fmconfig.h"

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
