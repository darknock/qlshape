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

#include "gaussselector.h"
#include "effects.h"

GaussSelector::GaussSelector(QWidget * parent, Qt::WindowFlags f)
{
    Q_UNUSED(parent)
    Q_UNUSED(f)

    ui.setupUi(this);

    connect(ui.doubleSpinBox1, SIGNAL(valueChanged(double)), this, SLOT(updateGauss()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateGauss()));
}

void GaussSelector::updateGauss()
{
    if (ui.checkBox->checkState() == Qt::Checked) {
        QImage *gaussy = gaussianBlur(ui.previewLabel->preview(), radius());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*gaussy));
        delete gaussy;
    }
}

void GaussSelector::checkChange()
{
    if (ui.checkBox->checkState() != Qt::Checked)
        ui.previewLabel->refresh();
    else
        updateGauss();
}

void GaussSelector::setPreview(QImage* prev)
{
    ui.previewLabel->setPreview(prev);
    updateGauss();
}
