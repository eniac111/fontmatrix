//
// C++ Implementation: fmfontstrings
//
// Description: 
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "fmfontstrings.h"
#include "fmpaths.h"

#include <KLocalizedString>
#include <QFile>

FontStrings * FontStrings::instance = nullptr;
FontStrings::FontStrings()
{
	fillNamesMeaning();
	fillPanoseMap();
	fillCharsetMap();
	fillTTTableList();
	fillFSftypeMap();
	fillUniCat();
	fillScriptTagNames();
}

FontStrings * FontStrings::getInstance()
{
	if(!instance)
		instance = new FontStrings;
	return instance;
}

void FontStrings::fillNamesMeaning()
{
	m_name[FMFontDb::Copyright]= i18n( "Copyright" );
	m_name[FMFontDb::FontFamily]= i18n( "Font Family" );
	m_name[FMFontDb::FontSubfamily]= i18n( "Font Subfamily" );
	m_name[FMFontDb::UniqueFontIdentifier]= i18n( "Unique font identifier" );
	m_name[FMFontDb::FullFontName]= i18n( "Full font name" );
	m_name[FMFontDb::VersionString]= i18n( "Version string" );
	m_name[FMFontDb::PostscriptName]= i18n( "Postscript name" );
	m_name[FMFontDb::Trademark]= i18n( "Trademark" );
	m_name[FMFontDb::ManufacturerName]= i18n( "Manufacturer" );
	m_name[FMFontDb::Designer]= i18n( "Designer" );
	m_name[FMFontDb::Description]= i18n( "Description" );
	m_name[FMFontDb::URLVendor]= i18n( "URL Vendor" );
	m_name[FMFontDb::URLDesigner]= i18n( "URL Designer" );
	m_name[FMFontDb::LicenseDescription]= i18n( "License Description" );
	m_name[FMFontDb::LicenseInfoURL]= i18n( "License Info URL" );
	m_name[FMFontDb::PreferredFamily]= i18n( "Preferred Family" );
	m_name[FMFontDb::PreferredSubfamily]= i18n( "Preferred Subfamily" );
	m_name[FMFontDb::CompatibleMacintosh]= i18n( "Compatible Full (Macintosh only)" );
	m_name[FMFontDb::SampleText]= i18n( "Sample text" );
	m_name[FMFontDb::PostScriptCIDName]= i18n( "PostScript CID findfont name" );
// 	m_name[FMFontDb::Panose]= i18n("Panose");
	m_name[FMFontDb::AllInfo]= i18n("All fields");
	
}

