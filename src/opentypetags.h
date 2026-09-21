/*
    SPDX-FileCopyrightText: 2007 Pierre Marchand <pierre@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef OTTMEAN_H
#define OTTMEAN_H

#include <KLocalizedString>
#include <QMap>

QString OTTagMeans(QString tag)
{
    static QMap<QString, QString> OTTagsMeaning;
    if (OTTagsMeaning.isEmpty()) {
        OTTagsMeaning[QStringLiteral("aalt")] = i18n("Access All Alternates");
        OTTagsMeaning[QStringLiteral("abvf")] = i18n("Above-Base Forms");
        OTTagsMeaning[QStringLiteral("abvm")] = i18n("Above-Base Mark Positioning");
        OTTagsMeaning[QStringLiteral("abvs")] = i18n("Above-Base Substitutions");
        OTTagsMeaning[QStringLiteral("afrc")] = i18n("Alternative Fractions");
        OTTagsMeaning[QStringLiteral("akhn")] = i18n("Akhands");
        OTTagsMeaning[QStringLiteral("blwf")] = i18n("Below-Base Forms");
        OTTagsMeaning[QStringLiteral("blwm")] = i18n("Below-Base Mark Positioning");
        OTTagsMeaning[QStringLiteral("blws")] = i18n("Below-Base Substitutions");
        OTTagsMeaning[QStringLiteral("c2pc")] = i18n("Petite Capitals From Capitals");
        OTTagsMeaning[QStringLiteral("c2sc")] = i18n("Small Capitals From Capitals");
        OTTagsMeaning[QStringLiteral("calt")] = i18n("Contextual Alternates");
        OTTagsMeaning[QStringLiteral("case")] = i18n("Case-Sensitive Forms");
        OTTagsMeaning[QStringLiteral("ccmp")] = i18n("Glyph Composition/Decomposition");
        OTTagsMeaning[QStringLiteral("clig")] = i18n("Contextual Ligatures");
        OTTagsMeaning[QStringLiteral("cjct")] = i18n("Conjunct Forms");
        OTTagsMeaning[QStringLiteral("cpsp")] = i18n("Capital Spacing");
        OTTagsMeaning[QStringLiteral("cswh")] = i18n("Contextual Swash");
        OTTagsMeaning[QStringLiteral("curs")] = i18n("Cursive Positioning");
        OTTagsMeaning[QStringLiteral("dflt")] = i18n("Default Processing");
        OTTagsMeaning[QStringLiteral("dist")] = i18n("Distances");
        OTTagsMeaning[QStringLiteral("dlig")] = i18n("Discretionary Ligatures");
        OTTagsMeaning[QStringLiteral("dnom")] = i18n("Denominators");
        OTTagsMeaning[QStringLiteral("expt")] = i18n("Expert Forms");
        OTTagsMeaning[QStringLiteral("falt")] = i18n("Final glyph Alternates");
        OTTagsMeaning[QStringLiteral("fin2")] = i18n("Terminal Forms #2");
        OTTagsMeaning[QStringLiteral("fin3")] = i18n("Terminal Forms #3");
        OTTagsMeaning[QStringLiteral("fina")] = i18n("Terminal Forms");
        OTTagsMeaning[QStringLiteral("frac")] = i18n("Fractions");
        OTTagsMeaning[QStringLiteral("fwid")] = i18n("Full Width");
        OTTagsMeaning[QStringLiteral("half")] = i18n("Half Forms");
        OTTagsMeaning[QStringLiteral("haln")] = i18n("Halant Forms");
        OTTagsMeaning[QStringLiteral("halt")] = i18n("Alternate Half Width");
        OTTagsMeaning[QStringLiteral("hist")] = i18n("Historical Forms");
        OTTagsMeaning[QStringLiteral("hkna")] = i18n("Horizontal Kana Alternates");
        OTTagsMeaning[QStringLiteral("hlig")] = i18n("Historical Ligatures");
        OTTagsMeaning[QStringLiteral("hngl")] = i18n("Hangul");
        OTTagsMeaning[QStringLiteral("hojo")] = i18n("Hojo Kanji Forms (JIS x 212-1990 Kanji Forms)");
        OTTagsMeaning[QStringLiteral("hwid")] = i18n("Half Width");
        OTTagsMeaning[QStringLiteral("init")] = i18n("Initial Forms");
        OTTagsMeaning[QStringLiteral("isol")] = i18n("Isolated Forms");
        OTTagsMeaning[QStringLiteral("ital")] = i18n("Italics");
        OTTagsMeaning[QStringLiteral("jalt")] = i18n("Justification Alternatives");
        OTTagsMeaning[QStringLiteral("jp78")] = i18n("JIS78 Forms");
        OTTagsMeaning[QStringLiteral("jp83")] = i18n("JIS83 Forms");
        OTTagsMeaning[QStringLiteral("jp90")] = i18n("JIS90 Forms");
        OTTagsMeaning[QStringLiteral("jp04")] = i18n("JIS2004 Forms");
        OTTagsMeaning[QStringLiteral("kern")] = i18n("Kerning");
        OTTagsMeaning[QStringLiteral("lfbd")] = i18n("Left Bounds");
        OTTagsMeaning[QStringLiteral("liga")] = i18n("Standard Ligatures");
        OTTagsMeaning[QStringLiteral("ljmo")] = i18n("Leading Jamo Forms");
        OTTagsMeaning[QStringLiteral("lnum")] = i18n("Lining Figures");
        OTTagsMeaning[QStringLiteral("locl")] = i18n("Localized Forms");
        OTTagsMeaning[QStringLiteral("mark")] = i18n("Mark Positioning");
        OTTagsMeaning[QStringLiteral("med2")] = i18n("Medial Forms #2");
        OTTagsMeaning[QStringLiteral("medi")] = i18n("Medial Forms");
        OTTagsMeaning[QStringLiteral("mgrk")] = i18n("Mathematical Greek");
        OTTagsMeaning[QStringLiteral("mkmk")] = i18n("Mark to Mark Positioning");
        OTTagsMeaning[QStringLiteral("mset")] = i18n("Mark Positioning via Substitution");
        OTTagsMeaning[QStringLiteral("nalt")] = i18n("Alternate Annotation Forms");
        OTTagsMeaning[QStringLiteral("nlck")] = i18n("NLC Kanji Forms");
        OTTagsMeaning[QStringLiteral("nukt")] = i18n("Nukta Forms");
        OTTagsMeaning[QStringLiteral("numr")] = i18n("Numerators");
        OTTagsMeaning[QStringLiteral("onum")] = i18n("Old Style Figures");
        OTTagsMeaning[QStringLiteral("opbd")] = i18n("Optical Bounds");
        OTTagsMeaning[QStringLiteral("ordn")] = i18n("Ordinals");
        OTTagsMeaning[QStringLiteral("ornm")] = i18n("Ornaments");
        OTTagsMeaning[QStringLiteral("palt")] = i18n("Proportional Alternate Width");
        OTTagsMeaning[QStringLiteral("pcap")] = i18n("Petite Capitals");
        OTTagsMeaning[QStringLiteral("pnum")] = i18n("Proportional Figures");
        OTTagsMeaning[QStringLiteral("pref")] = i18n("Pre-base Forms");
        OTTagsMeaning[QStringLiteral("pres")] = i18n("Pre-base Substitutions");
        OTTagsMeaning[QStringLiteral("pstf")] = i18n("Post-base Forms");
        OTTagsMeaning[QStringLiteral("psts")] = i18n("Post-base Substitutions");
        OTTagsMeaning[QStringLiteral("pwid")] = i18n("Proportional Widths");
        OTTagsMeaning[QStringLiteral("qwid")] = i18n("Quarter Widths");
        OTTagsMeaning[QStringLiteral("rand")] = i18n("Randomize");
        OTTagsMeaning[QStringLiteral("rkrf")] = i18n("Rakar Forms");
        OTTagsMeaning[QStringLiteral("rlig")] = i18n("Required Ligatures");
        OTTagsMeaning[QStringLiteral("rphf")] = i18n("Reph Form");
        OTTagsMeaning[QStringLiteral("rtbd")] = i18n("Right Bounds");
        OTTagsMeaning[QStringLiteral("rtla")] = i18n("Right-To-Left Alternates");
        OTTagsMeaning[QStringLiteral("ruby")] = i18n("Ruby Notation Forms");
        OTTagsMeaning[QStringLiteral("salt")] = i18n("Stylistic Alternates");
        OTTagsMeaning[QStringLiteral("sinf")] = i18n("Scientific Inferiors");
        OTTagsMeaning[QStringLiteral("size")] = i18n("Optical Size");
        OTTagsMeaning[QStringLiteral("smcp")] = i18n("Small Capitals");
        OTTagsMeaning[QStringLiteral("smpl")] = i18n("Simplified Forms");
        OTTagsMeaning[QStringLiteral("ss01")] = i18n("Stylistic Set 1");
        OTTagsMeaning[QStringLiteral("ss02")] = i18n("Stylistic Set 2");
        OTTagsMeaning[QStringLiteral("ss03")] = i18n("Stylistic Set 3");
        OTTagsMeaning[QStringLiteral("ss04")] = i18n("Stylistic Set 4");
        OTTagsMeaning[QStringLiteral("ss05")] = i18n("Stylistic Set 5");
        OTTagsMeaning[QStringLiteral("ss06")] = i18n("Stylistic Set 6");
        OTTagsMeaning[QStringLiteral("ss07")] = i18n("Stylistic Set 7");
        OTTagsMeaning[QStringLiteral("ss08")] = i18n("Stylistic Set 8");
        OTTagsMeaning[QStringLiteral("ss09")] = i18n("Stylistic Set 9");
        OTTagsMeaning[QStringLiteral("ss10")] = i18n("Stylistic Set 10");
        OTTagsMeaning[QStringLiteral("ss11")] = i18n("Stylistic Set 11");
        OTTagsMeaning[QStringLiteral("ss12")] = i18n("Stylistic Set 12");
        OTTagsMeaning[QStringLiteral("ss13")] = i18n("Stylistic Set 13");
        OTTagsMeaning[QStringLiteral("ss14")] = i18n("Stylistic Set 14");
        OTTagsMeaning[QStringLiteral("ss15")] = i18n("Stylistic Set 15");
        OTTagsMeaning[QStringLiteral("ss16")] = i18n("Stylistic Set 16");
        OTTagsMeaning[QStringLiteral("ss17")] = i18n("Stylistic Set 17");
        OTTagsMeaning[QStringLiteral("ss18")] = i18n("Stylistic Set 18");
        OTTagsMeaning[QStringLiteral("ss19")] = i18n("Stylistic Set 19");
        OTTagsMeaning[QStringLiteral("ss20")] = i18n("Stylistic Set 20");
        OTTagsMeaning[QStringLiteral("subs")] = i18n("Subscript");
        OTTagsMeaning[QStringLiteral("sups")] = i18n("Superscript");
        OTTagsMeaning[QStringLiteral("swsh")] = i18n("Swash");
        OTTagsMeaning[QStringLiteral("titl")] = i18n("Titling");
        OTTagsMeaning[QStringLiteral("tjmo")] = i18n("Trailing Jamo Forms");
        OTTagsMeaning[QStringLiteral("tnam")] = i18n("Traditional Name Forms");
        OTTagsMeaning[QStringLiteral("tnum")] = i18n("Tabular Figures");
        OTTagsMeaning[QStringLiteral("trad")] = i18n("Traditional Forms");
        OTTagsMeaning[QStringLiteral("twid")] = i18n("Third Widths");
        OTTagsMeaning[QStringLiteral("unic")] = i18n("Unicase");
        OTTagsMeaning[QStringLiteral("valt")] = i18n("Alternate Vertical Metrics");
        OTTagsMeaning[QStringLiteral("vatu")] = i18n("Vattu Variants");
        OTTagsMeaning[QStringLiteral("vert")] = i18n("Vertical Writing");
        OTTagsMeaning[QStringLiteral("vhal")] = i18n("Alternate Vertical Half Metrics");
        OTTagsMeaning[QStringLiteral("vjmo")] = i18n("Vowel Jamo Forms");
        OTTagsMeaning[QStringLiteral("vkna")] = i18n("Vertical Kana Alternates");
        OTTagsMeaning[QStringLiteral("vkrn")] = i18n("Vertical Kerning");
        OTTagsMeaning[QStringLiteral("vpal")] = i18n("Proportional Alternate Vertical Metrics");
        OTTagsMeaning[QStringLiteral("vrt2")] = i18n("Vertical Rotation");
        OTTagsMeaning[QStringLiteral("zero")] = i18n("Slashed Zero");
    }
    return OTTagsMeaning.value(tag);
};

#endif
