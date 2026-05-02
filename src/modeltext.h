//
// C++ Interface: modeltext
//
// Description: 
//
//
// Author: Pierre Marchand <pierremarc@oep-h.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef MODELTEXT_H
#define MODELTEXT_H

#include <QTextEdit>


class ModelText : public QTextEdit
{
	Q_OBJECT
	public:
		ModelText(QWidget * parent);
		~ModelText() override{}
		
	protected:
		bool canInsertFromMimeData( const QMimeData *source ) const override;
		void insertFromMimeData ( const QMimeData * source ) override ;
		
	signals:
		void insertContent();
};

#endif // MODELTEXT_H
