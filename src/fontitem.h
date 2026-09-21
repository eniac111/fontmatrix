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
#ifndef FONTITEM_H
#define FONTITEM_H

#include <functional>

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>
#include <QPointF>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QIcon>
#include <QPixmap>
#include <QUrl>
#include <QFlags>
// #include <QThread>
#include <QGraphicsItem>
#include <QRectF>
#include <QVariant>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "fmsharestruct.h"

class QGraphicsPixmapItem;
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsTextItem;
struct OTFSet;
class FMOtf;
class QGraphicsView;
class QGraphicsObject;

class QProgressDialog;
class QFile;




constexpr int PROGRESSION_LTR = 0;
constexpr int PROGRESSION_RTL = 2;
constexpr int PROGRESSION_TTB = 4;
constexpr int PROGRESSION_BTT = 8;

constexpr int GLYPH_DATA_GLYPH = 1;
constexpr int GLYPH_DATA_BITMAPLEFT = 2;
constexpr int GLYPH_DATA_BITMAPTOP = 3;
constexpr int GLYPH_DATA_HADVANCE = 4;
constexpr int GLYPH_DATA_VADVANCE = 5;
constexpr int GLYPH_DATA_HADVANCE_SCALED = 6;
constexpr int GLYPH_DATA_ERROR = 7;
constexpr int GLYPH_DATA_FONTNAME = 100;


/**
	@author Pierre Marchand <pierre@oep-h.com>

	(reminder) glyph data -> 1 = index, 2 = charcode
*/
struct FontLocalInfo
{
	QString file;
	QString family;
	QString variant;
	QString type;
	QString panose;
// 	QMap<int,QMap<QString, QString> > info;
	QStringList tags;
	QPixmap pix;
};

class MetaGlyphItem : public QGraphicsItem
{
	QMap<int, QVariant> m_Data;
public:
	void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override{}
	[[nodiscard]] QRectF boundingRect() const override {return QRectF();}
	void setMetaData(int key, const QVariant &value)
	{
		m_Data.insert(key,value);
	}
	[[nodiscard]] QVariant metaData(int key) const
	{
		return m_Data.value(key);
	}
};

class FontItem : public QObject
{
		Q_OBJECT
		Q_PROPERTY(QString Family READ family)
		Q_PROPERTY(QString Variant READ variant)
		Q_PROPERTY(QString Path READ path)
		Q_PROPERTY(QStringList Tags READ tags WRITE setTags)
		Q_PROPERTY(bool OpenType READ isOpenType)
		Q_PROPERTY(bool Active READ isActivated WRITE setActivated)
		Q_PROPERTY(int GlyphsCount READ glyphsCount)
				
	public:
		enum FsType
		{
		    NOT_RESTRICTED	= 0x0000,
		    RESTRICTED		= 0x0002,
		    PREVIEW_PRINT	= 0x0004,
		    EDIT_EMBED		= 0x0008,
		    NOSUBSET		= 0x0100,
		    BITMAP_ONLY		= 0x0200
		};
				
		explicit FontItem ( QString path , bool remote = false, bool faststart = false);
		FontItem (QString path,  QString family, QString variant, QString type , bool active);
		FontItem * Clone();
		/** Needed when the item has been instantiate with "faststart=true" */
		void updateItem();
		~FontItem() override;
		
		
		static QList<int> legitimateNonPathChars;
		void fillLegitimateSpaces();
		
	private:
		bool isUpToDate;
		bool m_valid;
		bool m_active;
		
		bool m_remote;
		bool remoteCached;
		QString remoteHerePath;
		bool stopperDownload;
// 		QHttp *rHttp; // TODO To be replaced
		QFile *rFile = nullptr;
		int remoteId = 0;
		QProgressDialog *rProgressDialog = nullptr;
		
		QString m_path;
		QUrl m_url;
		QString m_afm;
		QString m_name;
		// Basically, we collect all infos that are in an FT_FaceRec
		QString m_faceFlags;
		QString m_type;
		QString m_styleFlags;
		QString m_family;
		QString m_variant;
		QString m_fileSize;
		double m_size = 0.0;
		int m_numGlyphs = 0;
		int m_numFaces = 0;
// 		QString m_panose;
		double unitPerEm;
		
		
		
		QString getAlternateFamilyName();
		QString getAlternateVariantName();
		QList<FT_Encoding> m_charsets;
		QList<int> spaceIndex;
		