void FontStrings::fillPanoseMap()
{
// 	QString panofilepath( FMPaths::LocalizedFilePath(FMPaths::ResourcesDir() + "Panose", ".xml" ) );
// 	if(!panofilepath.isEmpty())
// 	{
// 		panoseFromFile(panofilepath);
// 		return;
// 	}
		
	// http://www.microsoft.com/OpenType/OTSpec/os2ver0.htm#pan
	// http://www.monotypeimaging.com/ProductsServices/pan2.aspx

		QMap<int, QString> mapModel;
		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Family Type", "Text and Display") ;
		mapModel[ 3 ] = i18nc("Family Type", "Script") ;
		mapModel[ 4 ] = i18nc("Family Type", "Decorative") ;
		mapModel[ 5 ] = i18nc("Family Type", "Pictorial") ;

		m_panoseMap[FamilyType] = mapModel;
		m_panoseKeyName[FamilyType] = i18n( "Family Type" );
		m_panoseKeyInfo[FamilyType] = "<h1>Family Kind</h1> Defines what type of font is being classified." ;
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Serif style", "Cove") ;
		mapModel[ 3 ] = i18nc("Serif style", "Obtuse Cove") ;
		mapModel[ 4 ] = i18nc("Serif style", "Square Cove") ;
		mapModel[ 5 ] = i18nc("Serif style", "Obtuse Square Cove") ;
		mapModel[ 6 ] = i18nc("Serif style", "Square") ;
		mapModel[ 7 ] = i18nc("Serif style", "Thin") ;
		mapModel[ 8 ] = i18nc("Serif style", "Bone") ;
		mapModel[ 9 ] = i18nc("Serif style", "Exaggerated") ;
		mapModel[ 10 ] = i18nc("Serif style", "Triangle") ;
		mapModel[ 11 ] = i18nc("Serif style", "Normal Sans") ;
		mapModel[ 12 ] = i18nc("Serif style", "Obtuse Sans") ;
		mapModel[ 13 ] = i18nc("Serif style", "Perp Sans") ;
		mapModel[ 14 ] = i18nc("Serif style", "Flared") ;
		mapModel[ 15 ] = i18nc("Serif style", "Rounded") ;

		m_panoseMap[SerifStyle ] = mapModel;
		m_panoseKeyName[SerifStyle] = i18n( "Serif style" ) ;
		m_panoseKeyInfo[SerifStyle] ="<h1>Serif style</h1>This digit describes the appearance of the serifs used in a font design";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Weight", "Very Light") ;
		mapModel[ 3 ] = i18nc("Weight", "Light") ;
		mapModel[ 4 ] = i18nc("Weight", "Thin") ;
		mapModel[ 5 ] = i18nc("Weight", "Book") ;
		mapModel[ 6 ] = i18nc("Weight", "Medium") ;
		mapModel[ 7 ] = i18nc("Weight", "Demi") ;
		mapModel[ 8 ] = i18nc("Weight", "Bold") ;
		mapModel[ 9 ] = i18nc("Weight", "Heavy") ;
		mapModel[ 10 ] = i18nc("Weight", "Black") ;
		mapModel[ 11 ] = i18nc("Weight", "Nord") ;

		m_panoseMap[Weight ] = mapModel;
		m_panoseKeyName[Weight] = i18n( "Weight" );
		m_panoseKeyInfo[Weight] ="<h1>Weight</h1>The Weight digit classifies the appearance of a fonts’ stroke thickness in relation to its height.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Proportion", "Old Style") ;
		mapModel[ 3 ] = i18nc("Proportion", "Modern") ;
		mapModel[ 4 ] = i18nc("Proportion", "Even Width") ;
		mapModel[ 5 ] = i18nc("Proportion", "Expanded") ;
		mapModel[ 6 ] = i18nc("Proportion", "Condensed") ;
		mapModel[ 7 ] = i18nc("Proportion", "Very Expanded") ;
		mapModel[ 8 ] = i18nc("Proportion", "Very Condensed") ;
		mapModel[ 9 ] = i18nc("Proportion", "Monospaced") ;

		m_panoseMap[Proportion ] = mapModel;
		m_panoseKeyName[Proportion] = i18n( "Proportion" );
		m_panoseKeyInfo[Proportion] ="<h1>Proportion</h1>The proportion of a font in the PANOSE Typeface Matching System is defined in greater detail than simply an indication of general glyph shape aspect ratio such as extended and condensed.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Contrast", "None") ;
		mapModel[ 3 ] = i18nc("Contrast", "Very Low") ;
		mapModel[ 4 ] = i18nc("Contrast", "Low") ;
		mapModel[ 5 ] = i18nc("Contrast", "Medium Low") ;
		mapModel[ 6 ] = i18nc("Contrast", "Medium") ;
		mapModel[ 7 ] = i18nc("Contrast", "Medium High") ;
		mapModel[ 8 ] = i18nc("Contrast", "High") ;
		mapModel[ 9 ] = i18nc("Contrast", "Very High") ;

		m_panoseMap[Contrast ] = mapModel;
		m_panoseKeyName[Contrast] = i18n( "Contrast" );
		m_panoseKeyInfo[Contrast] ="<h1>Contrast</h1>The Contrast digit describes the ratio between the thickest point on the stroke of the letter O and the narrowest point on the letter O.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Stroke Variation", "Gradual/Diagonal") ;
		mapModel[ 3 ] = i18nc("Stroke Variation", "Gradual/Transitional") ;
		mapModel[ 4 ] = i18nc("Stroke Variation", "Gradual/Vertical") ;
		mapModel[ 5 ] = i18nc("Stroke Variation", "Gradual/Horizontal") ;
		mapModel[ 6 ] = i18nc("Stroke Variation", "Rapid/Vertical") ;
		mapModel[ 7 ] = i18nc("Stroke Variation", "Rapid/Horizontal") ;
		mapModel[ 8 ] = i18nc("Stroke Variation", "Instant/Vertical") ;

		m_panoseMap[StrokeVariation] = mapModel;
		m_panoseKeyName[StrokeVariation] = i18n( "Stroke Variation" );
		m_panoseKeyInfo[StrokeVariation] ="<h1>Stroke Variation</h1>The Stroke Variation category further details the contrast trait by describing the kind of transition that occurs as the stem thickness changes on rounded glyph shapes.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Arm Style", "Straight Arms/Horizontal") ;
		mapModel[ 3 ] = i18nc("Arm Style", "Straight Arms/Wedge") ;
		mapModel[ 4 ] = i18nc("Arm Style", "Straight Arms/Vertical") ;
		mapModel[ 5 ] = i18nc("Arm Style", "Straight Arms/Single Serif") ;
		mapModel[ 6 ] = i18nc("Arm Style", "Straight Arms/Double Serif") ;
		mapModel[ 7 ] = i18nc("Arm Style", "Non-Straight Arms/Horizontal") ;
		mapModel[ 8 ] = i18nc("Arm Style", "Non-Straight Arms/Wedge") ;
		mapModel[ 9 ] = i18nc("Arm Style", "Non-Straight Arms/Vertical") ;
		mapModel[ 10 ] = i18nc("Arm Style", "Non-Straight Arms/Single Serif") ;
		mapModel[ 11 ] = i18nc("Arm Style", "Non-Straight Arms/Double Serif") ;

		m_panoseMap[ArmStyle ] = mapModel;
		m_panoseKeyName[ArmStyle] = i18n( "Arm Style" );
		m_panoseKeyInfo[ArmStyle] ="<h1>Arm Style</h1>The Arm Style category classifies two attributes of a glyph design: special treatment of diagonal stems and termination of open rounded letterforms.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Letterform", "Normal/Contact") ;
		mapModel[ 3 ] = i18nc("Letterform", "Normal/Weighted") ;
		mapModel[ 4 ] = i18nc("Letterform", "Normal/Boxed");
		mapModel[ 5 ] = i18nc("Letterform", "Normal/Flattened") ;
		mapModel[ 6 ] = i18nc("Letterform", "Normal/Rounded") ;
		mapModel[ 7 ] = i18nc("Letterform", "Normal/Off Center") ;
		mapModel[ 8 ] = i18nc("Letterform", "Normal/Square") ;
		mapModel[ 9 ] = i18nc("Letterform", "Oblique/Contact") ;
		mapModel[ 10 ] = i18nc("Letterform", "Oblique/Weighted") ;
		mapModel[ 11 ] = i18nc("Letterform", "Oblique/Boxed") ;
		mapModel[ 12 ] = i18nc("Letterform", "Oblique/Flattened") ;
		mapModel[ 13 ] = i18nc("Letterform", "Oblique/Rounded") ;
		mapModel[ 14 ] = i18nc("Letterform", "Oblique/Off Center") ;
		mapModel[ 15 ] = i18nc("Letterform", "Oblique/Square") ;

		m_panoseMap[Letterform] = mapModel;
		m_panoseKeyName[Letterform] = i18n( "Letterform" );
		m_panoseKeyInfo[Letterform] ="<h1>Letterform</h1>Roundness and predominant skewing is classified in the Letterform category.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("Midline", "Standard/Trimmed") ;
		mapModel[ 3 ] = i18nc("Midline", "Standard/Pointed") ;
		mapModel[ 4 ] = i18nc("Midline", "Standard/Serifed") ;
		mapModel[ 5 ] = i18nc("Midline", "High/Trimmed") ;
		mapModel[ 6 ] = i18nc("Midline", "High/Pointed") ;
		mapModel[ 7 ] = i18nc("Midline", "High/Serifed") ;
		mapModel[ 8 ] = i18nc("Midline", "Constant/Trimmed") ;
		mapModel[ 9 ] = i18nc("Midline", "Constant/Pointed") ;
		mapModel[ 10 ] = i18nc("Midline", "Constant/Serifed") ;
		mapModel[ 11 ] = i18nc("Midline", "Low/Trimmed") ;
		mapModel[ 12 ] = i18nc("Midline", "Low/Pointed") ;
		mapModel[ 13 ] = i18nc("Midline", "Low/Serifed") ;

		m_panoseMap[Midline ] = mapModel;
		m_panoseKeyName[Midline] = i18n( "Midline" );
		m_panoseKeyInfo[Midline] ="<h1>Midline</h1>The ninth category in the PANOSE classification system analyzes two traits, the placement of the midline across the uppercase characters and the treatment of diagonal stem apexes.";
		mapModel.clear();

		mapModel[ 0 ] = i18n( "Any" ) ;
		mapModel[ 1 ] = i18n( "No Fit" ) ;
		mapModel[ 2 ] = i18nc("X-Height", "Constant/Small") ;
		mapModel[ 3 ] = i18nc("X-Height", "Constant/Standard") ;
		mapModel[ 4 ] = i18nc("X-Height", "Constant/Large") ;
		mapModel[ 5 ] = i18nc("X-Height", "Ducking/Small") ;
		mapModel[ 6 ] = i18nc("X-Height", "Ducking/Standard") ;
		mapModel[ 7 ] = i18nc("X-Height", "Ducking/Large") ;

		m_panoseMap[XHeight ] = mapModel;
		m_panoseKeyName[XHeight] = i18n( "X-Height" );
		m_panoseKeyInfo[XHeight] ="<h1>X-Height</h1>Two different traits are represented in the X-height digit: the treatment of uppercase glyphs with diacritical marks and the relative size of the lowercase characters.";

}

