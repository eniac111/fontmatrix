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

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QObject>
#include <QMap>

class QAction;
class QMenu;
class KStatusNotifierItem;
class typotek;

class Systray : public QObject
{
	Q_OBJECT

public:
	Systray();
	~Systray() override;

	bool isVisible();
	bool hasActivateAll();
	bool allConfirmation();
	bool tagsConfirmation();

public slots:
	void show();
	void hide();
	void slotSetVisible(bool isVisible);
	void slotSetActivateAll(bool isVisible);
	void newTag(QString);
	void deleteTag(const QString &name);
	void updateTagMenu(const QStringList& nameOfFontWhichCausedThisUpdate);
	void requireAllConfirmation(bool doRequire);
	void requireTagsConfirmation(bool doRequire);

private slots:
	void slotActivateAll();
	void slotDeactivateAll();
	void slotMinimize();
	void slotRestore();
	void slotTagMenuClicked(QAction *action);
	void slotQuit();
	void slotPrepareMenu();

private:
	bool confirmAll;
	bool confirmTags;

	void createActions();
	void createTrayIcon();
	void createTagMenu();

	QAction *activateAllAction;
	QAction *deactivateAllAction;
	QAction *minimizeAction;
	QAction *restoreAction;
	QAction *quitAction;

	KStatusNotifierItem *trayIcon;
	QMenu               *trayIconMenu;
	QMenu               *tagMenu;
// 	QMenu           *tagSetMenu;
	
	QMap<QString, QAction*> tagActions;

	static typotek *ttek;

	bool showAllConfirmation;
	bool showTagsConfirmation;

	Systray(const Systray&) = delete;
	Systray& operator=(const Systray&) = delete;
	
// 	protected:
// 		bool eventFilter ( QObject * watched, QEvent * event ) ;
};

#endif
