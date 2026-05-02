//
// C++ Implementation: prefspaneldialog
//
// Description:
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "prefspaneldialog.h"
#include "typotek.h"
#include "shortcuts.h"
#include "hyphenate/fmhyphenator.h"
#include "fmpaths.h"

#include <KLocalizedString>
#include <KMessageWidget>
#include <KPageWidgetItem>
#include <QAction>
#include <QGridLayout>
#include <QDebug>
#include <QToolTip>
#include "fmconfig.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDialogButtonBox>

PrefsPanelDialog::PrefsPanelDialog ( QWidget *parent )
		: KPageDialog ( parent )
{
	//get this before anything
	double pSize = typotek::getInstance()->getPreviewSize();

	setWindowTitle(i18nc("@title:window", "Preferences"));
	setFaceType(KPageDialog::List);
	setStandardButtons(QDialogButtonBox::Close);
	setModal(true);

	// Build the legacy QDialog UI on a hidden holder, then move each page widget
	// into the KPageDialog. Form widgets remain accessible via Ui::PrefsPanel.
	m_uiHolder = new QDialog(this);
	m_uiHolder->setVisible(false);
	setupUi(m_uiHolder);

	m_pageGeneral    = addPage(page,         i18n("General"));
	m_pageGeneral->setIcon(QIcon::fromTheme(QStringLiteral("preferences-other")));
	m_pageSystray    = addPage(pageSystray,  i18n("System tray"));
	m_pageSystray->setIcon(QIcon::fromTheme(QStringLiteral("preferences-system")));
	m_pageDisplay    = addPage(pageDisplay,  i18n("Display"));
	m_pageDisplay->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-display")));
	m_pageTools      = addPage(page_5,       i18n("Tools"));
	m_pageTools->setIcon(QIcon::fromTheme(QStringLiteral("applications-utilities")));
	m_pageSampleText = addPage(page_2,       i18n("Samples collection"));
	m_pageSampleText->setIcon(QIcon::fromTheme(QStringLiteral("format-text-bold")));
	m_pageFiles      = addPage(page_3,       i18n("Files && Folders"));
	m_pageFiles->setIcon(QIcon::fromTheme(QStringLiteral("folder")));
	m_pageShortcuts  = addPage(page_4,       i18n("Shortcuts"));
	m_pageShortcuts->setIcon(QIcon::fromTheme(QStringLiteral("configure-shortcuts")));

	// Inline banner shown on the System tray page when the host has no tray
	// available. Replaces a tooltip on a disabled groupbox (which most styles
	// don't render). Pushed to the top of pageSystray's grid; the existing
	// systrayFrame is bumped down a row.
	m_systrayUnavailable = new KMessageWidget(pageSystray);
	m_systrayUnavailable->setMessageType(KMessageWidget::Warning);
	m_systrayUnavailable->setIcon(QIcon::fromTheme(QStringLiteral("dialog-warning")));
	m_systrayUnavailable->setText(i18n("This desktop does not provide a system tray. The options below have no effect."));
	m_systrayUnavailable->setCloseButtonVisible(false);
	m_systrayUnavailable->setWordWrap(true);
	m_systrayUnavailable->hide();
	if (auto *grid = qobject_cast<QGridLayout *>(pageSystray->layout()))
	{
		grid->removeWidget(systrayFrame);
		grid->addWidget(m_systrayUnavailable, 0, 0);
		grid->addWidget(systrayFrame, 1, 0);
	}

	// Inline validation banner on the Samples Collection page. Surfaces the
	// previously-silent failures of addSampleName() (empty input, duplicate name).
	m_sampleNameWarning = new KMessageWidget(widget);
	m_sampleNameWarning->setMessageType(KMessageWidget::Warning);
	m_sampleNameWarning->setIcon(QIcon::fromTheme(QStringLiteral("dialog-warning")));
	m_sampleNameWarning->setCloseButtonVisible(true);
	m_sampleNameWarning->setWordWrap(true);
	m_sampleNameWarning->hide();
	if (auto *grid = qobject_cast<QGridLayout *>(widget->layout()))
		grid->addWidget(m_sampleNameWarning, 3, 0, 1, 3);

	fontEditorPath->setText ( typotek::getInstance()->fontEditorPath() );

	systrayFrame->setCheckable ( true );
	previewWord->setText ( typotek::getInstance()->word() );
	previewWord->setToolTip(i18n("You can use the following keywords to be replaced by data from fonts: <strong>&#60;name&#62;</strong> ; <strong>&#60;family&#62;</strong> ; <strong>&#60;variant&#62;</strong>"));
	previewSizeSpin->setValue ( pSize );
	previewIsRTL->setChecked ( typotek::getInstance()->getPreviewRTL() );
	previewSubtitled->setChecked ( typotek::getInstance()->getPreviewSubtitled() );
	initTagBox->setChecked ( typotek::getInstance()->initialTags() );
	showNamesBox->setChecked ( typotek::getInstance()->showImportedFonts() );
// 	familyNameScheme->setChecked ( !typotek::getInstance()->familySchemeFreetype() );

	{
		QFont chartFont(typotek::getInstance()->getChartInfoFontName());
		chartFont.setPointSize(typotek::getInstance()->getChartInfoFontSize());
		chartFontRequester->setFont(chartFont);
		chartFontRequester->setSampleText(i18nc("@info:placeholder sample text in font requester", "Aa Bb 123"));
	}

// 	qDebug()<< "ss" << FMConfig::value("SplashScreen",false).toBool();
	splashCheck->setChecked ( FMConfig::value ( QStringLiteral("SplashScreen"), true ).toBool() );

	namedSampleTextText->setText ( i18n( "Please select an item in the list or create a new one." ) );
	namedSampleTextText->setEnabled ( false );
	
	/// browser
	QStringList webBrowsers;
	webBrowsers << "Fontmatrix" << "firefox" << "konqueror"; // TODO fill in
	QString browser(typotek::getInstance()->getWebBrowser()); 
	if(!webBrowsers.contains(browser))
		webBrowsers << browser;
// 	qDebug()<<"Browsers ("<< browser <<"):"<< webBrowsers.join(" ; ");
	browserCombo->addItems(webBrowsers);
	browserCombo->setCurrentIndex(webBrowsers.indexOf(browser));
	
	browserOptions->setText(typotek::getInstance()->getWebBrowserOptions());

	doConnect();
}


