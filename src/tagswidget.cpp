//
// C++ Implementation: tagswidget
//
// Description:
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <KLocalizedString>
#include <QInputDialog>
#include <KMessageBox>
#include <QMenu>
#include <QDebug>
#include <QFont>
#include <QModelIndex>

#include "typotek.h"
#include "tagswidget.h"
#include "fontitem.h"
#include "fmfontdb.h"
#include "tagswidget_listmodel.h"



TagsWidget::TagsWidget ( QWidget * parent )
		:QWidget ( parent )
{
	setupUi ( this );

	model = new TagsWidget_ListModel(this);
	tagsListView->setModel(model);

	connect ( newTagButton, &QPushButton::clicked, this, &TagsWidget::slotNewTag );
	connect( removeTagButton, &QPushButton::clicked, this, &TagsWidget::slotActRemovetag );

}

TagsWidget::~ TagsWidget()
= default;

void TagsWidget::prepare(QList<FontItem *> fonts)
{
	model->setFonts(fonts);
}

void TagsWidget::slotNewTag()
{
	QModelIndex  idx(model->addTag());
	if(!idx.isValid())
		return;
	tagsListView->setCurrentIndex(idx);
	tagsListView->edit(idx);
}



void TagsWidget::slotActRemovetag()
{
	QModelIndex idx(tagsListView->currentIndex());
	if(!idx.isValid())
		return;
	QString currentTag(model->data(idx, Qt::DisplayRole).toString());
	QString message;
	message = i18n( "Please confirm that you want to remove\nthe following tag from database:" ) + " " + currentTag;
	if ( KMessageBox::warningContinueCancel ( typotek::getInstance(),
	                                          message,
	                                          i18nc ( "@title:window", "Remove Tag" ),
	                                          KStandardGuiItem::remove(),
	                                          KStandardGuiItem::cancel(),
	                                          QString(),
	                                          KMessageBox::Options(KMessageBox::Notify | KMessageBox::Dangerous) )
	        == KMessageBox::Continue )
	{
		FMFontDb::DB()->removeTagFromDB ( currentTag );
	}

}

#include "moc_tagswidget.cpp"
