/*
    SPDX-FileCopyrightText: 2008 Pierre Marchand <pierremarc@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

/* fmpath.h */
/* Get all useful paths in one place with the hope that */
/* finally we will not need resource.qrc anymore */
#ifndef FMPATHS_H
#define FMPATHS_H

#include <QDir>
#include <QLocale>
#include <QMap>
#include <QString>

class FMPaths
{
    QMap<QString, QString> FMPathsDB;
    FMPaths() = default;
    // Meyers singleton — thread-safe by C++11 static-local guarantee
    static FMPaths *getThis();

public:
    static QString TranslationsDir();

    static QString ResourcesDir();

    static QString HelpDir();

    static QString SamplesDir();

    static QString FiltersDir();

    static QString LocalizedDirPath(const QString &base, const QString &fallback = QStringLiteral("en"));

    static QString LocalizedFilePath(const QString &base, const QString &ext, const QString &fallback = QStringLiteral("en"));
    /**
     * The hyphenation dictionary for @p locale among those installed on the system
     * (the hyph_xx_YY.dic files of the hunspell "hyphen-*" packages, in every
     * <data dir>/hyphen: /usr/share/hyphen, the Flatpak runtime, <appdir>/data on
     * Windows). "bg_BG" is tried, then any "bg_*", then any "bg"; an empty string when
     * there is none.
     */
    static QString HyphenationDictionary(const QLocale &locale = QLocale::system());
};
#endif
