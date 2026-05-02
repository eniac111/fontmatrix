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
#include "dataloader.h"
#include "fmpaths.h"

#include <QDir>
#include <QFile>
#include <QLocale>

DataLoader::DataLoader ()
{
	load();
}

void DataLoader::reload()
{
	load();
}

void DataLoader::load()
{
	sm.clear();
	pm.clear();

	// System samples — skip silently if the directory is absent (e.g. not yet installed)
	QDir samplesDir(FMPaths::ResourcesDir() + "Samples");
	if (samplesDir.exists())
	{
		for (const auto& ld : samplesDir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs))
		{
			QDir lang(samplesDir.absoluteFilePath(ld));
			QLocale locale(ld);
			QString loclang(QLocale::languageToString(locale.language()));
			for (const auto& st : lang.entryList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files))
			{
				QFile fp(lang.absoluteFilePath(st));
				if(fp.open(QIODevice::ReadOnly))
				{
					sm[loclang][st] = QString::fromUtf8(fp.readAll());
				}
			}
		}
	}

	// User samples
	QDir uDir(FMPaths::SamplesDir());
	if(!uDir.exists())
	{
		uDir.mkpath(uDir.absolutePath());
	}
	else
	{
		for (const auto& ld : uDir.entryList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files))
		{
			QFile fp(uDir.absoluteFilePath(ld));
			if(fp.open(QIODevice::ReadOnly))
			{
				pm[ld] = QString::fromUtf8(fp.readAll());
			}
		}
	}

	// Fallback — keeps the UI functional when no samples are installed
	if(sm.isEmpty() && pm.isEmpty())
	{
		sm["Emergency"]["Text"] = QString("Emergency Text");
	}
}

bool DataLoader::update(const QString& name, const QString& sample)
{
	QDir uDir(FMPaths::SamplesDir());
	QFile fp(uDir.absoluteFilePath(name));
	if(fp.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		const QByteArray utf8 = sample.toUtf8();
		if(fp.write(utf8) == utf8.size())
		{
			pm[name] = sample;
			return true;
		}
	}
	return false;
}

bool DataLoader::remove(const QString& name)
{
	QDir uDir(FMPaths::SamplesDir());
	QFile fp(uDir.absoluteFilePath(name));
	if(fp.exists())
	{
		if(fp.remove())
		{
			pm.remove(name);
			return true;
		}
	}
	return false;
}
