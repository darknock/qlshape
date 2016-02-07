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

#include "aiselector.h"
#include "aieffects.h"
#include "net/hamming.h"
#include "net/pattern.h"

AiSelector::AiSelector(QWidget * parent, Qt::WFlags f) {
    ui.setupUi(this);
#ifdef Q_WS_X11
    darknocksBrain::PatternSet pSet("/usr/share/darknock/data/input");
#endif

#ifdef Q_WS_MAC
    darknocksBrain::PatternSet pSet("/usr/share/darknock/data/input");
#endif

#ifdef Q_WS_WIN
    darknocksBrain::PatternSet pSet("data/input");
#endif

    hamming = new darknocksBrain::Hamming(&pSet);

    connect(ui.horizontalSlider, SIGNAL(sliderReleased()), this, SLOT(updateAi()));
    connect(ui.checkBox, SIGNAL(stateChanged(int)), this, SLOT(checkChange()));
    connect(ui.checkBoxBlur, SIGNAL(stateChanged(int)), this, SLOT(checkBlurChange()));
    connect(ui.previewLabel, SIGNAL(dragingFinished()), this, SLOT(updateAi()));
}

void AiSelector::updateAi() {
    if (ui.checkBox->checkState() == Qt::Checked) {
        QImage *ai = previewHammingAiEdges(ui.previewLabel->preview(), new std::vector<double>(*d), ui.previewLabel->borders(), xMin(), xMax(), hammingNet(), thold(), blur());
        ui.previewLabel->setPixmap(QPixmap::fromImage(*ai));
        delete ai;
    }
}

void AiSelector::checkChange() {
    if (ui.checkBox->checkState() != Qt::Checked) {
        ui.previewLabel->refresh();
    } else updateAi();
}

void AiSelector::checkBlurChange() {
    if(!d) delete d;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    d = derivative(prev, blur());
    min = d->at(0), max = d->at(0);
    for( unsigned i = 0; i < d->size(); ++i) {
        min = min > d->at(i) ? d->at(i) : min;
        max = max < d->at(i) ? d->at(i) : max;
    }
    updateAi();
    QApplication::restoreOverrideCursor();
}

void AiSelector::setPreview(QImage* prev) {
    this->prev = prev;
    ui.previewLabel->setPreview(prev);
    checkBlurChange();
}

AiSelector::~AiSelector() { 
    delete hamming;
    delete d;
}

