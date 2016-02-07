/***************************************************************************
 *   Copyright (C) 2007 - 2016 by Konrad Ciekot                            *
 *   darknock@gmail.com                                                    *
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

#include "cannyselector.h"
#include "effects.h"

CannySelector::CannySelector(QWidget * parent, Qt::WindowFlags f) {
    ui.setupUi(this);

    connect(ui.spinBoxHi, SIGNAL(valueChanged(int)), this, SLOT(updateCannyHi()));
    connect(ui.spinBoxLow, SIGNAL(valueChanged(int)), this, SLOT(updateCannyLow()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.checkBoxBlur, SIGNAL(stateChanged(int)), this, SLOT(updateCannyHi()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateCannyHi()));
}

void CannySelector::updateCannyHi() {
    if (ui.checkBox->checkState() == Qt::Checked) {
        if(ui.spinBoxHi->value() < ui.spinBoxLow->value()) ui.spinBoxLow->setValue(ui.spinBoxHi->value());
        QImage *diffy = canny(ui.previewLabel->preview(), tholdHi(), tholdLow(), blur());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*diffy));
        delete diffy;
    }
}

void CannySelector::updateCannyLow() {
    if (ui.checkBox->checkState() == Qt::Checked) {
        if(ui.spinBoxHi->value() < ui.spinBoxLow->value()) ui.spinBoxHi->setValue(ui.spinBoxLow->value());
        QImage *diffy = canny(ui.previewLabel->preview(), tholdHi(), tholdLow(), blur());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*diffy));
        delete diffy;
    }
}

void CannySelector::checkChange() {
    if (ui.checkBox->checkState() != Qt::Checked) {
        ui.previewLabel->refresh();
    } else updateCannyHi();
}

void CannySelector::setPreview(QImage* prev) {
    ui.previewLabel->setPreview(prev);
    updateCannyHi();
}