void FontStrings::panoseFromFile(const QString & )
{
	return;
}
void FontStrings::fillCharsetMap()
{
	charsetMap[FT_ENCODING_NONE] = "None";
	charsetMap[FT_ENCODING_UNICODE] = "Unicode";
	charsetMap[FT_ENCODING_MS_SYMBOL] = "MS Symbol";
	charsetMap[FT_ENCODING_SJIS] = "SJIS";
	charsetMap[FT_ENCODING_GB2312	] = "GB2312";
	charsetMap[FT_ENCODING_BIG5] = "BIG5";
	charsetMap[FT_ENCODING_WANSUNG] = "Wansung";
	charsetMap[FT_ENCODING_JOHAB] = "Johab";
	charsetMap[FT_ENCODING_ADOBE_LATIN_1] = "Adobe Latin 1";
	charsetMap[FT_ENCODING_ADOBE_STANDARD] = "Adobe Standard";
	charsetMap[FT_ENCODING_ADOBE_EXPERT] = "Adobe Expert";
	charsetMap[FT_ENCODING_ADOBE_CUSTOM] = "Adobe Custom";
	charsetMap[FT_ENCODING_APPLE_ROMAN] = "Apple Roman";
	charsetMap[FT_ENCODING_OLD_LATIN_2] = i18n( "This value is deprecated and was never used nor reported by FreeType. Don't use or test for it." );
	charsetMap[FT_ENCODING_MS_SJIS] = "MS SJIS";
	charsetMap[FT_ENCODING_MS_GB2312] = "MS GB2312";
	charsetMap[FT_ENCODING_MS_BIG5] = "MS BIG5";
	charsetMap[FT_ENCODING_MS_WANSUNG] = "MS Wansung";
	charsetMap[FT_ENCODING_MS_JOHAB] = "MS Johab";
}

