//
// C++ Interface: fmshaper
//
// Description: in fact represents the Harfbuzz shaper
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FMSHAPER_H
#define FMSHAPER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_TABLES_H

#include <QString>
#include <QStringList>
#include "fmotf.h" 

/**
	@author Pierre Marchand <pierremarc@oep-h.com>

	Text shaped the way an application does it: HarfBuzz applies the features
	the script asks for. FMOtf, which owns the font, does the work.
*/
class FMShaper
{
	public:
		explicit FMShaper(FMOtf *anchor);

		~FMShaper();

		/* An OpenType script tag: "arab", "deva". Returns false for an
		   empty one, HarfBuzz then shapes without a script. */
		bool setScript ( QString script );

		QList<RenderedGlyph> doShape(QString string , bool ltr);

	private:
		// the anchor owns the HarfBuzz font, nothing to copy here
		Q_DISABLE_COPY ( FMShaper )
		FMOtf *anchorOTF = nullptr;
		QString m_script;

};

#endif