PrefsPanelDialog::~PrefsPanelDialog()
{
}

void PrefsPanelDialog::initSystrayPrefs ( bool hasSystray, bool isVisible, bool hasActivateAll, bool allConfirmation, bool tagConfirmation )
{
	if ( !hasSystray )
	{
		systrayFrame->setEnabled ( false );
		m_systrayUnavailable->show();
	}
	else
	{
		m_systrayUnavailable->hide();
	}
	systrayFrame->setChecked ( isVisible );
	activateAllFrame->setChecked ( hasActivateAll );
	activateAllConfirmation->setChecked ( allConfirmation );
	tagsConfirmation->setChecked ( tagConfirmation );
	closeToSystray->setChecked ( FMConfig::value ( QStringLiteral("Systray/CloseToTray"), true ).toBool() );
	startToSystemTray->setChecked ( FMConfig::value ( QStringLiteral("Systray/StartToTray"), false ).toBool() );
	previewSizeSpin->setValue ( typotek::getInstance()->getPreviewSize() );

}

void PrefsPanelDialog::initSampleTextPrefs()
{
	//At least fill the sampletext list :)
	sampleTextNamesList->addItems ( typotek::getInstance()->namedSamplesNames().value(QString("User")) );
	fontSizeSpin->setValue ( FMConfig::value ( QStringLiteral("Sample/FontSize"), 14.0 ).toDouble() );
	interLineSpin->setValue ( FMConfig::value ( QStringLiteral("Sample/Interline"), 18.0 ).toDouble() );
	dictEdit->setText ( FMConfig::value ( QStringLiteral("Sample/HyphenationDict"), QLatin1String("") ).toString() );
	leftBox->setValue ( FMConfig::value ( QStringLiteral("Sample/HyphLeft"), 2 ).toInt() );
	rightBox->setValue ( FMConfig::value ( QStringLiteral("Sample/HyphRight"), 3 ).toInt() );
}

void PrefsPanelDialog::initFilesAndFolders()
{
	templatesFolder->setText ( typotek::getInstance()->getTemplatesDir() );
	QStringList remoteDirV ( FMConfig::value ( QStringLiteral("RemoteDirectories") ).toStringList() );
	remoteDirList->addItems ( remoteDirV );
	localStorageLine->setText ( typotek::getInstance()->remoteTmpDir() );

}

