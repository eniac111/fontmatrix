/*
    SPDX-FileCopyrightText: 2008 Pierre Marchand <pierremarc@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "pangoshaper.h"

PangoShaper::PangoShaper(FMOtf * o, QString s)
	:FMBaseShaper(o,s)
{
}

PangoShaper::~ PangoShaper()
= default;

GlyphList PangoShaper::doShape(const QString & )
{
	return GlyphList();
}



