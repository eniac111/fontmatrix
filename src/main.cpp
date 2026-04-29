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
#include <QTranslator>
#include <QSettings>

#ifdef HAVE_KF6_COREADDONS
#include <KAboutData>
#include <KLocalizedString>
#endif

#ifdef HAVE_KF6_CONFIG
#include <KSharedConfig>
#include <KConfigGroup>
#endif

#include "typotek.h"
#include "mainviewwidget.h"
//#include "listdockwidget.h"
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
	QCoreApplication::setOrganizationName ( "FontMatrix-NG" );
	QCoreApplication::setOrganizationDomain ( "io.fontmatrix" );
	QCoreApplication::setApplicationName ( "fontmatrix-ng" );

	// On Plasma 6, kded6's kappmenu D-Bus registrar is enabled by default.
	// If no panel widget consumes the exported menu, Qt still hides the local
	// QMenuBar — leaving users with no menu anywhere. Force in-window menus.
	// macOS keeps its system menu via the platform default.
#ifndef Q_OS_MACOS
	QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
#endif

	Q_INIT_RESOURCE ( application );
	QApplication app ( argc, argv );
	app.setWindowIcon ( QIcon ( ":/fontmatrix_icon.png" ) );

#ifdef HAVE_KF6_COREADDONS
	KLocalizedString::setApplicationDomain("fontmatrix-ng");

	KAboutData aboutData(
	    QStringLiteral("fontmatrix-ng"),
	    QStringLiteral("FontMatrix-NG"),
	    QStringLiteral("%1.%2.%3")
	        .arg(FONTMATRIX_VERSION_MAJOR)
	        .arg(FONTMATRIX_VERSION_MINOR)
	        .arg(FONTMATRIX_VERSION_PATCH)
	);
	aboutData.setShortDescription(QStringLiteral("Font management application"));
	aboutData.setLicense(KAboutLicense::GPL_V2);
	aboutData.setCopyrightStatement(QStringLiteral("© 2007–2026 FontMatrix contributors"));
	aboutData.setHomepage(QStringLiteral("https://github.com/eniac111/fontmatrix"));
	aboutData.setBugAddress(QByteArrayLiteral("https://github.com/eniac111/fontmatrix/issues"));
	aboutData.setOrganizationDomain(QByteArrayLiteral("io.fontmatrix"));
	aboutData.addAuthor(
	    QStringLiteral("Blagovest Petrov"),
	    QStringLiteral("Current maintainer"),
	    QStringLiteral("blagovest@petrovs.info")
	);
	aboutData.addAuthor(
	    QStringLiteral("Pierre Marchand"),
	    QStringLiteral("Original author"),
	    QStringLiteral("pierremarc@oep-h.com")
	);
	aboutData.addCredit(
	    QStringLiteral("FontMatrix-NG contributors"),
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
#else
	app.setApplicationVersion ( QString ( "%1.%2.%3" )
	                            .arg ( FONTMATRIX_VERSION_MAJOR )
	                            .arg ( FONTMATRIX_VERSION_MINOR )
	                            .arg ( FONTMATRIX_VERSION_PATCH ) );
#endif

	// Migrate QSettings forward through the rename history:
	//   Undertype/fontmatrix  →  Fontmatrix/fontmatrix  →  FontMatrix-NG/fontmatrix-ng
	// Each step only runs when the destination scope is empty.
	// On Linux, newSettings is just a staging area; KConfig imports it below.
	// On Windows/macOS, newSettings IS the live store and must match FMConfig::sharedSettings() (IniFormat).
	{
#ifdef HAVE_KF6_CONFIG
		QSettings newSettings;
#else
		QSettings newSettings ( QSettings::IniFormat, QSettings::UserScope,
		                        QStringLiteral ( "FontMatrix-NG" ), QStringLiteral ( "fontmatrix-ng" ) );
#endif
		if ( newSettings.allKeys().isEmpty() )
		{
			// Try the most recent old scope first (pre-rename "Fontmatrix").
			QSettings midSettings ( QSettings::defaultFormat(), QSettings::UserScope,
			                        QLatin1String ( "Fontmatrix" ), QLatin1String ( "fontmatrix" ) );
			const QStringList midKeys = midSettings.allKeys();
			if ( !midKeys.isEmpty() )
			{
				for ( const QString &key : midKeys )
					newSettings.setValue ( key, midSettings.value ( key ) );
				newSettings.sync();
			}
			else
			{
				// Fall back to the even older "Undertype" scope.
				QSettings oldSettings ( QSettings::defaultFormat(), QSettings::UserScope,
				                        QLatin1String ( "Undertype" ), QLatin1String ( "fontmatrix" ) );
				const QStringList keys = oldSettings.allKeys();
				for ( const QString &key : keys )
					newSettings.setValue ( key, oldSettings.value ( key ) );
				if ( !keys.isEmpty() )
					newSettings.sync();
			}
		}
	}

#ifdef HAVE_KF6_CONFIG
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
#endif

	QTranslator translator;
	if ( translator.load ( FMPaths::LocalizedFilePath( FMPaths::TranslationsDir() + "fontmatrix-"  , ".qm" ) ) )
	{
		app.installTranslator ( &translator );
		qDebug() << "Translator is installed."<<FMPaths::LocalizedFilePath( FMPaths::TranslationsDir() + "fontmatrix-" , ".qm" );
	}
	else
	{
		qDebug() << "Unable to load"<< FMPaths::LocalizedFilePath( FMPaths::TranslationsDir() + "fontmatrix-" , ".qm" );
	}


	if ( app.arguments().contains ( "listfonts" ) )
	{
		__FM_SHOW_FONTLOADED = true;
	}
	else
	{
		__FM_SHOW_FONTLOADED = false;
	}

	typotek * mw = typotek::getInstance();


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
//	QObject::connect ( &lazyInit, SIGNAL ( endOfRun() ), ListDockWidget::getInstance(), SLOT ( unlockFilter() ) );
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