void PrefsPanelDialog::initShortcuts()
{
	Part0 = "";
	Part1 = "";
	Part2 = "";
	Part3 = "";
	keyCode = 0;

	shortcutLabel->setText ( "" );

	shortcutModel = new QStandardItemModel ( 0, 3, this );
	reloadShortcuts();
	shortcutList->setModel ( shortcutModel );
	shortcutList->setShowGrid ( false );
	shortcutList->setSelectionBehavior ( QAbstractItemView::SelectRows );
	shortcutList->setSelectionMode ( QAbstractItemView::SingleSelection );
	shortcutList->resizeColumnsToContents();
	shortcutList->resizeRowsToContents();
	shortcutList->setSortingEnabled ( true );
	shortcutList->sortByColumn ( 0, Qt::AscendingOrder );
}


void PrefsPanelDialog::doConnect()
{
	connect ( commitSample,SIGNAL ( clicked() ),this,SLOT ( validateSampleName() ) );
	connect ( addSampleTextNameButton,SIGNAL ( released() ),this,SLOT ( addSampleName() ) );
	connect ( newSampleTextNameText,SIGNAL ( editingFinished() ),this,SLOT ( addSampleName() ) );
	connect ( deleteSampleTextNameButton,SIGNAL ( released() ),this,SLOT ( deleteSampleName() ) );
	connect ( sampleTextNamesList,SIGNAL ( currentTextChanged ( const QString& ) ),this,SLOT ( displayNamedText() ) );
	connect ( dictButton, SIGNAL ( clicked() ), this, SLOT ( slotDictDialog() ) );
// 	connect ( applySampleTextButton,SIGNAL ( released() ),this,SLOT ( applySampleText() ) );

	connect ( systrayFrame, SIGNAL ( clicked ( bool ) ), this, SLOT ( setSystrayVisible ( bool ) ) );
	connect ( activateAllFrame, SIGNAL ( clicked ( bool ) ), this, SLOT ( setSystrayActivateAll ( bool ) ) );
	connect ( activateAllConfirmation, SIGNAL ( clicked ( bool ) ), this, SLOT ( setSystrayAllConfirmation ( bool ) ) );
	connect ( tagsConfirmation, SIGNAL ( clicked ( bool ) ), this, SLOT ( setSystrayTagsConfirmation ( bool ) ) );
	connect ( closeToSystray, SIGNAL ( clicked ( bool ) ), typotek::getInstance(), SLOT ( slotCloseToSystray ( bool ) ) );
	connect ( startToSystemTray , SIGNAL ( clicked ( bool ) ), typotek::getInstance(), SLOT ( slotSystrayStart(bool) ) );

	connect ( previewWord, SIGNAL ( textChanged ( const QString ) ), this, SLOT ( updateWord ( QString ) ) );
	connect ( previewSizeSpin, SIGNAL ( valueChanged ( double ) ), this, SLOT ( updateWordSize ( double ) ) );
	connect ( previewIsRTL, SIGNAL ( stateChanged ( int ) ), this, SLOT ( updateWordRTL ( int ) ) );
	connect ( previewSubtitled, SIGNAL ( stateChanged ( int ) ), this, SLOT ( updateWordSubtitled ( int ) ) );

	connect ( chartFontRequester, SIGNAL( fontSelected( const QFont& ) ), this, SLOT( updateChartFont( const QFont& ) ) );

	connect ( fontEditorPath, SIGNAL ( textChanged ( const QString ) ), this, SLOT ( setupFontEditor ( QString ) ) );
	connect ( fontEditorBrowse, SIGNAL ( clicked() ), this, SLOT ( slotFontEditorBrowse() ) );
	
	connect(browserButton,SIGNAL(clicked( )), this, SLOT(addAndSelectWebBrowser()));
	connect(browserCombo, SIGNAL(activated( const QString& )), this, SLOT( selectWebBrowser(const QString& ) ));
	connect(browserOptions, SIGNAL(textChanged( const QString& )), this, SLOT(setupWebBrowserOptions(const QString& )));

	connect ( initTagBox, SIGNAL ( clicked ( bool ) ), typotek::getInstance(), SLOT ( slotUseInitialTags ( bool ) ) );
// 	connect ( familyNameScheme,SIGNAL ( toggled ( bool ) ),this,SLOT ( slotFamilyNotPreferred ( bool ) ) );
	connect ( splashCheck,SIGNAL ( toggled ( bool ) ),this,SLOT ( slotSplashScreen ( bool ) ) );

	connect ( templatesDirBrowse,SIGNAL ( clicked( ) ),this, SLOT ( slotTemplatesBrowse() ) );
	connect ( templatesFolder,SIGNAL ( textChanged ( const QString& ) ),this,SLOT ( setupTemplates ( const QString& ) ) );

	connect ( remoteDirAdd,SIGNAL ( clicked() ),this,SLOT ( slotAddRemote() ) );
	connect ( remoteDirRemove,SIGNAL ( clicked() ),this,SLOT ( slotRemoveRemote() ) );
	connect ( localStorageLine,SIGNAL ( textChanged ( const QString& ) ),this,SLOT ( slotSetLocalStorage ( QString ) ) );
	connect ( localStorageButton,SIGNAL ( clicked( ) ),this,SLOT ( slotBrowseLocalStorage() ) );

	connect ( showNamesBox, SIGNAL ( stateChanged ( int ) ), this, SLOT ( slotShowImportedFonts ( int ) ) );

	connect ( clearButton, SIGNAL ( clicked() ), this, SLOT ( slotClearShortcut() ) );
	connect ( changeButton, SIGNAL ( clicked() ), this, SLOT ( slotChangeShortcut() ) );
	connect ( shortcutList, SIGNAL ( clicked ( const QModelIndex& ) ), this, SLOT ( slotActionSelected ( const QModelIndex& ) ) );
	// connect ( shortcutList, SIGNAL ( activated ( const QModelIndex& ) ), changeButton, SLOT ( toggle() ) );
}

