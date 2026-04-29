//
// C++ Interface: prefspaneldialog
//
// Description:
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PREFSPANELDIALOG_H
#define PREFSPANELDIALOG_H

#include <KPageDialog>
#include <ui_prefs_panel.h>

class QStandardItemModel;
class KPageWidgetItem;

/**
	@author Pierre Marchand <pierremarc@oep-h.com>
*/
class PrefsPanelDialog : public KPageDialog, private Ui::PrefsPanel
{
	Q_OBJECT
	public:
		PrefsPanelDialog ( QWidget *parent );

		~PrefsPanelDialog() override;

		enum PAGE{PAGE_GENERAL = 0,
			PAGE_SYSTRAY,
   			PAGE_DISPLAY,
   			PAGE_SERVICES,
			PAGE_SAMPLETEXT,
   			PAGE_FILES,
   			PAGE_SHORTCUTS};

		void initSystrayPrefs(bool hasSystray, bool isVisible, bool hasActivateAll, bool allConfirmation, bool tagConfirmation);
		void initSampleTextPrefs();
		void initFilesAndFolders();
		void initShortcuts();
		void showPage(PAGE page);

		bool event( QEvent* ev ) override;
		void keyPressEvent(QKeyEvent *k) override;
		void keyReleaseEvent(QKeyEvent *k) override;
		void done(int r) override;
		static QString getKeyText(int KeyC);

	private:
		void doConnect();
		QStandardItemModel *shortcutModel;
		QDialog *m_uiHolder;
		KPageWidgetItem *m_pageGeneral;
		KPageWidgetItem *m_pageSystray;
		KPageWidgetItem *m_pageDisplay;
		KPageWidgetItem *m_pageTools;
		KPageWidgetItem *m_pageSampleText;
		KPageWidgetItem *m_pageFiles;
		KPageWidgetItem *m_pageShortcuts;

		/* For the keyboard shortcut */
		int keyCode;
		QString Part0;
		QString Part1;
		QString Part2;
		QString Part3;
		QString Part4;
		void shortcutSet(const QString &shortcut);
		void reloadShortcuts();
		void setSelected(const QString &actionText);

	private slots:
		void applySampleText();

		void addSampleName();
		void deleteSampleName();
		void validateSampleName();
		void displayNamedText();

		void setSystrayVisible(bool);
		void setSystrayActivateAll(bool);
		void setSystrayAllConfirmation(bool);
		void setSystrayTagsConfirmation(bool);

		void updateWord(QString);
		void updateWordSize(double);
		void updateWordRTL(int);
		void updateWordSubtitled(int);

		void updateChartFont(const QFont & font);

		void setupFontEditor(QString);
		void slotFontEditorBrowse();
		
		void addAndSelectWebBrowser();
		void selectWebBrowser(const QString & text );
		void setupWebBrowserOptions(const QString & text );
		
		void selectInfoStyle(const QString& css);

		void setupTemplates(const QString&);
		void slotTemplatesBrowse();

		void slotAddRemote();
		void slotRemoveRemote();

		void slotSetLocalStorage(QString s);
		void slotBrowseLocalStorage();

		void slotShowImportedFonts(int i);
// 		void slotFamilyNotPreferred(bool state);
		void slotSplashScreen(bool state);

		void slotChangeShortcut();
		void slotClearShortcut();
		void slotActionSelected(const QModelIndex &mi);

		void slotDictDialog();


};

#endif
