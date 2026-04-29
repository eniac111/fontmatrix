/***************************************************************************
 *   Copyright (C) 2007 by Riku Leino                                      *
 *   riku@scribus.info                                                     *
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

#include "systray.h"
#include "fmconfig.h"
#include "mainviewwidget.h"
#include "typotek.h"
#include "fontitem.h"
#include "fmfontdb.h"
#include <QtGui>
#include <QMenu>
#include <QDebug>
#include <KStatusNotifierItem>

typotek* Systray::ttek = nullptr;

Systray::Systray()
{
	createActions();
	createTrayIcon();
	createTagMenu();

	connect(trayIconMenu, &QMenu::aboutToShow, this, &Systray::slotPrepareMenu);

	showAllConfirmation = FMConfig::value(QStringLiteral("Systray/AllConfirmation"), true).toBool();
	showTagsConfirmation = FMConfig::value(QStringLiteral("Systray/TagsConfirmation"), false).toBool();

	slotSetActivateAll(FMConfig::value(QStringLiteral("Systray/ActivateAllVisible"), false).toBool());

	const QIcon trayThemed = QIcon::fromTheme(QStringLiteral("fontmatrix-tray"),
	                                          QIcon(QStringLiteral(":/fontmatrix_systray_icon.png")));
	trayIcon->setIconByPixmap(trayThemed);
	trayIcon->setToolTipIconByPixmap(trayThemed);
	trayIcon->setToolTipTitle(QStringLiteral("Fontmatrix"));

	if (FMConfig::value(QStringLiteral("Systray/Visible"), false).toBool())
		trayIcon->setStatus(KStatusNotifierItem::Active);
	else
		trayIcon->setStatus(KStatusNotifierItem::Passive);
}

Systray::~Systray()
{

}

void Systray::slotSetVisible(bool isVisible)
{
	trayIcon->setStatus(isVisible ? KStatusNotifierItem::Active
	                              : KStatusNotifierItem::Passive);
	FMConfig::setValue(QStringLiteral("Systray/Visible"), isVisible);
}

void Systray::slotSetActivateAll(bool isVisible)
{
	activateAllAction->setVisible(isVisible);
	deactivateAllAction->setVisible(isVisible);
	FMConfig::setValue(QStringLiteral("Systray/ActivateAllVisible"), isVisible);
}

void Systray::show()
{
	trayIcon->setStatus(KStatusNotifierItem::Active);
}

void Systray::hide()
{
	trayIcon->setStatus(KStatusNotifierItem::Passive);
}

void Systray::slotMinimize()
{
	ttek->showMinimized();
}

void Systray::slotRestore()
{
	ttek->showNormal();
}

void Systray::slotActivateAll()
{
//	ttek->theMainView->slotViewAll();
//	if (showAllConfirmation) {
//		bool wasVisible = ttek->isVisible();
//		if (!wasVisible)
//			ttek->show();
//		ttek->slotActivateCurrents();
//		if (!wasVisible)
//			ttek->hide();
//	} else
//		ttek->theMainView->slotActivateAll();

//	disconnect(tagMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotTagMenuClicked(QAction*)));
//	QList<QAction*> tags = tagActions.values();
//	for (auto* a : tags) {
//		a->setChecked(true);
//	}
//	connect(tagMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotTagMenuClicked(QAction*)));
}

void Systray::slotDeactivateAll()
{
//	ttek->theMainView->slotViewAll();
//	if (showAllConfirmation) {
//		bool wasVisible = ttek->isVisible();
//		if (!wasVisible)
//			ttek->show();
//		ttek->slotDeactivateCurrents();
//		if (!wasVisible)
//			ttek->hide();
//	} else
//		ttek->theMainView->slotDesactivateAll();
//	disconnect(tagMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotTagMenuClicked(QAction*)));
//	QList<QAction*> tags = tagActions.values();
//	for (auto* a : tags) {
//		a->setChecked(false);
//	}
//	connect(tagMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotTagMenuClicked(QAction*)));
}

void Systray::slotTagMenuClicked(QAction *)
{

	// Deactivate the feature atm
//	action->setIcon(QIcon ());
//	QString name = action->text();
//	if (name.isEmpty())
//		return;

//	if (!action->isChecked()) { // deactivate based on the tag name
//		ttek->theMainView->slotFilterTag(name);
//		if (showTagsConfirmation) {
//			bool wasVisible = ttek->isVisible();
//			if (!wasVisible)
//				ttek->show();
//			ttek->slotDeactivateCurrents();
//			if (!wasVisible)
//				ttek->hide();
//		} else
//			ttek->theMainView->slotDesactivateAll();
//	} else { // activate based on the tag name
//		ttek->theMainView->slotFilterTag(name);
//		if (showTagsConfirmation) {
//			bool wasVisible = ttek->isVisible();
//			if (!wasVisible)
//				ttek->show();
//			ttek->slotActivateCurrents();
//			if (!wasVisible)
//				ttek->hide();
//		} else
//			ttek->theMainView->slotActivateAll();
//	}
}

void Systray::slotQuit()
{
		ttek->writeSettings();
		qApp->quit();
}

void Systray::slotPrepareMenu()
{

	if (ttek->isVisible() && !ttek->isMinimized()) {
		restoreAction->setEnabled(false);
		minimizeAction->setEnabled(true);
	} else if (ttek->isHidden() || ttek->isMinimized()) {
		restoreAction->setEnabled(true);
		minimizeAction->setEnabled(false);
	}
}

void Systray::newTag(QString name)
{
	qDebug()<<"Systray::newTag"<<name;
	if (tagActions.contains(name))
		return; // already added

	QAction *tmp = tagMenu->addAction(name);
	tmp->setCheckable(true);
	QList<FontItem*> taggedFonts = FMFontDb::DB()->Fonts( name , FMFontDb::Tags );
	int nActivated(0);
	int nFonts(taggedFonts.count());
	for(int i = 0; i <  nFonts ; ++i)
	{
// 		qDebug()<<taggedFonts[i]->path();
		Q_ASSERT(taggedFonts[i]);
		if(taggedFonts[i]->isActivated())
		{
			++nActivated;
			
		}
	}
	if(nActivated < nFonts)
	{
		if(nActivated > 0)
		{
			QFont f(tmp->font());
			f.setBold(true);
			tmp->setFont(f);
			tmp->setText(name + " "+QString::number(nActivated)+"/"+QString::number(nFonts));
			tmp->setChecked(false);
		}
		else
		{
			tmp->setChecked(false);
		}
	}
	else
		tmp->setChecked(true);
	
	tagActions[name] = tmp;
}

void Systray::deleteTag(const QString &name)
{
	QAction *tmp = tagActions[name];
	if (tmp) {
		tagMenu->removeAction(tmp);
		tagActions.remove(name);
	}
}

void Systray::createActions()
{
    activateAllAction = new QAction(tr("&Activate all"), this);
    connect(activateAllAction, SIGNAL(triggered()), this, SLOT(slotActivateAll()));

    deactivateAllAction = new QAction(tr("&Deactivate all"), this);
    connect(deactivateAllAction, SIGNAL(triggered()), this, SLOT(slotDeactivateAll()));

    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(slotMinimize()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(slotRestore()));

    quitAction = new QAction(tr("E&xit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(slotQuit()));
}

void Systray::createTrayIcon()
{
	if (!ttek)
		ttek = typotek::getInstance();

	trayIconMenu = new QMenu(nullptr);
	trayIconMenu->addAction(activateAllAction);
	trayIconMenu->addAction(deactivateAllAction);
	tagMenu = trayIconMenu->addMenu(tr("&Tags"));
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(minimizeAction);
	trayIconMenu->addAction(restoreAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new KStatusNotifierItem(QStringLiteral("fontmatrix"), this);
	trayIcon->setCategory(KStatusNotifierItem::ApplicationStatus);
	trayIcon->setTitle(QStringLiteral("Fontmatrix"));
	trayIcon->setStandardActionsEnabled(false);
	trayIcon->setContextMenu(trayIconMenu);

	connect(trayIcon, &KStatusNotifierItem::activateRequested,
	        this, [](bool /*active*/, const QPoint & /*pos*/) {
		if (!ttek)
			return;
		if (ttek->isVisible())
			ttek->hide();
		else
			ttek->show();
	});
}