void PrefsPanelDialog::applySampleText()
{
	typotek::getInstance()->changeFontSizeSettings ( fontSizeSpin->value(), interLineSpin->value() );
	typotek::getInstance()->forwardUpdateView();
	FMHyphenator *hyphenator = typotek::getInstance()->getHyphenator();
	if ( hyphenator->loadDict ( dictEdit->text(), leftBox->value(), rightBox->value() ) )
	{
		FMConfig::setValue ( QStringLiteral("Sample/HyphenationDict"), dictEdit->text() );
		FMConfig::setValue ( QStringLiteral("Sample/HyphLeft"), leftBox->value() );
		FMConfig::setValue ( QStringLiteral("Sample/HyphRight"), rightBox->value() );

	}
	else   // use the previous values
	{
		dictEdit->setText ( FMConfig::value ( QStringLiteral("Sample/HyphenationDict"), QLatin1String("") ).toString() );
		leftBox->setValue ( FMConfig::value ( QStringLiteral("Sample/HyphLeft"), 2 ).toInt() );
		rightBox->setValue ( FMConfig::value ( QStringLiteral("Sample/HyphRight"), 3 ).toInt() );
		FMConfig::setValue ( QStringLiteral("Sample/HyphenationDict"), QLatin1String("") );
		FMConfig::setValue ( QStringLiteral("Sample/HyphLeft"), 2 );
		FMConfig::setValue ( QStringLiteral("Sample/HyphRight"), 3 );
	}
}

void PrefsPanelDialog::addSampleName()
{
	QString n = newSampleTextNameText->text();
	if ( n.isEmpty() )
	{
		m_sampleNameWarning->setText(i18nc("@info:status validation", "Please enter a name for the new sample."));
		m_sampleNameWarning->animatedShow();
		return;
	}
	if ( typotek::getInstance()->namedSamplesNames().contains ( n ) )
	{
		m_sampleNameWarning->setText(i18nc("@info:status validation", "A sample named \"%1\" already exists.", n));
		m_sampleNameWarning->animatedShow();
		return;
	}

	m_sampleNameWarning->animatedHide();
	typotek::getInstance()->addNamedSample ( n, i18nc("A default sample text inserted when creating a new sample", "Sample Text") );
	sampleTextNamesList->addItem ( n );
	newSampleTextNameText->clear();
// 	displayNamedText();

}