void FontStrings::fillTTTableList()
{
	tttableList.clear();
	
	// Required Tables
	tttableList["cmap"] = i18n("Character to glyph mapping");
	tttableList["head"] = i18n("Font header");
	tttableList["hhea"] = i18n("Horizontal header");
	tttableList["hmtx"] = i18n("Horizontal metrics");
	tttableList["maxp"] = i18n("Maximum profile");
	tttableList["name"] = i18n("Naming table");
	tttableList["OS/2"] = i18n("OS/2 and Windows specific metrics");
	tttableList["post"] = i18n("PostScript information");
	
	// Tables Related to TrueType Outlines
	tttableList["cvt"] = i18n("Control Value Table");
	tttableList["fpgm"] = i18n("Font program");
	tttableList["glyf"] = i18n("Glyph data");
	tttableList["loca"] = i18n("Index to location");
	tttableList["prep"] = i18n("CVT Program");
	
	// Tables Related to PostScript Outlines
	tttableList["CFF"] = i18n("PostScript font program");
	tttableList["VORG"] = i18n("Vertical Origin");
	
	// Tables Related to Bitmap Glyphs
	tttableList["EBDT"] = i18n("Embedded bitmap data");
	tttableList["EBLC"] = i18n("Embedded bitmap location data");
	tttableList["EBSC"] = i18n("Embedded bitmap scaling data");
	
	// Advanced Typographic Tables
	tttableList["BASE"] = i18n("Baseline data");
	tttableList["GDEF"] = i18n("Glyph definition data");
	tttableList["GPOS"] = i18n("Glyph positioning data");
	tttableList["GSUB"] = i18n("Glyph substitution data");
	tttableList["JSTF"] = i18n("Justification data");
	
	// Other OpenType Tables
	tttableList["DSIG"] = i18n("Digital signature");
	tttableList["gasp"] = i18n("Grid-fitting/Scan-conversion");
	tttableList["hdmx"] = i18n("Horizontal device metrics");
	tttableList["kern"] = i18n("Kerning");
	tttableList["LTSH"] = i18n("Linear threshold data");
	tttableList["PCLT"] = i18n("PCL 5 data");
	tttableList["VDMX"] = i18n("Vertical device metrics");
	tttableList["vhea"] = i18n("Vertical Metrics header");
	tttableList["vmtx"] = i18n("Vertical Metrics");
}


