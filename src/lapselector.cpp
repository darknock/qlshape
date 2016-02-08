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

#include "lapselector.h"
#include "effects.h"

LapSelector::LapSelector(QWidget * parent, Qt::WindowFlags f)
{
    Q_UNUSED(parent)
    Q_UNUSED(f)

    ui.setupUi(this);

    connect(ui.radioButton4, SIGNAL(clicked(bool)), this, SLOT(updateLap()));
    connect(ui.radioButton8, SIGNAL(clicked(bool)), this, SLOT(updateLap()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.checkBoxNeg, SIGNAL(stateChanged(int)), this, SLOT(updateLap()));
    connect(ui.checkBoxNor, SIGNAL(stateChanged(int)), this, SLOT(updateLap()));
    connect(ui.checkBoxBlur, SIGNAL(stateChanged(int)), this, SLOT(updateLap()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateLap()));
}

void LapSelector::updateLap()
{
    if (ui.checkBox->checkState() == Qt::Checked) {
        QImage *lapy = laplace(ui.previewLabel->preview(), mask(), blur(), negative(), normalize());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*lapy));
        delete lapy;
    }
}

void LapSelector::checkChange()
{
    if (ui.checkBox->checkState() != Qt::Checked)
        ui.previewLabel->refresh();
    else
        updateLap();
}

void LapSelector::setPreview(QImage* prev)
{
    ui.previewLabel->setPreview(prev);
    updateLap();
}