void PrefsPanelDialog::deleteSampleName()
{
	QList<QListWidgetItem *> sel ( sampleTextNamesList->selectedItems() );
	if ( sel.isEmpty() )
		return;

	QString sampleKey ( sel[0]->text() );
	QString  message ( i18nc("the name of a sample text will be append to the string", "Do you confirm that you want to remove:") + " \"%1\"" );

	if ( QMessageBox::warning ( this ,
	                            "Fontmatrix",
	                            message.arg(sampleKey),
	                            QMessageBox::Yes | QMessageBox::No,
	                            QMessageBox::No ) ==  QMessageBox::Yes )
	{
		QListWidgetItem * it(sampleTextNamesList->takeItem( sampleTextNamesList->row(sel[0]) ));
		if(it)
			delete it;
		typotek::getInstance()->removeNamedSample( sampleKey );
		qDebug()<<"Removed"<<sampleKey;
	}
	else
		qDebug()<<"Did not removed"<<sampleKey;
}

void PrefsPanelDialog::displayNamedText()
{
	namedSampleTextText->setEnabled ( true );
	QString name ( sampleTextNamesList->currentItem()->text() );
//	qDebug() << "name is "<< name;
	QString text ( typotek::getInstance()->namedSample ( QString("User::") +name ) );
//	qDebug() << "text is " << text;
	namedSampleTextText->setPlainText ( text );
}

void PrefsPanelDialog::validateSampleName()
{
	if(!sampleTextNamesList->currentItem())
		return;
	typotek::getInstance()->changeSample ( sampleTextNamesList->currentItem()->text(), namedSampleTextText->toPlainText() );
}



void PrefsPanelDialog::setSystrayVisible ( bool isVisible )
{
	typotek::getInstance()->setSystrayVisible ( isVisible );
}

void PrefsPanelDialog::setSystrayActivateAll ( bool isVisible )
{
	typotek::getInstance()->showActivateAllSystray ( isVisible );
}

void PrefsPanelDialog::setSystrayAllConfirmation ( bool isEnabled )
{
	typotek::getInstance()->systrayAllConfirmation ( isEnabled );
}

void PrefsPanelDialog::setSystrayTagsConfirmation ( bool isEnabled )
{
	typotek::getInstance()->systrayTagsConfirmation ( isEnabled );
}

void PrefsPanelDialog::updateWord ( QString s )
{
	typotek::getInstance()->setPreviewSize ( previewSizeSpin->value() );
	typotek::getInstance()->setWord ( s, true );
}

void PrefsPanelDialog::updateWordSize ( double d )
{

	FMConfig::setValue ( QStringLiteral("Preview/Size"), d );
	typotek::getInstance()->setPreviewSize ( d );
	typotek::getInstance()->setWord ( previewWord->text(), true );
}


void PrefsPanelDialog::updateWordRTL ( int rtl )
{
	bool booleanState = ( rtl == Qt::Checked ) ? true : false;
	FMConfig::setValue ( QStringLiteral("Preview/RTL"), booleanState );
	typotek::getInstance()->setPreviewRTL ( booleanState );
}

void PrefsPanelDialog::updateWordSubtitled(int sub )
{
	bool booleanState = ( sub == Qt::Checked ) ? true : false;
	FMConfig::setValue ( QStringLiteral("Preview/Subtitled"), booleanState );
	typotek::getInstance()->setPreviewSubtitled ( booleanState );
}

void PrefsPanelDialog::setupFontEditor ( QString s )
{
	typotek::getInstance()->setFontEditorPath ( s );
}

void PrefsPanelDialog::slotFontEditorBrowse()
{
	QString s = QFileDialog::getOpenFileName ( this, i18n( "Select font editor" ) );
	if ( !s.isEmpty() )
	{
		fontEditorPath->setText ( s );
	}
}

void PrefsPanelDialog::addAndSelectWebBrowser()
{
	QString s = QFileDialog::getOpenFileName ( this, i18n( "Select web browser" ) );
	if ( !s.isEmpty() )
	{
		QStringList l;
		for(int i(0); i < browserCombo->count(); i++)
		{
			l << browserCombo->itemText(i);
		}
		if(!l.contains(s))
		{
			browserCombo->addItem(s);
			browserCombo->setCurrentIndex(browserCombo->count() - 1);
		}
		else
		{
			browserCombo->setCurrentIndex(l.indexOf(s));
		}
		selectWebBrowser(s);
	}
}

void PrefsPanelDialog::selectWebBrowser(const QString & text)
{
	FMConfig::setValue(QStringLiteral("Info/Browser"), text);
	typotek::getInstance()->setWebBrowser(text);
}

