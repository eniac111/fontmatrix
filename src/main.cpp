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

#include "typotek.h"
#include "mainviewwidget.h"
//#include "listdockwidget.h"
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
	QCoreApplication::setOrganizationName ( "Fontmatrix" );
	QCoreApplication::setOrganizationDomain ( "io.fontmatrix" );
	QCoreApplication::setApplicationName ( "fontmatrix" );

	Q_INIT_RESOURCE ( application );
	QApplication app ( argc, argv );
	app.setWindowIcon ( QIcon ( ":/fontmatrix_icon.png" ) );
	app.setApplicationVersion ( QString ( "%1.%2.%3" )
	                            .arg ( FONTMATRIX_VERSION_MAJOR )
	                            .arg ( FONTMATRIX_VERSION_MINOR )
	                            .arg ( FONTMATRIX_VERSION_PATCH ) );

	// Migrate QSettings from the old "Undertype" organization name.
	// Only copies if the new location is empty and the old one has data.
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


	QSettings settings;

	QSplashScreen theSplash;
	QPixmap theSplashPix ( ":/fontmatrix_splash.png" );
	bool splash = settings.value ( "SplashScreen", true ).toBool();
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
	        && ( settings.value ( "Systray/CloseToTray", true ).toBool() ) )
	{
		if ( ! settings.value ( "Systray/StartToTray", false ).toBool() )
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

