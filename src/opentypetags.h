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

#ifndef OTTMEAN_H
#define OTTMEAN_H

#include <KLocalizedString>
#include <QMap>


QString OTTagMeans ( QString tag )
{
	static QMap<QString,QString> OTTagsMeaning;
	if ( OTTagsMeaning.isEmpty() )
	{
		OTTagsMeaning["aalt"]= i18n("Access All Alternates");
		OTTagsMeaning["abvf"]= i18n("Above-Base Forms");
		OTTagsMeaning["abvm"]= i18n("Above-Base Mark Positioning");
		OTTagsMeaning["abvs"]= i18n("Above-Base Substitutions");
		OTTagsMeaning["afrc"]= i18n("Alternative Fractions");
		OTTagsMeaning["akhn"]= i18n("Akhands");
		OTTagsMeaning["blwf"]= i18n("Below-Base Forms");
		OTTagsMeaning["blwm"]= i18n("Below-Base Mark Positioning");
		OTTagsMeaning["blws"]= i18n("Below-Base Substitutions");
		OTTagsMeaning["c2pc"]= i18n("Petite Capitals From Capitals");
		OTTagsMeaning["c2sc"]= i18n("Small Capitals From Capitals");
		OTTagsMeaning["calt"]= i18n("Contextual Alternates");
		OTTagsMeaning["case"]= i18n("Case-Sensitive Forms");
		OTTagsMeaning["ccmp"]= i18n("Glyph Composition/Decomposition");
		OTTagsMeaning["clig"]= i18n("Contextual Ligatures");
		OTTagsMeaning["cjct"]= i18n("Conjunct Forms");
		OTTagsMeaning["cpsp"]= i18n("Capital Spacing");
		OTTagsMeaning["cswh"]= i18n("Contextual Swash");
		OTTagsMeaning["curs"]= i18n("Cursive Positioning");
		OTTagsMeaning["dflt"]= i18n("Default Processing");
		OTTagsMeaning["dist"]= i18n("Distances");
		OTTagsMeaning["dlig"]= i18n("Discretionary Ligatures");
		OTTagsMeaning["dnom"]= i18n("Denominators");
		OTTagsMeaning["expt"]= i18n("Expert Forms");
		OTTagsMeaning["falt"]= i18n("Final glyph Alternates");
		OTTagsMeaning["fin2"]= i18n("Terminal Forms #2");
		OTTagsMeaning["fin3"]= i18n("Terminal Forms #3");
		OTTagsMeaning["fina"]= i18n("Terminal Forms");
		OTTagsMeaning["frac"]= i18n("Fractions");
		OTTagsMeaning["fwid"]= i18n("Full Width");
		OTTagsMeaning["half"]= i18n("Half Forms");
		OTTagsMeaning["haln"]= i18n("Halant Forms");
		OTTagsMeaning["halt"]= i18n("Alternate Half Width");
		OTTagsMeaning["hist"]= i18n("Historical Forms");
		OTTagsMeaning["hkna"]= i18n("Horizontal Kana Alternates");
		OTTagsMeaning["hlig"]= i18n("Historical Ligatures");
		OTTagsMeaning["hngl"]= i18n("Hangul");
		OTTagsMeaning["hojo"]= i18n("Hojo Kanji Forms (JIS x 212-1990 Kanji Forms)");
		OTTagsMeaning["hwid"]= i18n("Half Width");
		OTTagsMeaning["init"]= i18n("Initial Forms");
		OTTagsMeaning["isol"]= i18n("Isolated Forms");
		OTTagsMeaning["ital"]= i18n("Italics");
		OTTagsMeaning["jalt"]= i18n("Justification Alternatives");
		OTTagsMeaning["jp78"]= i18n("JIS78 Forms");
		OTTagsMeaning["jp83"]= i18n("JIS83 Forms");
		OTTagsMeaning["jp90"]= i18n("JIS90 Forms");
		OTTagsMeaning["jp04"]= i18n("JIS2004 Forms");
		OTTagsMeaning["kern"]= i18n("Kerning");
		OTTagsMeaning["lfbd"]= i18n("Left Bounds");
		OTTagsMeaning["liga"]= i18n("Standard Ligatures");
		OTTagsMeaning["ljmo"]= i18n("Leading Jamo Forms");
		OTTagsMeaning["lnum"]= i18n("Lining Figures");
		OTTagsMeaning["locl"]= i18n("Localized Forms");
		OTTagsMeaning["mark"]= i18n("Mark Positioning");
		OTTagsMeaning["med2"]= i18n("Medial Forms #2");
		OTTagsMeaning["medi"]= i18n("Medial Forms");
		OTTagsMeaning["mgrk"]= i18n("Mathematical Greek");
		OTTagsMeaning["mkmk"]= i18n("Mark to Mark Positioning");
		OTTagsMeaning["mset"]= i18n("Mark Positioning via Substitution");
		OTTagsMeaning["nalt"]= i18n("Alternate Annotation Forms");
		OTTagsMeaning["nlck"]= i18n("NLC Kanji Forms");
		OTTagsMeaning["nukt"]= i18n("Nukta Forms");
		OTTagsMeaning["numr"]= i18n("Numerators");
		OTTagsMeaning["onum"]= i18n("Old Style Figures");
		OTTagsMeaning["opbd"]= i18n("Optical Bounds");
		OTTagsMeaning["ordn"]= i18n("Ordinals");
		OTTagsMeaning["ornm"]= i18n("Ornaments");
		OTTagsMeaning["palt"]= i18n("Proportional Alternate Width");
		OTTagsMeaning["pcap"]= i18n("Petite Capitals");
		OTTagsMeaning["pnum"]= i18n("Proportional Figures");
		OTTagsMeaning["pref"]= i18n("Pre-base Forms");
		OTTagsMeaning["pres"]= i18n("Pre-base Substitutions");
		OTTagsMeaning["pstf"]= i18n("Post-base Forms");
		OTTagsMeaning["psts"]= i18n("Post-base Substitutions");
		OTTagsMeaning["pwid"]= i18n("Proportional Widths");
		OTTagsMeaning["qwid"]= i18n("Quarter Widths");
		OTTagsMeaning["rand"]= i18n("Randomize");
		OTTagsMeaning["rkrf"]= i18n("Rakar Forms");
		OTTagsMeaning["rlig"]= i18n("Required Ligatures");
		OTTagsMeaning["rphf"]= i18n("Reph Form");
		OTTagsMeaning["rtbd"]= i18n("Right Bounds");
		OTTagsMeaning["rtla"]= i18n("Right-To-Left Alternates");
		OTTagsMeaning["ruby"]= i18n("Ruby Notation Forms");
		OTTagsMeaning["salt"]= i18n("Stylistic Alternates");
		OTTagsMeaning["sinf"]= i18n("Scientific Inferiors");
		OTTagsMeaning["size"]= i18n("Optical Size");
		OTTagsMeaning["smcp"]= i18n("Small Capitals");
		OTTagsMeaning["smpl"]= i18n("Simplified Forms");
		OTTagsMeaning["ss01"]= i18n("Stylistic Set 1");
		OTTagsMeaning["ss02"]= i18n("Stylistic Set 2");
		OTTagsMeaning["ss03"]= i18n("Stylistic Set 3");
		OTTagsMeaning["ss04"]= i18n("Stylistic Set 4");
		OTTagsMeaning["ss05"]= i18n("Stylistic Set 5");
		OTTagsMeaning["ss06"]= i18n("Stylistic Set 6");
		OTTagsMeaning["ss07"]= i18n("Stylistic Set 7");
		OTTagsMeaning["ss08"]= i18n("Stylistic Set 8");
		OTTagsMeaning["ss09"]= i18n("Stylistic Set 9");
		OTTagsMeaning["ss10"]= i18n("Stylistic Set 10");
		OTTagsMeaning["ss11"]= i18n("Stylistic Set 11");
		OTTagsMeaning["ss12"]= i18n("Stylistic Set 12");
		OTTagsMeaning["ss13"]= i18n("Stylistic Set 13");
		OTTagsMeaning["ss14"]= i18n("Stylistic Set 14");
		OTTagsMeaning["ss15"]= i18n("Stylistic Set 15");
		OTTagsMeaning["ss16"]= i18n("Stylistic Set 16");
		OTTagsMeaning["ss17"]= i18n("Stylistic Set 17");
		OTTagsMeaning["ss18"]= i18n("Stylistic Set 18");
		OTTagsMeaning["ss19"]= i18n("Stylistic Set 19");
		OTTagsMeaning["ss20"]= i18n("Stylistic Set 20");
		OTTagsMeaning["subs"]= i18n("Subscript");
		OTTagsMeaning["sups"]= i18n("Superscript");
		OTTagsMeaning["swsh"]= i18n("Swash");
		OTTagsMeaning["titl"]= i18n("Titling");
		OTTagsMeaning["tjmo"]= i18n("Trailing Jamo Forms");
		OTTagsMeaning["tnam"]= i18n("Traditional Name Forms");
		OTTagsMeaning["tnum"]= i18n("Tabular Figures");
		OTTagsMeaning["trad"]= i18n("Traditional Forms");
		OTTagsMeaning["twid"]= i18n("Third Widths");
		OTTagsMeaning["unic"]= i18n("Unicase");
		OTTagsMeaning["valt"]= i18n("Alternate Vertical Metrics");
		OTTagsMeaning["vatu"]= i18n("Vattu Variants");
		OTTagsMeaning["vert"]= i18n("Vertical Writing");
		OTTagsMeaning["vhal"]= i18n("Alternate Vertical Half Metrics");
		OTTagsMeaning["vjmo"]= i18n("Vowel Jamo Forms");
		OTTagsMeaning["vkna"]= i18n("Vertical Kana Alternates");
		OTTagsMeaning["vkrn"]= i18n("Vertical Kerning");
		OTTagsMeaning["vpal"]= i18n("Proportional Alternate Vertical Metrics");
		OTTagsMeaning["vrt2"]= i18n("Vertical Rotation");
		OTTagsMeaning["zero"]= i18n("Slashed Zero");
	}
	return OTTagsMeaning.value(tag);
};

#endif