void PrefsPanelDialog::setupWebBrowserOptions(const QString & text)
{
	FMConfig::setValue(QStringLiteral("Info/BrowserOptions"), text);
	typotek::getInstance()->setWebBrowserOptions(text);
}

void PrefsPanelDialog::showPage ( PAGE page )
{
	KPageWidgetItem *target = nullptr;
	switch (page)
	{
		case PAGE_GENERAL:    target = m_pageGeneral; break;
		case PAGE_SYSTRAY:    target = m_pageSystray; break;
		case PAGE_DISPLAY:    target = m_pageDisplay; break;
		case PAGE_SERVICES:   target = m_pageTools; break;
		case PAGE_SAMPLETEXT: target = m_pageSampleText; break;
		case PAGE_FILES:      target = m_pageFiles; break;
		case PAGE_SHORTCUTS:  target = m_pageShortcuts; break;
	}
	if (target)
		setCurrentPage(target);
}

void PrefsPanelDialog::slotTemplatesBrowse()
{
	QString s = QFileDialog::getExistingDirectory ( this, i18n( "Select Templates Folder" ), QDir::homePath(), QFileDialog::ShowDirsOnly );
	if ( !s.isEmpty() )
	{
		templatesFolder->setText ( s );
	}
}

void PrefsPanelDialog::setupTemplates ( const QString &tdir )
{
	if ( !tdir.isEmpty() )
		typotek::getInstance()->setTemplatesDir ( tdir );
}

void PrefsPanelDialog::slotAddRemote()
{
	QString rem ( newUrlText->text() );
	remoteDirList->addItem ( rem );
	QList<QVariant> tmpL ( FMConfig::value ( QStringLiteral("RemoteDirectories") ).toList() );
	tmpL << rem;
	FMConfig::setValue ( QStringLiteral("RemoteDirectories"), tmpL );
	newUrlText->clear();
}

void PrefsPanelDialog::slotRemoveRemote()
{
	if ( remoteDirList->currentItem() )
	{
		QString url ( remoteDirList->currentItem()->text() );
		qDebug() <<"about to remove "<< url;
		for ( int i ( 0 );i < remoteDirList->count();++i )
		{
			if ( remoteDirList->item ( i )->text() == url )
				remoteDirList->takeItem ( i );
		}
		QStringList tmpL ( FMConfig::value ( QStringLiteral("RemoteDirectories") ).toStringList() );
		QStringList remoteDirStrings;
		for (const auto& s : tmpL)
		{
			if ( s != url )
				remoteDirStrings << s;
			else
				qDebug() << "Exclude "<<url<< " from remote dirs";
		}
		qDebug() <<"RemoteDirectories : "<<remoteDirStrings.join ( ", " );
		FMConfig::setValue ( QStringLiteral("RemoteDirectories"), remoteDirStrings );

	}
}

void PrefsPanelDialog::slotSetLocalStorage ( QString s )
{
	typotek::getInstance()->setRemoteTmpDir ( s );
}

void PrefsPanelDialog::slotBrowseLocalStorage()
{
	QString s = QFileDialog::getExistingDirectory ( this, i18n( "Select Where remote font files will be stored" ) );
	if ( !s.isEmpty() )
	{
		localStorageLine->setText ( s );
	}
}

void PrefsPanelDialog::slotShowImportedFonts ( int show )
{
	int opposite = Qt::Unchecked;
	if ( show == Qt::Unchecked )
		opposite = Qt::Checked;
	typotek::getInstance()->showImportedFonts ( opposite );
}

void PrefsPanelDialog::slotChangeShortcut()
{
	if ( changeButton->isChecked() )
	{
		keyCode = 0;
		Part0 = "";
		Part1 = "";
		Part2 = "";
		Part3 = "";
		Part4 = "";
		grabKeyboard();
	}
	else
		releaseKeyboard();
}

void PrefsPanelDialog::slotClearShortcut()
{
	QModelIndex index = shortcutList->currentIndex();
	if ( !index.isValid() )
		return;

	int row = index.row();
	QStandardItem *item = shortcutModel->item ( row, 0 );
	QString iText = item->text();

	Shortcuts::getInstance()->clearShortcut ( iText );
	shortcutLabel->clear();
	reloadShortcuts();
	setSelected ( iText );
}

