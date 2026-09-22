/*
    SPDX-FileCopyrightText: 2008 Pierre Marchand <pierremarc@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "fmactivate.h"
#include "fontmatrix_debug.h"

#include "fmfontdb.h"
#include "fontitem.h"
#include "typotek.h"

#include <KLocalizedString>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>

FMActivate *FMActivate::instance = nullptr;

FMActivate::FMActivate()
{
    setErrorStrings();
}

void FMActivate::setErrorStrings()
{
    //: Activation subroutine failed to make a symbolic link to the font file
    errorStrings[NO_LINK] = i18nc("@info activation error", "Unable to link");
    //: The Font asked for activation is already activated
    errorStrings[ALREADY_ACTIVE] = i18nc("@info activation error", "Font already activated");
    //: Activation subroutine failed to remove a symbolic link to the font file
    errorStrings[NO_UNLINK] = i18nc("@info activation error", "Unable to un-link");
    //: The Font asked for de-activation is already de-activated
    errorStrings[ALREADY_UNACTIVE] = i18nc("@info activation error", "Font already de-activated");
    //: A postcript font (pfb) without its metrics file (afm)
    errorStrings[MISSING_AFM] = i18nc("@info activation error", "Cannot link or copy the metrics file");
    //: A generic error in activation or deactivation process
    errorStrings[OTHER_ERROR] = i18nc("activation", "Error");
    //: Windows: the font file could not be copied into the user's font folder
    errorStrings[NO_COPY] = i18nc("@info activation error", "Unable to copy the font file into the user font folder");
    //: Windows: the font could not be written to or removed from the user's font registry key
    errorStrings[NO_REGISTRY] = i18nc("@info activation error", "Unable to register the font for this user");
    //: Windows: AddFontResource failed, applications will see the font at the next logon only
    errorStrings[NO_FONT_RESOURCE] = i18nc("@info activation error", "The font is installed but could not be loaded for running applications");
    //: Windows: the font is installed for all users or by the user in Settings, Fontmatrix cannot change it
    errorStrings[LOCKED_FONT] = i18nc("@info activation error", "The font is installed by the system and cannot be changed here");
    //: Windows: only TrueType and OpenType fonts can be installed per user
    errorStrings[UNSUPPORTED_FORMAT] = i18nc("@info activation error", "Only TrueType and OpenType fonts can be activated for this user");
}

FMActivate *FMActivate::getInstance()
{
    if (!instance) {
        instance = new FMActivate;
        Q_ASSERT(instance);
    }
    return instance;
}

#ifdef PLATFORM_APPLE

void FMActivate::activate(QList<FontItem *> fitList, bool act)
{
    // TODO insert error messages.
    QHash<FontItem *, bool> stack;
    typotek *T(typotek::getInstance());
    for (auto *fit : fitList) {
        qCDebug(FONTMATRIX_LOG) << "Activation of " << fit->path() << act;
        if (act) // Activation
        {
            if (!T->isSysFont(fit)) {
                if (!fit->isActivated()) {
                    fit->setActivated(true);

                    // 				QFileInfo fofi ( fit->path() );

                    if (!QFile::copy(fit->localPath(), T->getManagedDir() + "/" + fit->activationName())) {
                        qCWarning(FONTMATRIX_LOG) << "unable to copy " << fit->path();
                    } else {
                        // Success
                        stack[fit] = true;
                        qCDebug(FONTMATRIX_LOG) << fit->path() << " copied";
                        if (!fit->afm().isEmpty()) {
                            // 						QFileInfo afm ( fit->afm() );
                            if (!QFile::copy(fit->afm(), T->getManagedDir() + "/" + fit->activationAFMName())) {
                                qCWarning(FONTMATRIX_LOG) << "unable to copy " << fit->afm();
                            } else {
                                qCDebug(FONTMATRIX_LOG) << fit->afm() << "copied";
                            }
                        } else {
                            qCDebug(FONTMATRIX_LOG) << "There is no AFM file attached to " << fit->path();
                        }
                    }
                } else {
                    qCDebug(FONTMATRIX_LOG) << "\tYet activated";
                }

            } else {
                qCDebug(FONTMATRIX_LOG) << "\tIs Locked";
            }

        } else // Deactivation
        {
            if (!T->isSysFont(fit)) {
                if (fit->isActivated()) {
                    fit->setActivated(false);
                    // 				QFileInfo fofi ( fit->path() );
                    if (!QFile::remove(T->getManagedDir() + "/" + fit->activationName())) {
                        qCWarning(FONTMATRIX_LOG) << "unable to unlink " << fit->name();
                    } else {
                        // Success
                        stack[fit] = false;
                        if (!fit->afm().isEmpty()) {
                            // 						QFileInfo afm ( fit->afm() );
                            if (!QFile::remove(T->getManagedDir() + "/" + fit->activationAFMName())) {
                                qCWarning(FONTMATRIX_LOG) << "unable to unlink " << fit->afm();
                            }
                        }
                        // 					typo->adaptator()->private_signal ( 0, fofi.fileName() );
                    }
                }

            } else {
                qCDebug(FONTMATRIX_LOG) << "\tIs Locked";
            }
        }
    }

    QStringList aList;
    FMFontDb::DB()->TransactionBegin();
    for (auto it(stack.constBegin()); it != stack.constEnd(); ++it) {
        it.key()->setActivated(it.value());
        aList << it.key()->path();
    }
    FMFontDb::DB()->TransactionEnd();

    Q_EMIT activationEvent(aList);
}

#elif defined(_WIN32)

/*
    A font is activated for the current user the way Settings > Fonts installs
    one: a copy in %LOCALAPPDATA%\Microsoft\Windows\Fonts (managedDir), a REG_SZ
    value under HKCU\Software\Microsoft\Windows NT\CurrentVersion\Fonts whose
    name is the full font name plus "(TrueType)" or "(OpenType)" and whose data
    is the path of the copy, which Windows loads at every logon, and
    AddFontResource() plus WM_FONTCHANGE so that running applications see it now.
    Deactivation takes the three away in the opposite order. Nothing needs
    administrator rights.
*/

