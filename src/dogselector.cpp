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

#include "dogselector.h"
#include "effects.h"

DogSelector::DogSelector(QWidget * parent, Qt::WindowFlags f)
{
    Q_UNUSED(parent)
    Q_UNUSED(f)
    ui.setupUi(this);

    connect(ui.doubleSpinBox1, SIGNAL(valueChanged(double)), this, SLOT(updateDog()));
    connect(ui.doubleSpinBox2, SIGNAL(valueChanged(double)), this, SLOT(updateDog()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.checkBoxNeg, SIGNAL(stateChanged(int)), this, SLOT(updateDog()));
    connect(ui.checkBoxNor, SIGNAL(stateChanged(int)), this, SLOT(updateDog()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateDog()));
}

void DogSelector::updateDog()
{
    if (ui.checkBox->checkState() == Qt::Checked) {
        QImage *diffy = diffOfGaussian(ui.previewLabel->preview(), radius1(), radius2(), negative(), normalize());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*diffy));
        delete diffy;
    }
}

void DogSelector::checkChange()
{
    if (ui.checkBox->checkState() != Qt::Checked) {
        ui.previewLabel->refresh();
    } else updateDog();
}

void DogSelector::setPreview(QImage* prev)
{
    ui.previewLabel->setPreview(prev);
    updateDog();
}
