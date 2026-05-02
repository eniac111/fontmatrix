/***************************************************************************
 *   Copyright (C) 2010 by Pierre Marchand   *
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

#include "fmvariants.h"

#include "fontitem.h"

FMVariants * FMVariants::instance = nullptr;

FMVariants::FMVariants()
{
	// init sorted variant names
	variants.clear();
	QStringList weight;
	weight << QString("Hairline")
			<<	QString("Thin")
			<<	QString("UltraLight")
			<<	QString("ExtraLight")
			<<	QString("Light")
			<<	QString("Book")
			<<	QString("Normal")
			<<	QString("Regular")
			<<	QString("Roman")
			<<	QString("Plain")
			<<	QString("Medium")
			<<	QString()
			<<	QString("Demi")
			<<	QString("DemiBold")
			<<	QString("SemiBold")
			<<	QString("Bold")
			<<	QString("ExtraBold")
			<<	QString("Extra")
			<<	QString("Heavy")
			<<	QString("Black")
			<<	QString("ExtraBlack")
			<<	QString("UltraBlack")
			<<	QString("Ultra");

	QStringList slope;
	slope << QString()
			<< QString("Italic")
			<< QString("Oblique")
			<< QString("Slanted");

	QStringList width;
	width << QString()
			<< QString("UltraCompressed")
			<<	QString("Compressed")
			<<	QString("UltraCondensed")
			<<	QString("Condensed")
			<<	QString("SemiCondensed")
			<<	QString("Narrow")
			<<	QString("SemiExtended")
			<<	QString("SemiExpanded")
			<<	QString("Extended")
			<<	QString("Expanded")
			<<	QString("ExtraExtended")
			<<	QString("ExtraExpanded");

	QStringList  optical;
	optical << QString()
			<< QString("Poster")
			<<	QString("Display")
			<<	QString("SubHead")
			<<	QString("SmallText")
			<<	QString("Caption");

	for (const auto& w : weight)
	{
		for (const auto& s : slope)
		{
			for (const auto& wi : width)
			{
				for (const auto& o : optical)
				{
					appendVariants(w, s, wi, o);
				}
			}
		}
	}
//	for (const auto& v : variants)
//		qDebug()<<v.join(" ");

	priorList <<	QString("Book")
			<<	QString("Normal")
			<<	QString("Regular")
			<<	QString("Roman")
			<<	QString("Plain")
			<<	QString("Medium");
}



void FMVariants::appendVariants(const QString &w, const QString &s, const QString &wi, const QString &o)
{
	QStringList p;
	p << w << s << wi << o;
	QStringList l;
	for (const auto& s : p)
	{
		if(!s.isEmpty())
			l << s;
	}
	variants << l;
}

bool FMVariants::compareVariants(const QStringList &a, const QStringList &b)
{
	for (const auto& va : a)
	{
		if(!b.contains(va, Qt::CaseInsensitive))
			return false;
	}
	for (const auto& vb : b)
	{
		if(!a.contains(vb, Qt::CaseInsensitive))
			return false;
	}
	return true;
}

QList<FontItem*> FMVariants::Order(QList<FontItem*> ul)
{
	if(instance == nullptr)
		instance = new FMVariants;

	FMVariants *vs(instance);

	QList<FontItem*> ret;
	QMap<FontItem*, QStringList> fl;
	for (auto* f : ul)
	{
		fl.insert(f, f->variant().split(QString(" ")));
	}
	for (const auto& v : vs->variants)
	{
		for (auto* f : fl.keys())
		{
			if(vs->compareVariants(v,fl[f]))
			{
				ret.append(f);
				fl.remove(f);
			}
		}
	}
	if(fl.count() > 0)
	{
		// for Univers-like fonts, we get the number key
		QMap<int, QMap<QString,FontItem*> > ulikeFonts;
		bool intOK(false);
		for (auto* f : fl.keys())
		{
			intOK = false;
			QString fs(fl[f].first());
			int idx(fs.toInt(&intOK, 10));
			if(intOK)
			{
				ulikeFonts[idx][f->variant()] = f;
				fl.remove(f);
			}
		}
		for (const auto& k : ulikeFonts.keys())
		{
			for (const auto& v : ulikeFonts[k].keys())
				ret << ulikeFonts[k][v];
		}

		// still fonts unsorted;
		if(fl.count() > 0)
		{
			QMap<QString, FontItem*> lastChance;
			for (auto* f : fl.keys())
			{
				lastChance[f->variant()] = f;
			}
			for (const auto& v : lastChance.keys())
				ret << lastChance[v];
		}
	}
	return ret;
}


FontItem * FMVariants::Preferred(QList<FontItem *> ul)
{
	if(ul.isEmpty())
		return nullptr;
	if(instance == nullptr)
		instance = new FMVariants;
	for (auto* it : ul)
	{
		if(instance->priorList.contains(it->variant(), Qt::CaseInsensitive))
		{
			return it;
		}
	}
	return ul.first();
};