		bool m_isOpenType;
		FMOtf *otf = nullptr;
		// words, spaces and hyphenation around whatever shapes one word
		GlyphList shapeWords(const QString& spec, double fsize, const std::function<GlyphList(const QString&)>& shapeWord);

//		FT_Library theLibrary;
		FT_Face	m_face;
		FT_Face lastFace;
		FT_Error      ft_error = 0;
//		QMap<FT_Library,FT_Face> faces;
		int facesRef = 0;
		FT_GlyphSlot m_glyph = nullptr;
		

		QImage glyphImage(QColor color = Qt::black);
		void fill256Palette();
		void fillInvertedPalette();
		
		bool m_rasterFreetype;
		unsigned int m_FTHintMode;
// 		unsigned int m_FTRenderMode;not yet implemented

		bool ensureFace();
		void releaseFace();
		void encodeFace();
		
		QList<int> getAlternates(int ccode);
		QString panose();

		QString testFlag ( long flag , long against, QString yes, QString no );
		QByteArray pixarray ( uchar *b, int len );


		QList<QGraphicsPixmapItem *> pixList;
		QList<QGraphicsPathItem*> glyphList;
		QList<QGraphicsTextItem*> labList;
		QList<QGraphicsRectItem*> selList;
		QMap<int, QGraphicsPixmapItem*> fancyGlyphs;
		QMap<int, QGraphicsTextItem*> fancyTexts;
		QMap<int, QList<QGraphicsPixmapItem*> > fancyAlternates;

		
		bool allIsRendered;
		bool isDerendered = false;
		int m_glyphsPerRow;
		bool m_isEncoded;
		bool m_unicodeBuiltIn = false;
		FT_Encoding m_currentEncoding = FT_ENCODING_NONE;
		int currentChar;
		
// 		bool m_RTL;// Right to Left
// 		bool m_VertUD;// Vertical Up Down
		int m_progression;
		
// 		bool m_lock;
		
		QMap<int,double> advanceCache;

//		QIcon theOneLinePreviewIcon;
//		QPixmap theOneLinePreviewPixmap;

		static QGraphicsScene *theOneLineScene;
		
		
		FontInfoMap moreInfo_sfnt();
		FontInfoMap moreInfo_type1();

// 		FontInfoMap moreInfo;
// 		QMap<QString, QString> panoseInfo;
		
		int m_shaperType;
		
		// if true return width, else return number of _chars_ consumed
		bool renderReturnWidth;
		
		
	private Q_SLOTS:
		void slotDownloadStart(int id);
		void slotDowloadProgress(int done, int total );
		void slotDownloadEnd(int id, bool error );
		void slotDownloadDone(bool error);
		
		void slotDownloadState(int state);
		
		
	Q_SIGNALS:
		void dowloadFinished();
		
	public Q_SLOTS:
		QString renderSVG(const QString& s, const double& size);
		int countCoverage ( int begin_code, int end_code );
		bool hasCharcode(int cc);
		bool hasChars(const QString& s);
		int firstChar();
		int lastChar();
		int countChars();
		int nextChar(int from, int offset = 1);
		unsigned short getNamedChar(const QString& name);
		QStringList getNames();
		
		/// We prepare ejection of renderLine methods
		GlyphList glyphs(QString spec, double fsize);
		GlyphList glyphs(QString spec, double fsize, OTFSet set);
		// Shaped the way applications do, with the features the script asks for
		GlyphList glyphsShaped(const QString& spec, double fsize);
		GlyphList glyphs(QString spec, double fsize, QString script);
		
		// experiences go there
		void exploreKernFeature();
		
	public:

		[[nodiscard]] QString path() const {return m_path;}
		[[nodiscard]] QString afm() const {return m_afm;}
		void setAfm ( QString apath ) {m_afm = apath;}
		[[nodiscard]] QString faceFlags() const {return m_faceFlags;}
		[[nodiscard]] QString family() const {return m_family;}
		[[nodiscard]] QString variant() const {return m_variant;}
		[[nodiscard]] QStringList tags() const  ;
		[[nodiscard]] int glyphsCount() const;
		QString type(){return m_type;}
		QStringList charmaps();
		void setTags ( QStringList l );
		void addTag(const QString &t);
		QString name();
		QString fancyName() {return m_family + QLatin1Char ( ' ' ) + m_variant;}
		QString infoGlyph ( int index, int code = 0 );
		QString glyphName(int codepoint, bool codeIsChar = true);
		FontInfoMap rawInfo();
		FontInfoMap moreInfo();
		FsType getFsType();
		QStringList supportedLangDeclaration();
		double italicAngle();
		