// clang-format off: windows.h has to come first
#define NOMINMAX
#include <windows.h>
// clang-format on

#include "fmfreetypelib.h"

#include <QByteArrayView>
#include <QDir>
#include <QFileInfo>
#include <QMultiHash>
#include <QStringDecoder>
#include <QThread>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

#include <string>
#include <vector>

namespace
{
constexpr wchar_t userFontsKeyPath[] = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";

/// the path as the Win32 API and the registry take it
std::wstring nativePath(const QString &path)
{
    return QDir::toNativeSeparators(QDir::cleanPath(path)).toStdWString();
}

/// paths are compared without their case and with either separator
bool samePath(const QString &a, const QString &b)
{
    return QDir::cleanPath(QDir::fromNativeSeparators(a)).compare(QDir::cleanPath(QDir::fromNativeSeparators(b)), Qt::CaseInsensitive) == 0;
}

bool isInside(const QString &path, const QString &dir)
{
    const QString p(QDir::cleanPath(QDir::fromNativeSeparators(path)));
    const QString d(QDir::cleanPath(QDir::fromNativeSeparators(dir)) + QLatin1Char('/'));
    return p.startsWith(d, Qt::CaseInsensitive);
}

/**
 * The full name (name ID 4) of every face of the file, the English one when
 * there is one. The file is read into memory: FT_New_Face() wants an 8-bit
 * path, which cannot name every folder on Windows.
 */
QStringList fullNames(const QString &path)
{
    QStringList names;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return names;
    const QByteArray data(f.readAll());
    FT_Library lib = FMFreetypeLib::lib(QThread::currentThread());
    FT_Long faceCount = 1;
    for (FT_Long i = 0; i < faceCount; ++i) {
        FT_Face face = nullptr;
        if (FT_New_Memory_Face(lib, reinterpret_cast<const FT_Byte *>(data.constData()), static_cast<FT_Long>(data.size()), i, &face))
            break;
        faceCount = face->num_faces;
        QString english;
        QString other;
        const FT_UInt count = FT_Get_Sfnt_Name_Count(face);
        for (FT_UInt n = 0; n < count && english.isEmpty(); ++n) {
            FT_SfntName sn;
            if (FT_Get_Sfnt_Name(face, n, &sn) != 0 || sn.name_id != TT_NAME_ID_FULL_NAME || sn.string_len == 0)
                continue;
            const QByteArrayView bytes(reinterpret_cast<const char *>(sn.string), sn.string_len);
            QString value;
            if (sn.platform_id == TT_PLATFORM_MICROSOFT) {
                // every Microsoft name record is UTF-16BE
                value = QString(QStringDecoder(QStringDecoder::Utf16BE)(bytes)).trimmed();
            } else if (sn.platform_id == TT_PLATFORM_MACINTOSH && sn.encoding_id == TT_MAC_ID_ROMAN) {
                value = QString::fromLatin1(bytes).trimmed();
            }
            if (value.isEmpty())
                continue;
            if (sn.platform_id == TT_PLATFORM_MICROSOFT && sn.language_id == TT_MS_LANGID_ENGLISH_UNITED_STATES)
                english = value;
            else if (other.isEmpty())
                other = value;
        }
        const QString name(english.isEmpty() ? other : english);
        if (!name.isEmpty() && !names.contains(name))
            names << name;
        FT_Done_Face(face);
    }
    return names;
}

/**
 * The name of the registry value, as Settings > Fonts writes it: the full font
 * name, the names of a collection joined by " & ", and the format in
 * parentheses. Empty for a format Windows does not install per user.
 */
QString registryValueName(FontItem *fit)
{
    QString suffix;
    if (fit->type() == QLatin1String("TrueType"))
        suffix = QStringLiteral(" (TrueType)");
    else if (fit->type() == QLatin1String("CFF"))
        suffix = QStringLiteral(" (OpenType)");
    else
        return QString();
    QStringList names(fullNames(fit->localPath()));
    if (names.isEmpty())
        names << fit->fancyName().trimmed();
    return names.join(QLatin1String(" & ")) + suffix;
}

/// The per-user font key, opened for the lifetime of the object.
class UserFontsKey
{
public:
    explicit UserFontsKey(bool write)
    {
        const REGSAM access = KEY_QUERY_VALUE | (write ? KEY_SET_VALUE : 0);
        LSTATUS status;
        if (write) {
            // the key does not exist until the first per-user font is installed
            status = RegCreateKeyExW(HKEY_CURRENT_USER, userFontsKeyPath, 0, nullptr, REG_OPTION_NON_VOLATILE, access, nullptr, &m_key, nullptr);
        } else {
            status = RegOpenKeyExW(HKEY_CURRENT_USER, userFontsKeyPath, 0, access, &m_key);
        }
        if (status != ERROR_SUCCESS) {
            qCWarning(FONTMATRIX_LOG) << "Cannot open the user fonts registry key, error" << status;
            m_key = nullptr;
        }
    }
    ~UserFontsKey()
    {
        if (m_key)
            RegCloseKey(m_key);
    }
    Q_DISABLE_COPY(UserFontsKey)

