//
// C++ Implementation: fmpaths
//
// Description:
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fmpaths.h"
#include "typotek.h"

#include <QApplication>
#include <QStandardPaths>

#include <array>
#include <span>

FMPaths * FMPaths::getThis()
{
	// Static-local initialisation is thread-safe under C++11 and later.
	static FMPaths inst;
	return &inst;
}

QString FMPaths::TranslationsDir()
{
	const QString cached = getThis()->FMPathsDB.value(QStringLiteral("TranslationsDir"));
	if (!cached.isEmpty())
		return cached;

	const QString dirsep(QDir::separator());
	QString dir;
#ifdef PLATFORM_APPLE
	dir = QApplication::applicationDirPath()
	      + dirsep + QStringLiteral("..") + dirsep + QStringLiteral("Resources")
	      + dirsep + QStringLiteral("Locales") + dirsep;
#elif defined(_WIN32)
	dir = QApplication::applicationDirPath()
	      + dirsep + QStringLiteral("share") + dirsep + QStringLiteral("qm") + dirsep;
#else
	dir = QApplication::applicationDirPath()
	      + dirsep + QStringLiteral("..") + dirsep + QStringLiteral("share")
	      + dirsep + QStringLiteral("fontmatrix") + dirsep + QStringLiteral("qm") + dirsep;
#endif
	getThis()->FMPathsDB[QStringLiteral("TranslationsDir")] = dir;
	return dir;
}


QString FMPaths::HelpDir()
{
	const QString cached = getThis()->FMPathsDB.value(QStringLiteral("HelpDir"));
	if (!cached.isEmpty())
		return cached;

	const QString dirsep(QDir::separator());
	QString hf;
#ifdef PLATFORM_APPLE
	hf = LocalizedDirPath(QApplication::applicationDirPath() + dirsep + QStringLiteral("help") + dirsep);
#elif defined(_WIN32)
	hf = LocalizedDirPath(QApplication::applicationDirPath() + dirsep + QStringLiteral("help") + dirsep);
#else
	hf = LocalizedDirPath(QApplication::applicationDirPath()
	                      + dirsep + QStringLiteral("..") + dirsep + QStringLiteral("share")
	                      + dirsep + QStringLiteral("fontmatrix") + dirsep + QStringLiteral("help") + dirsep);
#endif
	getThis()->FMPathsDB[QStringLiteral("HelpDir")] = hf;
	return hf;
}

QString FMPaths::ResourcesDir()
{
	const QString cached = getThis()->FMPathsDB.value(QStringLiteral("ResourcesDir"));
	if (!cached.isEmpty())
		return cached;

	const QString dirsep(QDir::separator());
	QString dir;
#ifdef PLATFORM_APPLE
	dir = QApplication::applicationDirPath()
	      + dirsep + QStringLiteral("..") + dirsep + QStringLiteral("Resources") + dirsep;
#elif defined(_WIN32)
	dir = QApplication::applicationDirPath()
	      + dirsep + QStringLiteral("share") + dirsep + QStringLiteral("resources") + dirsep;
#else
	// Relative to the executable so the app works both installed and from
	// a build directory (build/bin/fontmatrix → build/share/fontmatrix/resources/).
	dir = QApplication::applicationDirPath()
	      + dirsep + QStringLiteral("..") + dirsep + QStringLiteral("share")
	      + dirsep + QStringLiteral("fontmatrix") + dirsep + QStringLiteral("resources") + dirsep;
#endif
	getThis()->FMPathsDB[QStringLiteral("ResourcesDir")] = dir;
	return dir;
}

QString FMPaths::SamplesDir()
{
	const QString sep(QDir::separator());
	return typotek::getInstance()->getOwnDir().absolutePath() + sep + QStringLiteral("Samples") + sep;
}

QString FMPaths::FiltersDir()
{
	const QString sep(QDir::separator());
	QString dir = typotek::getInstance()->getConfigDir().absolutePath() + sep + QStringLiteral("Filters") + sep;
	QDir().mkpath(dir);
	return dir;
}

QString FMPaths::LocalizedDirPath(const QString & base, const QString& fallback)
{
	const QString sep(QStringLiteral("_"));
	const QStringList l_c(QLocale::system().name().split(sep));
	const QString langcode(l_c.first());
	const QString countrycode(l_c.last());

	std::array<QString, 4> candidates;
	int count = 0;
	if (!langcode.isEmpty() || !countrycode.isEmpty())
	{
		candidates[count++] = base + langcode + sep + countrycode;
		candidates[count++] = base + langcode;
	}
	candidates[count++] = base + fallback;
	candidates[count++] = base;

	for (const QString& candidate : std::span(candidates.data(), count))
	{
		QDir d(candidate);
		if (d.exists())
			return d.absolutePath() + QString(QDir::separator());
	}

	return QString();
}

QString FMPaths::LocalizedFilePath(const QString & base, const QString & ext, const QString& fallback)
{
	const QString sep(QStringLiteral("_"));
	const QStringList l_c(QLocale::system().name().split(sep));
	const QString langcode(l_c.first());
	const QString countrycode(l_c.last());

	std::array<QString, 4> candidates;
	int count = 0;
	if (!langcode.isEmpty() || !countrycode.isEmpty())
	{
		candidates[count++] = base + langcode + sep + countrycode + ext;
		candidates[count++] = base + langcode + ext;
	}
	candidates[count++] = base + fallback + ext;
	candidates[count++] = base + ext;

	for (const QString& candidate : std::span(candidates.data(), count))
	{
		if (QFile::exists(candidate))
			return candidate;
	}

	return QString();
}
