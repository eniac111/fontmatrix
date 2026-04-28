/***************************************************************************
 *   Copyright (C) 2010 by Pierre Marchand   *
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

#include "floatingwidget.h"
#include "floatingwidgetsregister.h"
#include "typotek.h"
#include "fmfontdb.h"
#include "fontitem.h"

FloatingWidget::FloatingWidget(const QString &f, const QString& typ, QWidget *parent) :
		QWidget(parent),
		fName(f),
		fType(typ),
		printer(nullptr),
		printDialog(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);
	QString fn;
	FontItem *fi(FMFontDb::DB()->Font(fName));
	if(fi != nullptr)
		fn = fi->fancyName();
	else
		fn = f;
	actionName =  QString("[%1]").arg(fType) + QString(" ") + fn;
	wTitle =  fn + QString(" - Fontmatrix");
	if(nullptr == parent)
	{
		ddetach();
	}
}

FloatingWidget::~FloatingWidget()
{
	if(printer)
		delete printer;
	if(printDialog)
		delete printDialog;
}


bool FloatingWidget::event(QEvent *e)
{
	//	if(windowTitle().isEmpty())
	//	{
	//		QWidget::setWindowTitle(wTitle);
	//	}
	if((e->type() == QEvent::Show) || (e->type() == QEvent::Hide))
		emit visibilityChange();

	return QWidget::event(e);
}

void FloatingWidget::activate(bool a)
{
	if(a)
	{
		if(!isVisible())
			setVisible(true);
		raise();
	}
	else
		hide();
}


void FloatingWidget::ddetach()
{
	if(nullptr != parent())
		setParent(nullptr, Qt::Window);
	setWindowTitle(wTitle);
	FloatingWidgetsRegister::Register(this, fName, fType);
	show();
	emit detached();
}
