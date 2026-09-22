/*
    SPDX-FileCopyrightText: 2008 Pierre Marchand <pierremarc@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FMACTIVATE_H
#define FMACTIVATE_H

#include <QHash>
#include <QMap>
#include <QObject>
#include <QString>

class FontItem;

class FMActivate : public QObject
{
    Q_OBJECT

    FMActivate();
    static FMActivate *instance;

    // no name here may be a macro of windows.h: wingdi.h has ERROR and SYSTEM_FONT
    enum Error {
        NO_LINK = 0,
        ALREADY_ACTIVE,
        NO_UNLINK,
        ALREADY_UNACTIVE,
        MISSING_AFM,
        OTHER_ERROR,
        // Windows
        NO_COPY,
        NO_REGISTRY,
        NO_FONT_RESOURCE,
        LOCKED_FONT,
        UNSUPPORTED_FORMAT
    };

    QHash<Error, QString> errorStrings;
    void setErrorStrings();

public:
    static FMActivate *getInstance();

    //		void activate(FontItem* fit , bool act );
    void activate(QList<FontItem *> fitList, bool act);
    QMap<QString, QString> errors();

#ifdef _WIN32
    /**
     * Brings the database in line with the per-user font folder and the
     * registry, once per start: a font whose copy or registry value has gone
     * (removed in Settings > Fonts) is flagged inactive, a registry value that
     * points to a file that has gone is deleted, and a copy that could not be
     * deleted at deactivation because it was in use is deleted now.
     */
    void reconcileUserFonts();
#endif

Q_SIGNALS:
    void activationEvent(const QStringList &);

private:
    /*
    Add and Remove fonts in ~/.config/fontconfig/fonts.conf
    with <selecfont><rejectfont><glob> sequence
    */
    bool addFcReject(const QString &path);
    bool remFcReject(const QString &path);

    QMap<QString, QString> m_errors;
};

#endif
