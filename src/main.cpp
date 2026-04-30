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


#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QSplashScreen>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QDebug>
#include <QElapsedTimer>
#include <QLocale>
#include <QThread>
#include <QSettings>

#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>

#include "typotek.h"
#include "mainviewwidget.h"
#include "fmconfig.h"
#include "fmpaths.h"
#include "systray.h"


bool __FM_SHOW_FONTLOADED;

/**
 *
 * @param argc
 * @param argv[]
 * @return
 */
int main ( int argc, char *argv[] )
{
	// Must be set before QApplication so QSettings picks up the right scope.
	QCoreApplication::setOrganizationName ( "Fontmatrix" );
	QCoreApplication::setOrganizationDomain ( "io.fontmatrix" );
	QCoreApplication::setApplicationName ( "fontmatrix" );

	// On Plasma 6, kded6's kappmenu D-Bus registrar is enabled by default.
	// If no panel widget consumes the exported menu, Qt still hides the local
	// QMenuBar — leaving users with no menu anywhere. Force in-window menus.
	// macOS keeps its system menu via the platform default.
#ifndef Q_OS_MACOS
	QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
#endif

	Q_INIT_RESOURCE ( application );
	QApplication app ( argc, argv );
	app.setWindowIcon ( QIcon::fromTheme ( QStringLiteral("fontmatrix"),
	                                       QIcon ( QStringLiteral(":/fontmatrix_icon.png") ) ) );

	KLocalizedString::setApplicationDomain("fontmatrix");

	KAboutData aboutData(
	    QStringLiteral("fontmatrix"),
	    QStringLiteral("Fontmatrix"),
	    QStringLiteral("%1.%2.%3")
	        .arg(FONTMATRIX_VERSION_MAJOR)
	        .arg(FONTMATRIX_VERSION_MINOR)
	        .arg(FONTMATRIX_VERSION_PATCH)
	);
	aboutData.setShortDescription(QStringLiteral("Font management application"));
	// Long description preserved from the legacy About dialog (src/messages/about.html).
	aboutData.setOtherText(QStringLiteral(
	    "<p>Fontmatrix is aimed at adventurous graphic designers and typesetters "
	    "who need to manage hundreds and even thousands of fonts for their work — "
	    "avoiding the need to browse overly long lists in dialogs.</p>"
	    "<p>Basically, Fontmatrix helps you do three things:</p>"
	    "<ul>"
	    "<li>Activating and deactivating your fonts</li>"
	    "<li>Tagging fonts with sets</li>"
	    "<li>Finding suitable fonts by constructing simple or complex queries</li>"
	    "<li>Refining the fonts selection by comparing glyphs in detail</li>"
	    "<li>Generating font \"books\" as PDF.</li>"
	    "</ul>"));
	aboutData.setLicense(KAboutLicense::GPL_V2);
	aboutData.setCopyrightStatement(QStringLiteral("© 2007–2026 FontMatrix contributors"));
	aboutData.setHomepage(QStringLiteral("https://github.com/eniac111/fontmatrix"));
	aboutData.setBugAddress(QByteArrayLiteral("https://github.com/eniac111/fontmatrix/issues"));
	aboutData.setOrganizationDomain(QByteArrayLiteral("io.fontmatrix"));
	// Authors and contributors — preserved from the legacy "The People" tab
	// of the old About dialog (src/messages/about_people.html).
	aboutData.addAuthor(
	    QStringLiteral("Blagovest Petrov"),
	    QStringLiteral("Maintainer since 2026, Qt6 support, Flatpak and improved Windows 11 support, Bulgarian translation"),
	    QStringLiteral("blagovest@petrovs.info")
	);
	aboutData.addAuthor(
	    QStringLiteral("Pierre Marchand"),
	    QStringLiteral("Initiator of Fontmatrix"),
	    QStringLiteral("pierremarc@oep-h.com"),
	    QStringLiteral("http://oep-h.com")
	);
	aboutData.addAuthor(
	    QStringLiteral("Mr Docs"),
	    QStringLiteral("Tester, packager and documentation"),
	    QStringLiteral("mrdocs@scribus.info"),
	    QStringLiteral("http://www.scribus.net")
	);
	aboutData.addAuthor(
	    QStringLiteral("Riku Leino"),
	    QStringLiteral("Systray, minor tweaks, Finnish translation"),
	    QStringLiteral("riku@scribus.info"),
	    QStringLiteral("http://www.tsoots.fi/")
	);
	aboutData.addAuthor(
	    QStringLiteral("ParagAN"),
	    QStringLiteral("GUI enhancements, Indic fonts"),
	    QStringLiteral("paragn@fedoraproject.org"),
	    QStringLiteral("http://paragn.fedorapeople.org")
	);
	aboutData.addAuthor(
	    QStringLiteral("Alexandre Prokoudine"),
	    QStringLiteral("Usability, user manual, Russian translation, website"),
	    QStringLiteral("alexandre.prokoudine@gmail.com"),
	    QStringLiteral("http://www.libregraphicsworld.org")
	);
	aboutData.addAuthor(
	    QStringLiteral("Vladimir Savic"),
	    QStringLiteral("General visual impact, documentation"),
	    QStringLiteral("vladimir.firefly.savic@gmail.com")
	);
	aboutData.addAuthor(
	    QStringLiteral("Pavel Fric"),
	    QStringLiteral("Czech translation"),
	    QStringLiteral("pavelfric@seznam.cz")
	);
	aboutData.addCredit(
	    QStringLiteral("Fontmatrix contributors"),
	    QStringLiteral("Original project: https://github.com/fontmatrix/fontmatrix"),
	    QString(),
	    QStringLiteral("https://github.com/fontmatrix/fontmatrix")
	);
	KAboutData::setApplicationData(aboutData);

	QCommandLineParser parser;
	aboutData.setupCommandLine(&parser);
	// Use parse() instead of process() so early-exit flags are handled below
	// with return 0 rather than ::exit(), which skips Qt thread cleanup and
	// causes "QThreadStorage: entry N destroyed before end of thread" warnings.
	parser.parse(app.arguments());

	if (parser.isSet(QStringLiteral("version"))) {
		fprintf(stdout, "%s %s\n",
		        qPrintable(aboutData.displayName()),
		        qPrintable(aboutData.version()));
		return 0;
	}
	if (parser.isSet(QStringLiteral("help")) || parser.isSet(QStringLiteral("help-all"))) {
		fputs(qPrintable(parser.helpText()), stdout);
		return 0;
	}
	if (parser.isSet(QStringLiteral("author"))) {
		fprintf(stdout, "%s was written by:\n", qPrintable(aboutData.displayName()));
		for (const KAboutPerson &person : aboutData.authors()) {
			if (!person.emailAddress().isEmpty())
				fprintf(stdout, "    %s <%s>\n",
				        qPrintable(person.name()), qPrintable(person.emailAddress()));
			else
				fprintf(stdout, "    %s\n", qPrintable(person.name()));
		}
		if (!aboutData.bugAddress().isEmpty())
			fprintf(stdout, "Please report bugs to %s.\n", qPrintable(aboutData.bugAddress()));
		return 0;
	}
	// processCommandLine handles any remaining KAboutData-specific flags.
	// It is only reached on a normal GUI launch (no early-exit flags set).
	aboutData.processCommandLine(&parser);

	// One-time QSettings forward-migration from the legacy "Undertype" scope
	// (used by the original Fontmatrix releases) into the current Fontmatrix scope.
	// Runs only when the destination scope is empty. newSettings is a staging
	// area; the KConfig import below pulls everything into the live store.
	{
		QSettings newSettings;
		if ( newSettings.allKeys().isEmpty() )
		{
			QSettings oldSettings ( QSettings::defaultFormat(), QSettings::UserScope,
			                        QLatin1String ( "Undertype" ), QLatin1String ( "fontmatrix" ) );
			const QStringList keys = oldSettings.allKeys();
			for ( const QString &key : keys )
				newSettings.setValue ( key, oldSettings.value ( key ) );
			if ( !keys.isEmpty() )
				newSettings.sync();
		}
	}

	// One-time import from QSettings into KConfig on the first launch of a KConfig-enabled build.
	{
		KSharedConfig::Ptr kconf = KSharedConfig::openConfig();
		if (!kconf->group(QStringLiteral("Migration")).hasKey(QStringLiteral("QSettingsImported")))
		{
			QSettings qst;
			const QStringList allKeys = qst.allKeys();
			for (const QString &fullKey : allKeys)
			{
				const int slash = fullKey.indexOf(QLatin1Char('/'));
				const QString group = (slash != -1) ? fullKey.left(slash) : QString{};
				const QString key   = (slash != -1) ? fullKey.mid(slash + 1) : fullKey;
				kconf->group(group).writeEntry(key, qst.value(fullKey));
			}
			kconf->group(QStringLiteral("Migration")).writeEntry(QStringLiteral("QSettingsImported"), true);
			kconf->sync();
		}
	}

	// Translation routing is owned by KLocalizedString (KF6 Ki18n). Strings
	// flow through i18n() / i18nc() / i18np() and are looked up in
	// ${KDE_INSTALL_LOCALEDIR}/<lang>/LC_MESSAGES/fontmatrix.mo.
	// setApplicationDomain() is called above next to KAboutData::setApplicationData.

	if ( app.arguments().contains ( "listfonts" ) )
	{
		__FM_SHOW_FONTLOADED = true;
	}
	else
	{
		__FM_SHOW_FONTLOADED = false;
	}

	// Single-instance guard. KDBusService::Unique aborts a second
	// `fontmatrix` invocation early; the existing process receives an
	// activateRequested signal so it can raise its window. Done before
	// typotek::getInstance() so we don't pay the font-DB init cost twice
	// on a duplicate launch.
	KDBusService dbusService(KDBusService::Unique);

	typotek * mw = typotek::getInstance();

	// Bring the existing window forward when a second invocation is rejected.
	QObject::connect(&dbusService, &KDBusService::activateRequested,
	                 mw, [mw](const QStringList &/*args*/, const QString &/*workingDir*/) {
		if (mw->isMinimized())
			mw->setWindowState(mw->windowState() & ~Qt::WindowMinimized);
		mw->show();
		mw->raise();
		mw->activateWindow();
	});


	QSplashScreen theSplash;
	QPixmap theSplashPix ( ":/fontmatrix_splash.png" );
	bool splash = FMConfig::value ( QStringLiteral("SplashScreen"), true ).toBool();
	if ( app.arguments().contains ( "splash" ) || splash )
	{
		QFont spFont;
		spFont.setPointSize ( 14 );
		QPainter p ( &theSplashPix );
		p.setFont ( spFont );
		p.setPen ( Qt::white );
		QString vString ( QString ( "%1.%2.%3" )
		                  .arg ( FONTMATRIX_VERSION_MAJOR )
		                  .arg ( FONTMATRIX_VERSION_MINOR )
		                  .arg ( FONTMATRIX_VERSION_PATCH ) );
		p.drawText ( theSplashPix.width() / 4 , theSplashPix.height() / 3, vString );
		p.end();

		spFont.setPointSize ( 9 );
		theSplash.setPixmap ( theSplashPix );
		theSplash.setFont ( spFont );
		QObject::connect ( mw, SIGNAL ( relayStartingStepOut ( QString, int, QColor ) ),
		                   &theSplash, SLOT ( showMessage ( const QString&, int, const QColor& ) ),
		                   Qt::DirectConnection );
	}

	QElapsedTimer splashTimer;
	if ( splash )
	{
		theSplash.show();
		splashTimer.start();
	}

	mw->initMatrix();

	if (	( typotek::getInstance()->getSystray() )
	        && ( typotek::getInstance()->getSystray()->isVisible() )
	        && ( FMConfig::value ( QStringLiteral("Systray/CloseToTray"), true ).toBool() ) )
	{
		if ( ! FMConfig::value ( QStringLiteral("Systray/StartToTray"), false ).toBool() )
			mw->show();
		else
			mw->hide();
	}
	else
		mw->show();


	LazyInit lazyInit;
	lazyInit.start ( QThread::LowestPriority );

	mw->postInit();

	if ( splash )
	{
		const int minSplashMs = 1500;
		qint64 remaining = minSplashMs - splashTimer.elapsed();
		if ( remaining > 0 )
			QThread::msleep ( static_cast<unsigned long>( remaining ) );
		theSplash.finish ( mw );
	}

	return app.exec();
}

