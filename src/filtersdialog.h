/***************************************************************************
 *   Copyright (C) 2010 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef FILTERSDIALOG_H
#define FILTERSDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>

namespace Ui {
    class FiltersDialog;
}

class FiltersDialogItem;
class FilterItem;
class FilterData;

class FiltersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FiltersDialog(const QList<FilterItem*>& currentFilter, QWidget *parent = nullptr);
    ~FiltersDialog() override;

private:
    Ui::FiltersDialog *ui;

    void loadFilters();
    QString filterString(FilterData *d, bool first = false);
    QList<FiltersDialogItem*> items;

    // Lazy-initialised translated strings; static-init i18n() would run before
    // KLocalizedString::setApplicationDomain() and fall back to source text.
    static const QString &andOp();
    static const QString &notOp();
    static const QString &orOp();

signals:
    void Filter(QString);
    void AddFilter(QString);
    void RemoveFilter(QString);

private slots:
    void slotAddFilter();
    void slotRemoveFilter(QString fs);
};

#endif // FILTERSDIALOG_H