void PrefsPanelDialog::slotActionSelected ( const QModelIndex & )
{
	QModelIndex index = shortcutList->currentIndex();
	if ( !index.isValid() )
		return;

	int row = index.row();
	QStandardItem *item = shortcutModel->item ( row, 1 );
	QString iShortcut = item->text();
	shortcutLabel->setText ( iShortcut );
}


bool PrefsPanelDialog::event ( QEvent* ev )
{
	bool ret = QWidget::event ( ev );
	if ( ev->type() == QEvent::KeyPress )
		keyPressEvent ( ( QKeyEvent* ) ev );
	if ( ev->type() == QEvent::KeyRelease )
		keyReleaseEvent ( ( QKeyEvent* ) ev );
	return ret;
}

void PrefsPanelDialog::keyPressEvent ( QKeyEvent *k )
{
	if ( changeButton->isChecked() )
	{
		QStringList tl;
		if ( !shortcutLabel->text().isEmpty() )
		{
			tl = shortcutLabel->text().split ( "+", Qt::SkipEmptyParts );
			Part4 = tl[tl.count()-1];
			if ( Part4 == i18n( "Alt" ) || Part4 == i18n( "Ctrl" ) || Part4 == i18n( "Shift" ) || Part4 == i18n( "Meta" ) )
				Part4 = "";
		}
		else
			Part4 = "";
		switch ( k->key() )
		{
			case Qt::Key_Meta:
				Part0 = i18n( "Meta+" );
				keyCode |= Qt::META;
				break;
			case Qt::Key_Shift:
				Part3 = i18n( "Shift+" );
				keyCode |= Qt::SHIFT;
				break;
			case Qt::Key_Alt:
				Part2 = i18n( "Alt+" );
				keyCode |= Qt::ALT;
				break;
			case Qt::Key_Control:
				Part1 = i18n( "Ctrl+" );
				keyCode |= Qt::CTRL;
				break;
			default:
				keyCode |= k->key();
				shortcutLabel->setText ( getKeyText ( keyCode ) );
				changeButton->setChecked ( false );
				releaseKeyboard();
				shortcutSet ( shortcutLabel->text() );
		}
	}
	if ( changeButton->isChecked() )
	{
		shortcutLabel->setText ( Part0+Part1+Part2+Part3+Part4 );
	}
}

void PrefsPanelDialog::keyReleaseEvent ( QKeyEvent *k )
{
	if ( changeButton->isChecked() )
	{
		if ( !shortcutLabel->text().isEmpty() )
		{
			QStringList tl;
			tl = shortcutLabel->text().split ( "+", Qt::SkipEmptyParts );
			Part4 = tl[tl.count()-1];
			if ( Part4 == i18n( "Alt" ) || Part4 == i18n( "Ctrl" ) || Part4 == i18n( "Shift" ) || Part4 == i18n( "Meta" ) )
				Part4 = "";
		}
		else
			Part4 = "";
		if ( k->key() == Qt::Key_Meta )
		{
			Part0 = "";
			keyCode &= ~Qt::META;
		}
		if ( k->key() == Qt::Key_Shift )
		{
			Part3 = "";
			keyCode &= ~Qt::SHIFT;
		}
		if ( k->key() == Qt::Key_Alt )
		{
			Part2 = "";
			keyCode &= ~Qt::ALT;
		}
		if ( k->key() == Qt::Key_Control )
		{
			Part1 = "";
			keyCode &= ~Qt::CTRL;
		}
		shortcutLabel->setText ( Part0+Part1+Part2+Part3+Part4 );
	}
}

QString PrefsPanelDialog::getKeyText ( int KeyC )
{
	if ( ( KeyC & ~ ( Qt::META | Qt::CTRL | Qt::ALT | Qt::SHIFT ) ) == 0 )
		return "";
	// on OSX Qt translates modifiers to forsaken symbols, arrows and the like
	// we prefer plain English
	QString res;
	if ( ( KeyC & Qt::META ) != 0 )
		res += "Meta+";
	if ( ( KeyC & Qt::CTRL ) != 0 )
		res += "Ctrl+";
	if ( ( KeyC & Qt::ALT ) != 0 )
		res += "Alt+";
	if ( ( KeyC & Qt::SHIFT ) != 0 )
		res += "Shift+";
	return res + QString ( QKeySequence ( KeyC & ~ ( Qt::META | Qt::CTRL | Qt::ALT | Qt::SHIFT ) ).toString() );
}

