/*
    SPDX-FileCopyrightText: 2007 Pierre Marchand <pierre@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef DATALOADER_H
#define DATALOADER_H

#include <QMap>
#include <QString>

/**
    @author Pierre Marchand <pierre@oep-h.com>
*/
class DataLoader
{
    QMap<QString, QMap<QString, QString>> sm;
    QMap<QString, QString> pm;

    void load();

public:
    DataLoader();
    ~DataLoader() = default;

    bool update(const QString &name, const QString &sample);
    bool remove(const QString &name);
    void reload();

    [[nodiscard]] const QMap<QString, QMap<QString, QString>> &systemSamples() const
    {
        return sm;
    }
    [[nodiscard]] const QMap<QString, QString> &userSamples() const
    {
        return pm;
    }
};

#endif