    [[nodiscard]] bool isOpen() const
    {
        return m_key != nullptr;
    }

    /// value name -> font path, for every string value of the key
    [[nodiscard]] QMap<QString, QString> values() const
    {
        QMap<QString, QString> ret;
        if (!m_key)
            return ret;
        DWORD maxNameChars = 0;
        DWORD maxDataBytes = 0;
        if (RegQueryInfoKeyW(m_key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &maxNameChars, &maxDataBytes, nullptr, nullptr)
            != ERROR_SUCCESS)
            return ret;
        std::vector<wchar_t> name(maxNameChars + 1);
        std::vector<BYTE> data(maxDataBytes + sizeof(wchar_t));
        for (DWORD i = 0;; ++i) {
            DWORD nameChars = static_cast<DWORD>(name.size());
            DWORD dataBytes = static_cast<DWORD>(data.size());
            DWORD type = 0;
            const LSTATUS status = RegEnumValueW(m_key, i, name.data(), &nameChars, nullptr, &type, data.data(), &dataBytes);
            if (status == ERROR_NO_MORE_ITEMS)
                break;
            if (status != ERROR_SUCCESS || type != REG_SZ)
                continue;
            QString path(QString::fromWCharArray(reinterpret_cast<const wchar_t *>(data.data()), static_cast<int>(dataBytes / sizeof(wchar_t))));
            while (path.endsWith(QChar(0)))
                path.chop(1);
            ret.insert(QString::fromWCharArray(name.data(), static_cast<int>(nameChars)), path);
        }
        return ret;
    }

