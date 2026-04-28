//
// C++ Interface: fmpdffontextractor
//
// Description: A dead simple font extractor from PDF docs
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FMPDFFONTEXTRACTOR
#define FMPDFFONTEXTRACTOR

#include <memory>
#include <QMap>

#include "fmfontextractorbase.h"

#include <podofo/podofo.h>

class FMPDFFontExtractor : public FMFontExtractorBase
{
	public:
		FMPDFFontExtractor();
		~FMPDFFontExtractor() override;

		bool loadFile(const QString& filePath) override;
		QStringList extensions() override;

		QStringList list() override;
		QString fontType(const QString& name) override;
		bool write(const QString& name, QIODevice * openedDevice) override;

	private:
		bool cachedList;
		std::unique_ptr<PoDoFo::PdfMemDocument> document;
		QMap<QString, PoDoFo::PdfObject*> mfont;
		QMap<QString, QString> mType;

};

#endif // FMPDFFONTEXTRACTOR