void FontStrings::fillFSftypeMap()
{
	// From http://www.microsoft.com/typography/otspec/os2.htm#fst
	
	m_FsType[FontItem::NOT_RESTRICTED] = i18n("This font may be embedded and permanently installed on the remote system by an application. The user of the remote system acquires the identical rights, obligations and licenses for that font as the original purchaser of the font, and is subject to the same end-user license agreement, copyright, design patent, and/or trademark as was the original purchaser.");
	m_FsType[FontItem::RESTRICTED] = i18n("This font must not be modified, embedded or exchanged in any manner without first obtaining permission of the legal owner.");
	m_FsType[FontItem::PREVIEW_PRINT] = i18n("This font may be embedded, and temporarily loaded on the remote system. Documents containing this font must be opened \"read-only;\" no edits can be applied to the document.");
	m_FsType[FontItem::EDIT_EMBED] = i18n("This font may be embedded but must only be installed  temporarily  on other systems. In contrast to Preview &amp; Print fonts, documents containing this font may be opened for reading, editing is permitted, and changes may be saved.");
	m_FsType[FontItem::NOSUBSET] = i18n("This font may not be subsetted prior to embedding. Other embedding restrictions specified in bits 0-3 and 9 also apply.");
	m_FsType[FontItem::BITMAP_ONLY] = i18n("Only bitmaps contained in this font may be embedded. No outline data may be embedded. If there are no bitmaps available in this font, then it is considered unembeddable and the embedding services will fail. Other embedding restrictions specified in bits 0-3 and 8 also apply.");
}

void FontStrings::fillUniCat()
{
	unicodeCategory[QChar::Mark_NonSpacing] = i18n("Mark, NonSpacing" );
	unicodeCategory[QChar::Mark_SpacingCombining] = i18n("Mark, SpacingCombining" );
	unicodeCategory[QChar::Mark_Enclosing] = i18n("Mark, Enclosing" );
	unicodeCategory[QChar::Number_DecimalDigit] = i18n("Number, DecimalDigit" );
	unicodeCategory[QChar::Number_Letter] = i18n("Number, Letter" );
	unicodeCategory[QChar::Number_Other] = i18n("Number, Other" );
	unicodeCategory[QChar::Separator_Space] = i18n("Separator, Space" );
	unicodeCategory[QChar::Separator_Line] = i18n("Separator, Line" );
	unicodeCategory[QChar::Separator_Paragraph] = i18n("Separator, Paragraph" );
	unicodeCategory[QChar::Other_Control] = i18n("Other, Control" );
	unicodeCategory[QChar::Other_Format] = i18n("Other, Format" );
	unicodeCategory[QChar::Other_Surrogate] = i18n("Other, Surrogate" );
	unicodeCategory[QChar::Other_PrivateUse] = i18n("Other, PrivateUse" );
	unicodeCategory[QChar::Other_NotAssigned] = i18n("Other, NotAssigned" );
	unicodeCategory[QChar::Letter_Uppercase] = i18n("Letter, Uppercase" );
	unicodeCategory[QChar::Letter_Lowercase] = i18n("Letter, Lowercase" );
	unicodeCategory[QChar::Letter_Titlecase] = i18n("Letter, Titlecase" );
	unicodeCategory[QChar::Letter_Modifier] = i18n("Letter, Modifier" );
	unicodeCategory[QChar::Letter_Other] = i18n("Letter, Other" );
	unicodeCategory[QChar::Punctuation_Connector] = i18n("Punctuation, Connector" );
	unicodeCategory[QChar::Punctuation_Dash] = i18n("Punctuation, Dash" );
	unicodeCategory[QChar::Punctuation_Open] = i18n("Punctuation, Open" );
	unicodeCategory[QChar::Punctuation_Close] = i18n("Punctuation, Close" );
	unicodeCategory[QChar::Punctuation_InitialQuote] = i18n("Punctuation, InitialQuote" );
	unicodeCategory[QChar::Punctuation_FinalQuote] = i18n("Punctuation, FinalQuote" );
	unicodeCategory[QChar::Punctuation_Other] = i18n("Punctuation, Other" );
	unicodeCategory[QChar::Symbol_Math] = i18n("Symbol, Math" );
	unicodeCategory[QChar::Symbol_Currency] = i18n("Symbol, Currency" );
	unicodeCategory[QChar::Symbol_Modifier] = i18n("Symbol, Modifier" );
	unicodeCategory[QChar::Symbol_Other] = i18n("Symbol, Other" );
}