void Systray::createTagMenu()
{
	if (!ttek)
		ttek = typotek::getInstance();

	QStringList tmp(FMFontDb::DB()->getTags());
	tmp.sort();
	for (const auto& tagName : tmp) {
// 		if (tagName != "Activated_On" && tagName != "Activated_Off")
			newTag(tagName);
	}

	connect(tagMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotTagMenuClicked(QAction*)));
}

bool Systray::isVisible()
{
	return trayIcon->status() != KStatusNotifierItem::Passive;
}

bool Systray::hasActivateAll()
{
	return activateAllAction->isVisible();
}

bool Systray::allConfirmation()
{
	return showAllConfirmation;
}

bool Systray::tagsConfirmation()
{
	return showTagsConfirmation;
}

void Systray::requireAllConfirmation(bool doRequire)
{
	showAllConfirmation = doRequire;
	FMConfig::setValue(QStringLiteral("Systray/AllConfirmation"), doRequire);
}

void Systray::requireTagsConfirmation(bool doRequire)
{
	showTagsConfirmation = doRequire;
	FMConfig::setValue(QStringLiteral("Systray/TagsConfirmation"), doRequire);
}

void Systray::updateTagMenu(const QStringList& nameOfFontWhichCausedThisUpdate)
{
	QStringList tags(tagActions.keys());
	bool lazy = true;
	for (const auto& tag : tags)
	{
		QList<FontItem*> taggedFonts = FMFontDb::DB()->Fonts( tag , FMFontDb::Tags );
//		ttek->resetFilter();
		for (auto* fit : taggedFonts)
		{
			if( nameOfFontWhichCausedThisUpdate.contains(fit->path()))
			{	// we’re concerned
				lazy = false;
				break;
			}
		}
	}
	if(lazy)
		return;
	for (const auto& tag : tags)
	{
		deleteTag(tag);
	}
	
	if (!ttek)
		ttek = typotek::getInstance();

	QStringList tmp(FMFontDb::DB()->getTags());
	tmp.sort();
	for (const auto& tagName : tmp) {
// 		if (tagName != "Activated_On" && tagName != "Activated_Off")
			newTag(tagName);
	}
	
}

