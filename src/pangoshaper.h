/*
    SPDX-FileCopyrightText: 2008 Pierre Marchand <pierremarc@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PANGOSHAPER_H
#define PANGOSHAPER_H

#include "fmbaseshaper.h"

class PangoShaper : public FMBaseShaper
{
public:
    PangoShaper(FMOtf *o, QString s);
    ~PangoShaper() override;
    GlyphList doShape(const QString &s) override;

private:
    Q_DISABLE_COPY(PangoShaper)
};

#endif
