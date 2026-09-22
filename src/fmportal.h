/*
    SPDX-FileCopyrightText: 2026 Blagovest Petrov <blagovest@petrovs.info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FMPORTAL_H
#define FMPORTAL_H

#include <QString>

class QWidget;

/**
 * The desktop portals, the way a sandboxed application asks the desktop for
 * what it cannot do itself.
 *
 * File dialogs and links need nothing here: Qt sends them through the portal by
 * itself when it finds the application in a sandbox. What is left is handing a
 * font file to another application, which Fontmatrix does for the font editor.
 */
namespace FMPortal
{
/// whether the portal answers, so that a file can be handed to the desktop
bool isAvailable();

/**
 * Asks the desktop which application should open the file, and gives that
 * application the file with permission to write it, so that a font editor can
 * save what it changes. False when there is no portal or it refused.
 */
bool openWith(const QString &path, QWidget *parent = nullptr);
}

#endif
