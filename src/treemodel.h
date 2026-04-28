/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "treeitem.h" //CB include this here to make subclasses easier

class TreeModel : public QAbstractItemModel
{
// 		Q_OBJECT

	public:
		TreeModel ( const QString &data, QObject *parent = nullptr );
		TreeModel ( QObject *parent = nullptr ) {}; //CB Added for ScHelpTreeModel
		~TreeModel() override;

		QVariant data ( const QModelIndex &index, int role ) const override;
		Qt::ItemFlags flags ( const QModelIndex &index ) const override;
		QVariant headerData ( int section, Qt::Orientation orientation,
		                      int role = Qt::DisplayRole ) const override;
		QModelIndex index ( int row, int column,
		                    const QModelIndex &parent = QModelIndex() ) const override;
		QModelIndex parent ( const QModelIndex &index ) const override;
		int rowCount ( const QModelIndex &parent = QModelIndex() ) const override;
		int columnCount ( const QModelIndex &parent = QModelIndex() ) const override;

	protected:
		void setupModelData ( const QStringList &lines, TreeItem *parent );

		TreeItem *rootItem;
};

#endif
