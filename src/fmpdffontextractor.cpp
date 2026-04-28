//
// C++ Implementation: fmpdffontextractor
//
// Description:
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "fmpdffontextractor.h"

#include <QDebug>
#include <QFile>

static PoDoFo::PdfObject* resolveRef(PoDoFo::PdfObject* obj, PoDoFo::PdfMemDocument& doc)
{
	if (!obj || !obj->IsReference())
		return obj;
	return doc.GetObjects().GetObject(obj->GetReference());
}

FMPDFFontExtractor::FMPDFFontExtractor()
	: cachedList(false)
{
}

FMPDFFontExtractor::~FMPDFFontExtractor()
{
}

bool FMPDFFontExtractor::loadFile(const QString & filePath)
{
	cachedList = false;
	mfont.clear();
	mType.clear();
	document.reset();

	if (!QFile::exists(filePath))
		return false;

	try
	{
		document = std::make_unique<PoDoFo::PdfMemDocument>();
		document->Load(filePath.toLocal8Bit().toStdString());
	}
	catch (PoDoFo::PdfError& e)
	{
		qDebug() << "PoDoFo::Error:" << e.what();
		qDebug() << "Unable to process the PDF file" << filePath;
		document.reset();
		return false;
	}
	return true;
}

QStringList FMPDFFontExtractor::extensions()
{
	QStringList ret;
	ret << "pdf" << "PDF";
	return ret;
}

QStringList FMPDFFontExtractor::list()
{
	if (!document)
		return mfont.keys();

	if (cachedList)
		return mfont.keys();
	else
		cachedList = true;

	for (auto obj : document->GetObjects())
	{
		if (!obj->IsDictionary())
			continue;

		auto* typeObj = obj->GetDictionary().FindKey("Type");
		if (!typeObj || !typeObj->IsName())
			continue;
		if (typeObj->GetName() != "Font")
			continue;

		auto* subtypeObj = obj->GetDictionary().FindKey("Subtype");
		if (!subtypeObj || !subtypeObj->IsName())
			continue;
		const PoDoFo::PdfName& subtype = subtypeObj->GetName();
		if (subtype != "Type1" && subtype != "TrueType")
			continue;

		PoDoFo::PdfObject* fontDescriptor = resolveRef(
			obj->GetDictionary().FindKey("FontDescriptor"), *document);
		if (!fontDescriptor || !fontDescriptor->IsDictionary())
			continue;

		PoDoFo::PdfObject* fontFile = resolveRef(
			fontDescriptor->GetDictionary().FindKey("FontFile"), *document);
		bool hasFile = (fontFile != nullptr);
		if (!hasFile)
		{
			fontFile = resolveRef(
				fontDescriptor->GetDictionary().FindKey("FontFile3"), *document);
			if (fontFile)
				hasFile = true;
			else
				qWarning("Font not embedded not supported yet");
		}

		if (!hasFile)
			continue;

		auto* fnObj = fontDescriptor->GetDictionary().FindKey("FontName");
		if (!fnObj || !fnObj->IsName())
			continue;

		QString n = QString::fromStdString(std::string(fnObj->GetName().GetString()));
		mfont[n] = fontFile;
		mType[n] = (subtype == "Type1") ? "pfb" : "ttf";
	}

	return mfont.keys();
}

bool FMPDFFontExtractor::write(const QString & name, QIODevice* openedDevice)
{
	if (!mfont.contains(name))
		return false;

	PoDoFo::PdfObject* fontFile = mfont[name];
	auto* stream = fontFile->GetStream();
	if (!stream)
		return false;

	try
	{
		PoDoFo::charbuff buffer = stream->GetCopy();
		QByteArray a(buffer.data(), static_cast<int>(buffer.size()));
		return (openedDevice->write(a) == static_cast<qint64>(buffer.size()));
	}
	catch (PoDoFo::PdfError& e)
	{
		qDebug() << "Unable to get font stream copy:" << e.what();
		return false;
	}
}

QString FMPDFFontExtractor::fontType(const QString & name)
{
	return mType.value(name);
}