void PrefsPanelDialog::shortcutSet ( const QString &shortcut )
{
	QModelIndex index = shortcutList->currentIndex();
	if ( !index.isValid() )
		return;

	int row = index.row();
	QStandardItem *item = shortcutModel->item ( row, 0 );
	QString iText = item->text();

	Shortcuts *tmp = Shortcuts::getInstance();
	QString reserved = tmp->isReserved ( shortcut, iText );
	if ( !reserved.isEmpty() ) // shortcut is already in use
	{
		if ( QMessageBox::question ( this, i18n( "Replace" ),
		                             "<qt>" + i18nc("action name will be appended to this", "Shortcut is already in use for") +
		                             QString ( "<br/><b>%1</b>.<br/>" ).arg ( reserved ) +
		                             i18n( "Do you still want to assign it?" ) + "</qt>",
		                             QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
		{
			tmp->clearShortcut ( reserved );
		}
		else
		{
			return; // user choose not to replace an existing shortcut
		}
	}
	tmp->setShortcut ( shortcut, iText );
	reloadShortcuts();
	setSelected ( iText );
}

void PrefsPanelDialog::reloadShortcuts()
{
	QFont font ( shortcutList->font() );
	font.setBold ( true );
	shortcutModel->clear();
	QList<QAction*> alist = Shortcuts::getInstance()->getActions();
	Shortcuts *scuts = Shortcuts::getInstance();
	for (auto* act : alist)
	{
		QStandardItem *iText = new QStandardItem ( scuts->cleanName(act->text()) );
		QStandardItem *iShortcut = new QStandardItem ( act->shortcut().toString() );
		iShortcut->setFont ( font );
		QString tooltip = act->toolTip();
		QString statusTip = act->statusTip();
		QString tip = tooltip;
		if ( statusTip.length() > tooltip.length() )
			tip = statusTip;
		QStandardItem *iTooltip = new QStandardItem ( tip );
		QList<QStandardItem *> iRow;
		iRow << iText << iShortcut << iTooltip;
		shortcutModel->appendRow ( iRow );
	}
	shortcutModel->setHeaderData ( 0, Qt::Horizontal, i18n( "Action" ) );
	shortcutModel->setHeaderData ( 1, Qt::Horizontal, i18n( "Shortcut" ) );
	shortcutModel->setHeaderData ( 2, Qt::Horizontal, i18n( "Tip" ) );
	shortcutList->resizeColumnsToContents();
	shortcutList->resizeRowsToContents();
	shortcutList->setSortingEnabled ( true );
}

void PrefsPanelDialog::setSelected ( const QString &actionText )
{
	QList<QStandardItem*> ilist = shortcutModel->findItems ( Shortcuts::getInstance()->cleanName(actionText) );
	if ( ilist.count() > 0 )
	{
		int row = ilist.at ( 0 )->row();
		shortcutList->selectRow ( row );
	}
}


// void PrefsPanelDialog::slotFamilyNotPreferred ( bool state )
// {
// 	qDebug() <<"slotFamilyNotPreferred("<< state <<")";
// 	QSettings settings;
// 	settings.setValue ( "FamilyPreferred", !state );
// 	typotek::getInstance()->setFamilySchemeFreetype ( !state );
// }

void PrefsPanelDialog::slotSplashScreen ( bool state )
{
// 	qDebug() <<"slotSplashScreen("<< state <<")";
	FMConfig::setValue ( QStringLiteral("SplashScreen"), state );
}

void PrefsPanelDialog::slotDictDialog()
{
	QString s = QFileDialog::getOpenFileName ( this, i18n( "Select hyphenation dictionary" ), QDir::homePath() );
	if ( !s.isEmpty() )
		dictEdit->setText ( s );
}

void PrefsPanelDialog::done(int r)
{
	applySampleText();
	KPageDialog::done(r);
}

void PrefsPanelDialog::updateChartFont(const QFont & font)
{
	// Chart subtitle consumers (fontitem.cpp) read family + size only;
	// style attributes from the requester are intentionally not persisted.
	const int size = font.pointSize() > 0 ? font.pointSize() : qRound(font.pointSizeF());

	FMConfig::setValue(QStringLiteral("ChartInfoFontFamily"), font.family());
	FMConfig::setValue(QStringLiteral("ChartInfoFontSize"), size);

	typotek::getInstance()->setChartInfoFontName(font.family());
	typotek::getInstance()->setChartInfoFontSize(size);
}