		// Return the length of a TT table
		int table(const QString& tableName);
		QByteArray tableData(const QString& tableName);

// 		QString value ( QString k );
// 		QString panose( QString k );

		double renderLine ( QGraphicsScene *scene, QString spec,  QPointF origine, double lineWidth, double fsize, double zindex = 100.0);
		double renderLine ( OTFSet set, QGraphicsScene *scene, QString spec,  QPointF origine, double lineWidth,double fsize);
		double renderLine ( QString script, QGraphicsScene *scene, QString spec,  QPointF origine, double lineWidth,double fsize);
		QGraphicsPathItem* itemFromChar ( int charcode, double size );
		QGraphicsPathItem* itemFromGindex ( int index, double size );
		
		QGraphicsPixmapItem* itemFromCharPix ( int charcode, double size);
		QGraphicsPixmapItem* itemFromGindexPix ( int index, double size);
		// cant have qpixmap outside main thread and QGraphicsPixmapItem create at least  a null one when instantiated
		MetaGlyphItem* itemFromGindexPix_mt ( int index, double size );
		
		QImage charImage(int charcode, double size);
		QImage glyphImage(int index, double size);
		
		
		
		void renderAll ( QGraphicsScene *scene, int begin_code, int end_code );
		//return count codes that remain
		int renderChart(QGraphicsScene *scene, int begin_code, int end_code ,double pwidth, double pheight);

		void deRenderAll();
		
		//Return a ref that will be asked for destroy the element. -1 if failed
		int showFancyGlyph(QGraphicsView *view, int charcode, bool charcodeIsAGlyphIndex = false);
		void hideFancyGlyph(int ref);
		
		QString toElement();
		
		QGraphicsPathItem* hasCodepointLoaded ( int code );
		
		void trimSpacesIndex();
		
		QString activationName();
		QString activationAFMName();

// 		QIcon oneLinePreviewIcon ( QString oneline );
		QPixmap oneLinePreviewPixmap ( QString oneline , QColor fg_color, QColor bg_color, int size_w = 0, int fsize = 0);
		void clearPreview();

		[[nodiscard]] bool isActivated() const;
		void setActivated ( bool act );
		
		bool isLocal();

		// Relative to fontactionwidget
// 		void lock() {m_lock=true;};
// 		void unLock() {m_lock=false;};
// 		bool isLocked() {return m_lock;};

// 		int debug_size();

		void adjustGlyphsPerRow ( int width );
		bool isOpenType(){return m_isOpenType;}
		FMOtf *takeOTFInstance();
		void releaseOTFInstance(FMOtf * rotf);
		// Returns a flat list of OT features
		QStringList features();
		
		void setFTRaster(bool f){m_rasterFreetype = f;}
		[[nodiscard]] bool rasterFreetype()const{return m_rasterFreetype;}
		
		void setProgression(int p){m_progression = p;}
		[[nodiscard]] int progression()const{return m_progression;}
		
		// sfnt names
// 		static void fillNamesMeaning();
// // 		static void fillPanoseMap();
		static void fillFSftypeMap();
		
		bool isValid(){return m_valid;}
		
		bool isRemote(){return m_remote;}
		bool isCached(){return remoteCached;}		
		void  fileRemote(QString family, QString variant, QString type, QString info, QPixmap pixmap);
		void  fileLocal(QString family, QString variant, QString type, QString p);
		void  fileLocal(FontLocalInfo);
		// retval : 1 => Ready; 2 => Wait ; ...
		int getFromNetwork();

	void setShaperType ( int theValue );
	[[nodiscard]] int shaperType() const;
	
	void setRenderReturnWidth ( bool theValue )
	{
		renderReturnWidth = theValue;
	}

	double getUnitPerEm();
	void setFTHintMode ( unsigned int theValue );
	[[nodiscard]] unsigned int getFTHintMode() const;
	
	void dumpIntoDB();

	[[nodiscard]] bool getUnicodeBuiltIn() const;

	[[nodiscard]] FT_Encoding getCurrentEncoding() const;

	[[nodiscard]] double getUnitPerEm() const;

	[[nodiscard]] QList< FT_Encoding > getCharsets() const;
	
	
	
	
	
	
};

		 
#endif