void FontStrings::fillScriptTagNames()
{
	scriptTagNames[QString("arab")] = i18n("Arabic");
	scriptTagNames[QString("armn")] = i18n("Armenian");
	scriptTagNames[QString("bali")] = i18n("Balinese");
	scriptTagNames[QString("beng")] = i18n("Bengali");
	scriptTagNames[QString("bopo")] = i18n("Bopomofo");
	scriptTagNames[QString("brai")] = i18n("Braille");
	scriptTagNames[QString("bugi")] = i18n("Buginese");
	scriptTagNames[QString("buhd")] = i18n("Buhid");
	scriptTagNames[QString("byzm")] = i18n("Byzantine Music");
	scriptTagNames[QString("cans")] = i18n("Canadian Syllabics");
	scriptTagNames[QString("cher")] = i18n("Cherokee");
	scriptTagNames[QString("hani")] = i18n("CJK Ideographic");
	scriptTagNames[QString("copt")] = i18n("Coptic");
	scriptTagNames[QString("cprt")] = i18n("Cypriot Syllabary");
	scriptTagNames[QString("cyrl")] = i18n("Cyrillic");
	scriptTagNames[QString("DFLT")] = i18n("Default");
	scriptTagNames[QString("dsrt")] = i18n("Deseret");
	scriptTagNames[QString("deva")] = i18n("Devanagari");
	scriptTagNames[QString("ethi")] = i18n("Ethiopic");
	scriptTagNames[QString("geor")] = i18n("Georgian");
	scriptTagNames[QString("glag")] = i18n("Glagolitic");
	scriptTagNames[QString("goth")] = i18n("Gothic");
	scriptTagNames[QString("grek")] = i18n("Greek");
	scriptTagNames[QString("gujr")] = i18n("Gujarati");
	scriptTagNames[QString("guru")] = i18n("Gurmukhi");
	scriptTagNames[QString("jamo")] = i18n("Hangul Jamo");
	scriptTagNames[QString("hang")] = i18n("Hangul");
	scriptTagNames[QString("hano")] = i18n("Hanunoo");
	scriptTagNames[QString("hebr")] = i18n("Hebrew");
	scriptTagNames[QString("kana")] = i18n("Hiragana and Katakana");
	scriptTagNames[QString("java")] = i18n("Javanese");
	scriptTagNames[QString("knda")] = i18n("Kannada");
	scriptTagNames[QString("khar")] = i18n("Kharosthi");
	scriptTagNames[QString("khmr")] = i18n("Khmer");
	scriptTagNames[QString("lao ")] = i18n("Lao");
	scriptTagNames[QString("latn")] = i18n("Latin");
	scriptTagNames[QString("limb")] = i18n("Limbu");
	scriptTagNames[QString("linb")] = i18n("Linear B");
	scriptTagNames[QString("mlym")] = i18n("Malayalam");
	scriptTagNames[QString("math")] = i18n("Mathematical Alphanumeric Symbols");
	scriptTagNames[QString("mong")] = i18n("Mongolian");
	scriptTagNames[QString("musc")] = i18n("Musical Symbols");
	scriptTagNames[QString("mymr")] = i18n("Myanmar");
	scriptTagNames[QString("nko ")] = i18n("N'ko");
	scriptTagNames[QString("ogam")] = i18n("Ogham");
	scriptTagNames[QString("ital")] = i18n("Old Italic");
	scriptTagNames[QString("xpeo")] = i18n("Old Persian Cuneiform");
	scriptTagNames[QString("orya")] = i18n("Oriya");
	scriptTagNames[QString("osma")] = i18n("Osmanya");
	scriptTagNames[QString("phag")] = i18n("Phags-pa");
	scriptTagNames[QString("phnx")] = i18n("Phoenician");
	scriptTagNames[QString("runr")] = i18n("Runic");
	scriptTagNames[QString("shaw")] = i18n("Shavian");
	scriptTagNames[QString("sinh")] = i18n("Sinhala");
	scriptTagNames[QString("xsux")] = i18n("Sumero-Akkadian Cuneiform");
	scriptTagNames[QString("sylo")] = i18n("Syloti Nagri");
	scriptTagNames[QString("syrc")] = i18n("Syriac");
	scriptTagNames[QString("tglg")] = i18n("Tagalog");
	scriptTagNames[QString("tagb")] = i18n("Tagbanwa");
	scriptTagNames[QString("tale")] = i18n("Tai Le");
	scriptTagNames[QString("talu")] = i18n("Tai Lu");
	scriptTagNames[QString("taml")] = i18n("Tamil");
	scriptTagNames[QString("telu")] = i18n("Telugu");
	scriptTagNames[QString("thaa")] = i18n("Thaana");
	scriptTagNames[QString("thai")] = i18n("Thai");
	scriptTagNames[QString("tibt")] = i18n("Tibetan");
	scriptTagNames[QString("tfng")] = i18n("Tifinagh");
	scriptTagNames[QString("ugar")] = i18n("Ugaritic Cuneiform");
	scriptTagNames[QString("yi ")] = i18n("Yi");
}

