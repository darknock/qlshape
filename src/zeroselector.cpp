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

#include "zeroselector.h"
#include "effects.h"

ZeroSelector::ZeroSelector(QWidget * parent, Qt::WindowFlags f) {
    ui.setupUi(this);
    connect(ui.horizontalSlider, SIGNAL(sliderReleased()), this, SLOT(updateZero()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.checkBoxBlur, SIGNAL(stateChanged(int)), this, SLOT(updateZero()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateZero()));
}

void ZeroSelector::updateZero() {
    if (ui.checkBox->checkState() == Qt::Checked) {
        QImage *zero = zeroCrossing(ui.previewLabel->preview(), blur(), thold());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*zero));
        delete zero;
    }
}

void ZeroSelector::checkChange() {
    if (ui.checkBox->checkState() != Qt::Checked) {
        ui.previewLabel->refresh();
    } else updateZero();
}

void ZeroSelector::setPreview(QImage* prev) {
    ui.previewLabel->setPreview(prev);
    updateZero();
}
