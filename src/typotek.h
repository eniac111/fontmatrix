/***************************************************************************
 *   Copyright (C) 2007 by Pierre Marchand   *
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


#ifndef TYPOTEK_H
#define TYPOTEK_H

#include <KXmlGuiWindow>
#include <QCloseEvent>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QLabel>
#include <QThread>
#include <QTime>

#include "prefspaneldialog.h"

class QAction;
class QMenu;
class QTextEdit;
class MainViewWidget;
class BrowserWidget;
class FontItem;
class QDockWidget;
class Systray;
class RemoteDir;
class FMHyphenator;
class QProgressBar;
class DataLoader;
class FloatingWidget;
class QStackedWidget;

class typotek : public KXmlGuiWindow
{
	Q_OBJECT

	static typotek* instance;
	static bool matrix;
	typotek();
	~typotek() override;
public:
	static typotek* getInstance();
	void initMatrix();
	void postInit();

protected:
	void closeEvent ( QCloseEvent *event ) override;
	void keyPressEvent ( QKeyEvent * event ) override ;

private Q_SLOTS:
	void fontBook();
	void slotActivateCurrents();
	void slotDeactivateCurrents();
	void slotEditFont();
	void toggleShowMenuBar(bool showMessage = true);
	void slotExportFontSet();
	void slotRemoteIsReady();
	void slotRepair();
	void slotTagAll();
	void slotDockAreaChanged(Qt::DockWidgetArea area);
//	void slotSwitchLayOptVisible();
//	void slotUpdateLayOptStatus();
	void slotShowTTTables();
	void slotEditPanose();
	void slotDumpInfo();
	void slotReloadFiltered();
	void slotReloadSingle();

	void slotExportXeTeX();

	void slotExtractFont();
	void slotMatchRaster();
	void slotHelpContents();

public Q_SLOTS:
	void open( QString path = QString(), bool recursive = true, bool announce = true, bool collect = false );
	void importFiles();
	void openList( QStringList files );
	void slotCloseToSystray(bool isEnabled);
	void slotSystrayStart(bool isEnabled);
	void slotUseInitialTags(bool isEnabled);
	void setImportedFontsHidden(bool hidden);
	bool showImportedFonts();
	void slotPrefsPanelDefault();
	void slotPrefsPanel(PrefsPanelDialog::PAGE page);
	void relayStartingStepIn(QString s);
	void showToltalFilteredFonts();
	void updateFloatingStatus();
	void closeAllFloatings();
	void showAllFloatings();
	void hideAllFloatings();
	void toggleMainView(bool v);
	void pushObject(QObject* o);
	// Force a real quit, bypassing the close-to-tray hide.
	// Used by File → Quit and the systray's "Exit" action.
	void slotQuit();

	// QWidget::show() and hide() are not virtual, both end up here
	void setVisible(bool visible) override;

Q_SIGNALS:
	void relayStartingStepOut(QString, int, QColor);
	void previewHasChanged();
	void newFontsArrived();

private:
	void installDock(const QString& id, const QString& name, QWidget *w, const QString& tip=QString() );
	void createActions();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	bool maybeSave();
	void initDir();
	void doConnect();
	void setupDrop();

	// Set by slotQuit() so closeEvent skips the close-to-tray hide branch.
	bool m_forceQuit = false;
	// Set by closeEvent() once the panels are being torn down; setVisible()
	// must not touch them any more when Qt hides the closed window.
	bool m_closing = false;

	// The font a menu action applies to; says so in the status bar when
	// there is none.
	FontItem* fontForAction();

	void checkOwnDir();
	void fillTagsList();

	QTextEdit *textEdit = nullptr;
	QString curFile;

	QMenu *viewMenu = nullptr;
	QAction *openAct = nullptr;
	QAction *importFilesAction = nullptr;
	QAction *fontBookAct = nullptr;
	QAction *activCurAct = nullptr;
	QAction *deactivCurAct = nullptr;
	QAction *fonteditorAct = nullptr;
	QAction *exportFontSetAct = nullptr;
	QAction *repairAct = nullptr;
	QAction *tagAll = nullptr;
	QAction *showTTTAct = nullptr;
	QAction *editPanoseAct = nullptr;
	QAction *dumpInfoAct = nullptr;
	QAction *reloadAct = nullptr;
	QAction *reloadSingleAct = nullptr;
	QAction *exportXeTeXAct = nullptr;
	QAction *extractFontAction = nullptr;
	QAction *matchRasterAct = nullptr;

	QAction *playAction = nullptr;
	QAction *compareAction = nullptr;
	QAction *closeAllFloat = nullptr;
	QAction *showAllFloat = nullptr;
	QAction *hideAllFloat = nullptr;
	QAction *floatSep = nullptr;
	QAction *m_paShowMenuBar = nullptr;

//	QAction *layOptAct;

	QProgressBar *statusProgressBar = nullptr;

	QStackedWidget * mainStack = nullptr;
	MainViewWidget *theMainView = nullptr;
	BrowserWidget * theBrowser = nullptr;

	QFile ResourceFile;
	QDir ownDir;
	QDir configDir;
	QDir managedDir;

	DataLoader * dataLoader = nullptr;
	//		QMap<QString,QString> m_namedSamples;
	QString m_theWord;

	QLabel *curFontPresentation = nullptr;
	QLabel *countFilteredFonts = nullptr;

	Systray *systray = nullptr;

	bool useInitialTags = false;
	bool showFontListDialog = false;
	static QString fonteditorPath;
	QString templatesDir;
	double previewSize = 0.0;
	bool previewRTL = false;
	bool previewSubtitled = false;
	bool m_familySchemeFreetype = false;
	QString m_sysTagName;

	void addFcDirItem(const QString &dirPath);
	QStringList getSystemFontDirs();
	QStringList sysFontList;

	RemoteDir *remoteDir = nullptr;
	QString m_remoteTmpDir;

	QMap<QString, QDockWidget*>  dockWidget;
	QMap<QString, QString> dockArea;
	QMap<QString, bool> dockVisible;
	QMap<QString, QRect> dockGeometry;

	FMHyphenator *hyphenator = nullptr;

	QString defaultOTFScript;
	QString defaultOTFLang;
	QStringList defaultOTFGPOS;
	QStringList defaultOTFGSUB;

	int chartInfoFontSize = 0;
	QString chartInfoFontName;

	double previewInfoFontSize = 0.0;

	QString databaseDriver;
	QString databaseHostname;
	QString databaseDbName;
	QString databaseUser;
	QString databasePassword;

	int panoseMatchTreshold = 0;

	double m_dpiX;
	double m_dpiY;

	QMap<FloatingWidget*, QAction*> floatingWidgets;
	QMap<FloatingWidget*, bool> visibleFloatingWidgets;
	bool playVisible;
	bool compareVisible = false;

	QString currentNamedSample;

	QToolButton * toggleMainViewButton = nullptr;

public:
	bool isSysFont(FontItem* f);
	FontItem* getSelectedFont();
	void resetFilter();


	QString getManagedDir(){return managedDir.absolutePath();}

	QFile* getResourceFile(){ return &ResourceFile; }

	void setSampleText(QString s);

	void presentFontName(QString s);

	void forwardUpdateView();

	// TODO there is a lot of things here which MUST go to an independent PrefsManager class

	[[nodiscard]] Systray *getSystray() const {return systray;}
	void setSystrayVisible(bool);
	void showActivateAllSystray(bool);
	void systrayAllConfirmation(bool);
	void systrayTagsConfirmation(bool);

	// Samples
	QString namedSample(QString name = QString());
	QMap<QString,QList<QString> > namedSamplesNames();
	void addNamedSample(QString name, QString sample);
	void removeNamedSample(const QString& key);
	void changeSample(QString name, QString text);
	QString defaultSampleName();

	void setFontEditorPath(const QString &path);
	QString fontEditorPath() {return fonteditorPath;}

	bool initialTags() { return useInitialTags;}

	void setTemplatesDir(const QString &dir);
	QString getTemplatesDir() {return templatesDir;}

	void setWord(QString s, bool updateView);
	QString word(FontItem * item = nullptr, const QString& alt = QString());
	void setPreviewSize(double d);
	double getPreviewSize(){ return previewSize; }
	void setPreviewRTL(bool d);
	bool getPreviewRTL(){ return previewRTL; }
	void setPreviewSubtitled(bool d);
	bool getPreviewSubtitled(){ return previewSubtitled; }

	void removeFontItem(QString key);
	void removeFontItem(QStringList keyList);

	void changeFontSizeSettings(double fSize, double lSize);

	void showStatusMessage(const QString &message);

	[[nodiscard]] QString remoteTmpDir() const {return m_remoteTmpDir;}
	void setRemoteTmpDir(const QString &s);





	[[nodiscard]] bool familySchemeFreetype() const{return m_familySchemeFreetype;}
	void setFamilySchemeFreetype ( bool theValue ){m_familySchemeFreetype = theValue;}

	[[nodiscard]] FMHyphenator* getHyphenator() const;

	void setDefaultOTFScript ( const QString& theValue );
	[[nodiscard]] QString getDefaultOTFScript() const;
	void setDefaultOTFLang ( const QString& theValue );
	[[nodiscard]] QString getDefaultOTFLang() const;
	void setDefaultOTFGPOS ( const QStringList& theValue );
	[[nodiscard]] QStringList getDefaultOTFGPOS() const;
	void setDefaultOTFGSUB ( const QStringList& theValue );
	[[nodiscard]] QStringList getDefaultOTFGSUB() const;

	void startProgressJob(int max);
	void runProgressJob(int i = 0);
	void endProgressJob();

	[[nodiscard]] int getChartInfoFontSize() const{return chartInfoFontSize;}
	[[nodiscard]] QString getChartInfoFontName() const{return chartInfoFontName;}

	void setChartInfoFontSize ( int theValue ){chartInfoFontSize = theValue;}
	void setChartInfoFontName ( const QString& theValue ){chartInfoFontName = theValue;}

	[[nodiscard]] MainViewWidget* getTheMainView() const{return theMainView;}

	void setDatabaseDriver ( const QString& theValue ){databaseDriver = theValue;}
	[[nodiscard]] QString getDatabaseDriver() const{return databaseDriver;}

	void setDatabaseHostname ( const QString& theValue ){databaseHostname = theValue;}
	[[nodiscard]] QString getDatabaseHostname() const{return databaseHostname;}

	void setDatabaseDbName ( const QString& theValue ){databaseDbName = theValue;}
	[[nodiscard]] QString getDatabaseDbName() const{return databaseDbName;}

	void setDatabaseUser ( const QString& theValue ){databaseUser = theValue;}
	[[nodiscard]] QString getDatabaseUser() const{return databaseUser;}

	void setDatabasePassword ( const QString& theValue ){databasePassword = theValue;}
	[[nodiscard]] QString getDatabasePassword() const{return databasePassword;}

	void setPanoseMatchTreshold ( int theValue );
	[[nodiscard]] int getPanoseMatchTreshold() const;

	[[nodiscard]] double getPreviewInfoFontSize() const{return previewInfoFontSize;}

	[[nodiscard]] QDir getOwnDir() const{return ownDir;}
	[[nodiscard]] QDir getConfigDir() const{return configDir;}

	[[nodiscard]] QString getSysTagName() const { return m_sysTagName; }

	[[nodiscard]] double getDpiX() const {return m_dpiX;}
	[[nodiscard]] double getDpiY() const {return m_dpiY;}
	
	
protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent ( QDropEvent * event ) override;
	void showEvent ( QShowEvent * event ) override;



	friend class Systray; // a bit ugly but i'll need access to privates
};

class LazyInit : public QThread
{
	Q_OBJECT
public:
	void run() override;
Q_SIGNALS:
	void endOfRun();
};


#endif