    bool set(const QString &name, const QString &path)
    {
        const std::wstring n(name.toStdWString());
        const std::wstring d(nativePath(path));
        const LSTATUS status =
            RegSetValueExW(m_key, n.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE *>(d.c_str()), static_cast<DWORD>((d.size() + 1) * sizeof(wchar_t)));
        if (status != ERROR_SUCCESS)
            qCWarning(FONTMATRIX_LOG) << "Cannot write the registry value" << name << ", error" << status;
        return status == ERROR_SUCCESS;
    }

    bool remove(const QString &name)
    {
        const LSTATUS status = RegDeleteValueW(m_key, name.toStdWString().c_str());
        if (status != ERROR_SUCCESS)
            qCWarning(FONTMATRIX_LOG) << "Cannot delete the registry value" << name << ", error" << status;
        return status == ERROR_SUCCESS;
    }

private:
    HKEY m_key = nullptr;
};

/**
 * Writes the value that makes Windows load the copy at logon. When another
 * font already holds the name, the copy's file name is added to it, so that
 * the other font keeps its value.
 */
bool registerUserFont(const QString &path, const QString &valueName)
{
    UserFontsKey key(true);
    if (!key.isOpen())
        return false;
    QString name(valueName);
    const QMap<QString, QString> values(key.values());
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        if (it.key().compare(valueName, Qt::CaseInsensitive) == 0 && !samePath(it.value(), path)) {
            name.insert(name.lastIndexOf(QLatin1String(" (")), QStringLiteral(" (%1)").arg(QFileInfo(path).fileName()));
            break;
        }
    }
    return key.set(name, path);
}

/// Deletes every value that points to the copy. Nothing to delete is a success.
bool unregisterUserFont(const QString &path)
{
    UserFontsKey key(true);
    if (!key.isOpen())
        return false;
    bool ok = true;
    const QMap<QString, QString> values(key.values());
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        if (samePath(it.value(), path))
            ok = key.remove(it.key()) && ok;
    }
    return ok;
}

/// Tells every window that the font table changed. Notify, not send: a hung
/// application must not hang Fontmatrix.
void broadcastFontChange()
{
    SendNotifyMessageW(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
}
} // namespace

void FMActivate::activate(QList<FontItem *> fitList, bool act)
{
    QHash<FontItem *, bool> stack;
    typotek *T(typotek::getInstance());
    const QString managed(T->getManagedDir());
    bool fontTableChanged = false;
    for (auto *fit : fitList) {
        if (act) // Activation
        {
            if (fit->isRemote() && !fit->isCached()) {
                // the file comes first; this font is activated on its own when it is here
                connect(
                    fit,
                    &FontItem::downloadFinished,
                    this,
                    [this, fit](bool ok) {
                        if (ok)
                            activate(QList<FontItem *>() << fit, true);
                    },
                    Qt::SingleShotConnection);
                fit->getFromNetwork();
                continue;
            }
            if (T->isSysFont(fit) || isInside(fit->localPath(), managed)) {
                // installed for everybody, or by the user in Settings > Fonts
                m_errors[fit->path()] = errorStrings[LOCKED_FONT];
                continue;
            }
            if (fit->isActivated()) {
                m_errors[fit->path()] = errorStrings[ALREADY_ACTIVE];
                continue;
            }
            const QString valueName(registryValueName(fit));
            if (valueName.isEmpty()) {
                m_errors[fit->path()] = errorStrings[UNSUPPORTED_FORMAT];
                continue;
            }
            const QString copy(managed + QLatin1Char('/') + fit->activationName());
            // a copy may be left from a deactivation while the font was in use
            if (!QFileInfo::exists(copy) && !QFile::copy(fit->localPath(), copy)) {
                qCWarning(FONTMATRIX_LOG) << "unable to copy" << fit->localPath() << "to" << copy;
                m_errors[fit->path()] = errorStrings[NO_COPY];
                continue;
            }
            if (!registerUserFont(copy, valueName)) {
                m_errors[fit->path()] = errorStrings[NO_REGISTRY];
                QFile::remove(copy);
                continue;
            }
            stack[fit] = true;
            fontTableChanged = true;
            qCDebug(FONTMATRIX_LOG) << fit->path() << "installed as" << valueName;
            // Windows loads the copy at the next logon anyway
            if (AddFontResourceW(nativePath(copy).c_str()) == 0) {
                qCWarning(FONTMATRIX_LOG) << "AddFontResource failed for" << copy;
                m_errors[fit->path()] = errorStrings[NO_FONT_RESOURCE];
            }
        } else // Deactivation
        {
            if (T->isSysFont(fit) || isInside(fit->localPath(), managed)) {
                m_errors[fit->path()] = errorStrings[LOCKED_FONT];
                continue;
            }
            if (!fit->isActivated()) {
                m_errors[fit->path()] = errorStrings[ALREADY_UNACTIVE];
                continue;
            }
            const QString copy(managed + QLatin1Char('/') + fit->activationName());
            // the value first: without it the font is gone at the next logon whatever happens to the file
            if (!unregisterUserFont(copy)) {
                m_errors[fit->path()] = errorStrings[NO_REGISTRY];
                continue;
            }
            stack[fit] = false;
            fontTableChanged = true;
            RemoveFontResourceW(nativePath(copy).c_str());
            // an application that uses the font keeps the file open; reconcileUserFonts() deletes it at the next start
            if (QFileInfo::exists(copy) && !QFile::remove(copy)) {
                qCWarning(FONTMATRIX_LOG) << "unable to delete" << copy;
                m_errors[fit->path()] = errorStrings[NO_UNLINK];
            }
        }
    }

    QStringList aList;
    FMFontDb::DB()->TransactionBegin();
    for (auto it(stack.constBegin()); it != stack.constEnd(); ++it) {
        it.key()->setActivated(it.value());
        aList << it.key()->path();
    }
    FMFontDb::DB()->TransactionEnd();

    if (fontTableChanged)
        broadcastFontChange();
    Q_EMIT activationEvent(aList);
}