const QMap< FMFontDb::InfoItem, QString >& FontStrings::Names()
{
	FontStrings *that(getInstance());
	return that->m_name;
}

const QMap< FontStrings::PanoseKey, QMap < int , QString > >& FontStrings::Panose()
{
	FontStrings *that(getInstance());
	return that->m_panoseMap;
}

const QString FontStrings::PanoseKeyName(PanoseKey pk)
{
	FontStrings *that(getInstance());
	return that->m_panoseKeyName.value(pk);
}

const QString FontStrings::PanoseKeyInfo(PanoseKey pk)
{
	FontStrings *that(getInstance());
	return that->m_panoseKeyInfo.value(pk);
}

const QString FontStrings::Encoding(FT_Encoding enc)
{
	FontStrings *that(getInstance());
	return that->charsetMap.value(enc);
}

const QMap< QString, QString > & FontStrings::Tables()
{
	FontStrings *that(getInstance());
	return that->tttableList;
}

QString FontStrings::FsType(int fstype_part, bool shortString)
{
	FontStrings *that(getInstance());
	if(!shortString)
		return that->m_FsType[fstype_part];
	else
	{
		if(FontItem::NOT_RESTRICTED == fstype_part)
			return i18n("Not Restricted");
		else if(FontItem::RESTRICTED == fstype_part)
			return i18n("Restricted");
		else if(FontItem::PREVIEW_PRINT == fstype_part)
			return i18n("Preview/Print");
		else if(FontItem::EDIT_EMBED == fstype_part)
			return i18n("Edit/Embed");
		else if(FontItem::NOSUBSET == fstype_part)
			return i18n("No Subset");
		else if(FontItem::BITMAP_ONLY == fstype_part)
			return i18n("Bitmap Only");
	}

	return QString();
}

FontStrings::PanoseKey FontStrings::firstPanoseKey()
{
	return FamilyType;
}

FontStrings::PanoseKey FontStrings::nextPanoseKey ( PanoseKey pk )
{
	switch ( pk )
	{
		case FamilyType:return SerifStyle;break;
		case SerifStyle:return Weight;break;
		case Weight:return Proportion;break;
		case Proportion:return Contrast;break;
		case Contrast:return StrokeVariation;break;
		case StrokeVariation:return ArmStyle;break;
		case ArmStyle:return Letterform;break;
		case Letterform:return Midline;break;
		case Midline:return XHeight;break;
		default:return InvalidPK;
	}
	return InvalidPK;
}

QString FontStrings::UnicodeCategory(QChar::Category cat)
{
	if(getInstance()->unicodeCategory.contains(cat))
		return getInstance()->unicodeCategory.value(cat);
	return QString();
}

QString FontStrings::scriptTagName(const QString &script)
{
	if(getInstance()->scriptTagNames.contains(script))
		return getInstance()->scriptTagNames.value(script);
	return script;
}

