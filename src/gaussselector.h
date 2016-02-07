/***************************************************************************
 *   Copyright (C) 2007 - 2016 by Konrad Ciekot                                   *
 *   darknock@o2.pl                                                        *
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

#ifndef GAUSSSELECTOR_H
#define GAUSSSELECTOR_H

#include "ui_gaussselector.h"

using namespace Ui;

class GaussSelector:public QDialog {
    Q_OBJECT

public:
    GaussSelector(QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~GaussSelector() {}
    double radius() const { return ui.doubleSpinBox1->value(); }
    void setPreview(QImage* prev);
public slots:
    void updateGauss();
    void checkChange();
private:
    GaussDialog ui;
};

#endif //GAUSSSELECTOR_H