void FMActivate::reconcileUserFonts()
{
    typotek *T(typotek::getInstance());
    const QString managed(T->getManagedDir());
    UserFontsKey key(true);
    if (!key.isOpen())
        return;
    QMap<QString, QString> values(key.values());

    // values of Fontmatrix's copies whose file has gone
    for (auto it = values.begin(); it != values.end();) {
        if (isInside(it.value(), managed) && !QFileInfo::exists(QDir::fromNativeSeparators(it.value()))) {
            qCDebug(FONTMATRIX_LOG) << "registry value without a file:" << it.key();
            key.remove(it.key());
            it = values.erase(it);
        } else {
            ++it;
        }
    }
    const auto isRegistered = [&values](const QString &path) {
        for (const QString &registered : std::as_const(values)) {
            if (samePath(registered, path))
                return true;
        }
        return false;
    };

    // the flag of the database against the folder and the registry
    const QList<FontItem *> fonts(FMFontDb::DB()->AllFonts());
    QMultiHash<QString, FontItem *> byFileName;
    QHash<FontItem *, bool> stack;
    for (FontItem *fit : fonts) {
        if (T->isSysFont(fit))
            continue;
        const QString local(fit->localPath());
        if (isInside(local, managed)) {
            // installed by the user in Settings > Fonts: active as long as it is registered
            const bool registered = isRegistered(local);
            if (registered != fit->isActivated())
                stack[fit] = registered;
            continue;
        }
        byFileName.insert(QFileInfo(local).fileName(), fit);
        if (!fit->isActivated())
            continue;
        const QString copy(managed + QLatin1Char('/') + fit->activationName());
        if (!QFileInfo::exists(copy) || !isRegistered(copy)) {
            qCDebug(FONTMATRIX_LOG) << fit->path() << "is not installed any more";
            stack[fit] = false;
        }
    }
    if (!stack.isEmpty()) {
        FMFontDb::DB()->TransactionBegin();
        for (auto it(stack.constBegin()); it != stack.constEnd(); ++it)
            it.key()->setActivated(it.value());
        FMFontDb::DB()->TransactionEnd();
    }

    // copies that were in use when their font was deactivated: "<size>-<file name>" of an inactive font
    const QFileInfoList files(QDir(managed).entryInfoList(QDir::Files));
    for (const QFileInfo &fi : files) {
        if (isRegistered(fi.absoluteFilePath()))
            continue;
        const QString name(fi.fileName());
        const qsizetype dash = name.indexOf(QLatin1Char('-'));
        if (dash <= 0)
            continue;
        bool numeric = false;
        const qint64 size = name.first(dash).toLongLong(&numeric);
        if (!numeric)
            continue;
        const QList<FontItem *> candidates(byFileName.values(name.sliced(dash + 1)));
        for (FontItem *fit : candidates) {
            if (fit->isActivated() || QFileInfo(fit->localPath()).size() != size)
                continue;
            if (QFile::remove(fi.absoluteFilePath()))
                qCDebug(FONTMATRIX_LOG) << "deleted the copy left behind:" << fi.absoluteFilePath();
            else
                qCWarning(FONTMATRIX_LOG) << "still unable to delete" << fi.absoluteFilePath();
            break;
        }
    }
}

