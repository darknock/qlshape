/***************************************************************************
 *   Copyright (C) 2007 by Konrad Ciekot                                   *
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

#ifndef CANNYSELECTOR_H
#define CANNYSELECTOR_H

#include "../ui_cannyselector.h"

using namespace Ui;

class CannySelector:public QDialog {
    Q_OBJECT

public:
    CannySelector(QWidget * parent = 0, Qt::WFlags f = 0);
    ~CannySelector() {}
    int tholdHi() const { return ui.spinBoxHi->value(); }
    int tholdLow() const { return ui.spinBoxLow->value(); }
    bool blur() const { return (ui.checkBoxBlur->checkState() == Qt::Checked); }
    void setPreview(QImage* prev);
public slots:
    void updateCannyHi();
    void updateCannyLow();
    void checkChange();
private:
    CannyDialog ui;
};

#endif //CANNYSELECTOR_H
