/***************************************************************************
 *   Copyright (C) 2007 by Konrad Ciekot   *
 *   darknock@o2.pl   *
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

#include "edgeselector.h"
#include "effects.h"

EdgeSelector::EdgeSelector(QWidget * parent, Qt::WFlags f) {
    ui.setupUi(this);

    ui.comboBox->addItem("Sobel (compass)");
    ui.comboBox->addItem("Previtt (compass)");
    ui.comboBox->addItem("Robinson");
    ui.comboBox->addItem("Kirsch");
    ui.comboBox->addItem("Differential");
    ui.comboBox->addItem("Sobel (gradient)");
    ui.comboBox->addItem("Previtt (gradient)");
    ui.comboBox->addItem("Roberts");

    connect(ui.horizontalSlider, SIGNAL(sliderReleased()), this, SLOT(updateEdge()));
    connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEdge()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.checkBoxNeg, SIGNAL(stateChanged(int)), this, SLOT(updateEdge()));
    connect(ui.checkBoxBin, SIGNAL(stateChanged(int)), this, SLOT(updateEdge()));
    connect(ui.checkBoxBlur, SIGNAL(stateChanged(int)), this, SLOT(updateEdge()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateEdge()));
}

void EdgeSelector::updateEdge() {
    if (ui.checkBox->checkState() == Qt::Checked) {
        QImage *edgy = edges(ui.previewLabel->preview(), filter(), thold(), blur(), negative(), binary());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*edgy));
        delete edgy;
    }
}

void EdgeSelector::checkChange() {
    if (ui.checkBox->checkState() != Qt::Checked) {
        ui.previewLabel->refresh();
    } else updateEdge();
}

void EdgeSelector::setPreview(QImage* prev) {
    ui.previewLabel->setPreview(prev);
    updateEdge();
}