#else // fontconfig

void FMActivate::activate(QList<FontItem *> fitList, bool act)
{
    QHash<FontItem *, bool> stack;
    typotek *T(typotek::getInstance());
    for (auto *fit : fitList) {
        if (act) // Activation
        {
            if (fit->isRemote() && !fit->isCached()) {
                // the file comes first; this font is activated on its own when it is here
                connect(
                    fit,
                    &FontItem::downloadFinished,
                    this,
                    [this, fit](bool ok) {
                        if (ok)
                            activate(QList<FontItem *>() << fit, true);
                    },
                    Qt::SingleShotConnection);
                fit->getFromNetwork();
                continue;
            }
            if (!T->isSysFont(fit)) {
                if (!fit->isActivated()) {
                    if (!QFile::link(fit->localPath(), T->getManagedDir() + "/" + fit->activationName())) {
                        qCWarning(FONTMATRIX_LOG) << "unable to link " << fit->path();
                        m_errors[fit->path()] = errorStrings[NO_LINK];
                    } else {
                        // Success
                        stack[fit] = true;
                        qCDebug(FONTMATRIX_LOG) << fit->path() << " linked";
                        if (!fit->afm().isEmpty()) {
                            if (!QFile::link(fit->afm(), T->getManagedDir() + "/" + fit->activationAFMName())) {
                                qCWarning(FONTMATRIX_LOG) << "unable to link " << fit->afm();
                                m_errors[fit->path()] = errorStrings[MISSING_AFM];
                            } else {
                                qCDebug(FONTMATRIX_LOG) << fit->afm() << " linked";
                            }
                        } else {
                            qCDebug(FONTMATRIX_LOG) << "There is no AFM file attached to " << fit->path();
                        }
                    }
                } else {
                    qCDebug(FONTMATRIX_LOG) << "\tYet activated";
                    m_errors[fit->path()] = errorStrings[ALREADY_ACTIVE];
                }

            } else {
                remFcReject(fit->path());
            }

        } else // Deactivation
        {
            if (!T->isSysFont(fit)) {
                if (fit->isActivated()) {
                    if (!QFile::remove(T->getManagedDir() + "/" + fit->activationName())) {
                        qCWarning(FONTMATRIX_LOG) << "unable to unlink " << fit->name();
                        m_errors[fit->path()] = errorStrings[NO_UNLINK];
                    } else {
                        stack[fit] = false;
                        if (!fit->afm().isEmpty()) {
                            if (!QFile::remove(T->getManagedDir() + "/" + fit->activationAFMName())) {
                                qCWarning(FONTMATRIX_LOG) << "unable to unlink " << fit->afm();
                                // if having warnings would not be over done, it would be a warning!
                                m_errors[fit->afm()] = errorStrings[NO_UNLINK];
                            }
                        }
                    }
                } else {
                    m_errors[fit->path()] = errorStrings[ALREADY_UNACTIVE];
                }

            } else {
                addFcReject(fit->path());
            }
        }
    }

    QStringList aList;
    FMFontDb::DB()->TransactionBegin();
    for (auto it(stack.constBegin()); it != stack.constEnd(); ++it) {
        it.key()->setActivated(it.value());
        aList << it.key()->path();
    }
    FMFontDb::DB()->TransactionEnd();

    Q_EMIT activationEvent(aList);
}

#endif

