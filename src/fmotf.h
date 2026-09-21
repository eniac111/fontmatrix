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


#ifndef WRAPLIBOTF
#define WRAPLIBOTF

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H


#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>

#include "fmshaper_own.h"
#include "fmsharestruct.h"

#include <hb.h>
#include <hb-ot.h>


/**
 * The OpenType side of a font: which scripts, languages and features its
 * GSUB and GPOS tables offer, and text shaped with a chosen set of them.
 * HarfBuzz does the work.
 *
 * Shaping here answers "what does this feature do": a feature that is not
 * asked for is switched off, including those a script gets by default in
 * applications. shape() is the exception and behaves like an application.
 *
 * Results are in font units and in logical order, whatever the script.
 */
class FMOtf
{
	public:
		// scale is not used any more, positions are always in font units
		FMOtf ( FT_Face, double scale = 0.0 );
		~FMOtf ();

		QString curString;

	private:
		// owns the HarfBuzz face and fonts
		Q_DISABLE_COPY ( FMOtf )
		FT_Face _face;
		hb_face_t *hbFace = nullptr;
		// glyphs are looked up by FreeType, with the charmap the application selected
		hb_font_t *hbFont = nullptr;
		// for input that is glyph indices already
		hb_font_t *hbGlyphFont = nullptr;

		bool GSUB, GPOS;
		// all the feature tags of the font, to switch off those not asked for
		QList<hb_tag_t> fontFeatures;
		QList<unsigned int> lastGlyphs;
		GlyphList m_lastRun;

		hb_tag_t tableTag() const;
		bool currentScript ( unsigned int *scriptIndex ) const;
		unsigned int currentLanguage ( unsigned int scriptIndex ) const;
		QList<hb_feature_t> featureList ( const QStringList& enabled ) const;
		GlyphList shapeBuffer ( hb_buffer_t *buffer, hb_font_t *font, const QString& script, const QString& lang,
		                        const QList<hb_feature_t>& features, bool ltr = true );
		void collectAlternates ( const QString& s, const QString& script, const QString& lang );

	public:
		// glyph index at a position of the last shaped run
		int get_glyph ( int index );
		QString curTable;
		QString curScriptName, curLangName;
		QStringList curFeatures;

		// All the alternates 'aalt' offers for the text of the last procstring()
		static QList<int> altGlyphs;
		/*
		 * These members functions apply features currently set
		 */
	public:
		// Yes there are a lot, doubtless too much.
		int procstring ( QString s, QString script, QString lang, QStringList gsub, QStringList gpos );
		QList<RenderedGlyph> procstring ( QString s, OTFSet set );
		// each character carries the features that apply to it alone
		QList<RenderedGlyph> procstring ( QList<Character> shaped , QString script );
		// input is glyph indices, not characters
		QList<RenderedGlyph> procstring ( QList<unsigned int> glyList , QString script, QString lang, QStringList gsub, QStringList gpos );

		// The way an application shapes: the script decides the features
		QList<RenderedGlyph> shape ( const QString& s, const QString& script, bool ltr );

		/*
		  * These functions give access to informations contained in the fontfile
		 */
		QStringList   get_tables ();
		QStringList   get_scripts ();
		QStringList   get_langs ();
		QStringList   get_features ( bool required=false );
		/*
		 * These allow to set up the features ( Tab -> Scr -> Lan -> Fea )
		 */
		void set_table ( QString );
		void set_script ( QString );
		void set_lang ( QString );
		void set_features ( QStringList );

	FT_Face face() const
	{
		return _face;
	}

		friend class FontItem;
};

#endif
