//
// C++ Interface: harfbuzzqtshaper
//
// Description: 
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef HARFBUZZSHAPER_H
#define HARFBUZZSHAPER_H

#include "fmbaseshaper.h"
#include "fmshaper.h"

class HarfbuzzShaper : public FMBaseShaper
{
	public:
		HarfbuzzShaper(FMOtf* o, QString s);
		~HarfbuzzShaper();	
		
		GlyphList doShape( const QString& s ) override;
	private:
		FMShaper *hbqtsh;
};

#endif