bool FMActivate::addFcReject([[maybe_unused]] const QString &path)
{
#ifdef HAVE_FONTCONFIG
    QFile fcfile(QDir::homePath() + "/.config/fontconfig/fonts.conf");
    if (!fcfile.open(QFile::ReadWrite)) {
        qWarning() << "Cannot open" << fcfile.fileName();
        return false;
    } else {
        QDomDocument fc("fontconfig");
        fc.setContent(&fcfile);
        QDomNodeList sellist = fc.elementsByTagName("selectfont");
        // First we search if there’s yet an entry for path
        if (!sellist.isEmpty()) {
            for (int s(0); s < sellist.count(); ++s) {
                QDomNodeList rejectlist(sellist.at(s).toElement().elementsByTagName("rejectfont"));
                if (!rejectlist.isEmpty()) {
                    for (int r(0); r < rejectlist.count(); ++r) {
                        QDomNodeList globlist(rejectlist.at(r).toElement().elementsByTagName("glob"));
                        if (!globlist.isEmpty()) {
                            for (int g(0); g < globlist.count(); ++g) {
                                QString t(globlist.at(g).toElement().text());
                                if (t == path) {
                                    qCDebug(FONTMATRIX_LOG) << "Already here";
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Now we can write in the first place available
        if (!sellist.isEmpty()) {
            QDomNodeList rejectlist(sellist.at(0).toElement().elementsByTagName("rejectfont"));
            if (!rejectlist.isEmpty()) {
                // 				QDomNodeList globlist( rejectlist.at(0).toElement().elementsByTagName("glob") );
                // 				if(!globlist.isEmpty())
                // 				{
                // 					QDomText pathelem = fc.createTextNode( path );
                // 					globlist.at(0).toElement().appendChild(pathelem);
                // 				}
                // 				else
                // 				{
                QDomElement globelem = fc.createElement("glob");
                QDomText pathelem = fc.createTextNode(path);
                globelem.appendChild(pathelem);
                rejectlist.at(0).toElement().appendChild(globelem);
                // 				}
            } else {
                QDomElement rejelem = fc.createElement("rejectfont");
                QDomElement globelem = fc.createElement("glob");
                QDomText pathelem = fc.createTextNode(path);
                globelem.appendChild(pathelem);
                rejelem.appendChild(globelem);
                sellist.at(0).toElement().appendChild(rejelem);
            }
        } else {
            QDomElement root = fc.documentElement();
            QDomElement selelem = fc.createElement("selectfont");
            QDomElement rejelem = fc.createElement("rejectfont");
            QDomElement globelem = fc.createElement("glob");
            QDomText pathelem = fc.createTextNode(path);
            globelem.appendChild(pathelem);
            rejelem.appendChild(globelem);
            selelem.appendChild(rejelem);
            root.appendChild(selelem);
        }

        fcfile.resize(0);
        QTextStream ts(&fcfile);
        fc.save(ts, 4);
        fcfile.close();
    }
#endif //  HAVE_FONTCONFIG
    return true;
}

bool FMActivate::remFcReject([[maybe_unused]] const QString &path)
{
#ifdef HAVE_FONTCONFIG
    QFile fcfile(QDir::homePath() + "/.config/fontconfig/fonts.conf");
    if (!fcfile.open(QFile::ReadWrite)) {
        return false;
    } else {
        QDomDocument fc("fontconfig");
        fc.setContent(&fcfile);
        QDomNodeList sellist = fc.elementsByTagName("selectfont");

        if (!sellist.isEmpty()) {
            for (int s(0); s < sellist.count(); ++s) {
                QDomNodeList rejectlist(sellist.at(s).toElement().elementsByTagName("rejectfont"));
                if (!rejectlist.isEmpty()) {
                    for (int r(0); r < rejectlist.count(); ++r) {
                        QDomNodeList globlist(rejectlist.at(r).toElement().elementsByTagName("glob"));
                        if (!globlist.isEmpty()) {
                            for (int g(0); g < globlist.count(); ++g) {
                                QString t(globlist.at(g).toElement().text());
                                if (t == path) {
                                    rejectlist.at(r).removeChild(globlist.at(g).toElement());
                                    fcfile.resize(0);
                                    QTextStream ts(&fcfile);
                                    fc.save(ts, 4);
                                    fcfile.close();
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#endif //  HAVE_FONTCONFIG
    return true;
}

QMap<QString, QString> FMActivate::errors()
{
    QMap<QString, QString> ret(m_errors);
    m_errors.clear();
    return ret;
}

#include "moc_fmactivate.cpp"
