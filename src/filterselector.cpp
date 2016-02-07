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

#include "filterselector.h"
#include <QDir>
#include <QFile>
#include <QLineEdit>
#include <QMessageBox>

FilterSelector::FilterSelector(QWidget * parent, Qt::WFlags f) {

    ui.setupUi(this);
    fixSize();

    editDialog = new QDialog(this);
    editDialog->setFixedSize(110, 30);
    lineEdit = new QLineEdit(editDialog);
    editDialog->setWindowTitle(tr("Filter name"));

    connect(ui.checkBox, SIGNAL(clicked(bool)), this, SLOT(customClicked(bool)));
    connect(ui.spinBox, SIGNAL(valueChanged(int)), this, SLOT(setRows(int)));
    connect(ui.spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(setCols(int)));
    connect(ui.comboBox, SIGNAL(activated(int)), this, SLOT(selectFilter(int)));
    connect(ui.tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(check()));
    connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClick()));
    connect(ui.deleteButton, SIGNAL(clicked()), this, SLOT(delButtonClick()));
    connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(finishEditing()));

    dataFile = QDir::homePath() + "/.darknock/data/filters";
    QFileInfo fileInfo(dataFile);
    QDir dir(fileInfo.absoluteDir());
    if(!dir.exists()) dir.mkpath(fileInfo.absolutePath());

    QFile file(dataFile);
    file.open(QFile::ReadOnly);
    QDataStream in(&file);
    in >> customFilters >> customFiltersData;

    filters << "Mean removal" << "HP1" << "HP2" << "HP3"
            << "Mean" << "LP1" << "LP2" << "Gauss";
    ui.comboBox->addItems(filters);
    selectFilter(0);
}

void FilterSelector::saveData() {
    QFile file(dataFile);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::information(this, tr("darknocK"), tr("Failed to open/create file %1: %2").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    QDataStream out(&file);
    out << customFilters << customFiltersData;
}

void FilterSelector::finishEditing() {
    editDialog->accept();
}

void FilterSelector::saveButtonClick() {
    lineEdit->setText(tr("custom%1").arg(ui.comboBox->count() + 1));
    lineEdit->selectAll();
    if(editDialog->exec()) {
        QVector<qint32> temp;
        temp << rows() << cols();
        for (int j = 0; j < ui.tableWidget->rowCount(); j++)
            for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
                temp << (qint32)ui.tableWidget->item(j, i)->text().toInt();
            }
        customFiltersData.append(temp);
        customFilters.append(lineEdit->text());
        ui.comboBox->addItem(lineEdit->text());
    }
}

void FilterSelector::delButtonClick() {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("darknocK"),
                               tr("Do You really want to delete \"%1\" mask?").arg(ui.comboBox->currentText()),
                                  QMessageBox::Yes | QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        int i = ui.comboBox->currentIndex();
        customFilters.removeAt(i);
        ui.comboBox->clear();
        ui.comboBox->addItems(customFilters);
        customFiltersData.remove(i);
    }
}

void FilterSelector::setMask(int *f, int rowCount, int columnCount) {
    QTableWidgetItem *item;
    ui.tableWidget->setRowCount(rowCount);
    ui.tableWidget->setColumnCount(columnCount);
    for (int j = 0; j < ui.tableWidget->rowCount(); j++)
        for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
            item = new QTableWidgetItem(tr("%1").arg(f[i + j * columnCount]));
            ui.tableWidget->setItem(j, i, item);
        }
   fixSize();
}

void FilterSelector::selectFilter(int i) {
    switch(i) {
        case 0: {   //Mean-removal
            int mr[] = {
                -1,-1,-1,
                -1, 9,-1,
                -1,-1,-1
            };
            setMask(mr, 3, 3);
        } break;
        case 1: {   //HP1
            int hp1[] = {
                 0,-1, 0,
                -1, 5,-1,
                 0,-1, 0
            };
            setMask(hp1, 3, 3);
        } break;
        case 2: {   //HP2
            int hp2[] = {
                 1,-2, 1,
                -2, 5,-2,
                 1,-2, 1
            };
            setMask(hp2, 3, 3);
        } break;
        case 3: {   //HP3
            int hp3[] = {
                 0,-1, 0,
                -1,20,-1,
                 0,-1, 0
            };
            setMask(hp3, 3, 3);
        } break;
        case 4: {   //Mean
            int m[] = {
                1, 1, 1,
                1, 1, 1,
                1, 1, 1
            };
            setMask(m, 3, 3);
        } break;
        case 5: {   //LP1
            int lp1[] = {
                1, 1, 1,
                1, 2, 1,
                1, 1, 1
            };
            setMask(lp1, 3, 3);
        } break;
        case 6: {   //LP2
            int lp2[] = {
                1, 1, 1,
                1, 4, 1,
                1, 1, 1
            };
            setMask(lp2, 3, 3);
        } break;
        case 7: {   //Gauss
            int g[] = {
                1, 2, 1,
                2, 4, 2,
                1, 2, 1
            };
            setMask(g, 3, 3);
        } break;
    }
    check();
}

void FilterSelector::selectCustomFilter(int i) {
    QVector<qint32> temp = customFiltersData[i];
    int rowCount, columnCount;
    rowCount = temp[0];
    columnCount = temp[1];
    temp.erase(temp.begin());
    temp.erase(temp.begin());
    setMask(temp.data(), rowCount, columnCount);
}

double* FilterSelector::mask() {
    double *f = new double[ui.tableWidget->columnCount() * ui.tableWidget->rowCount()];
    for (int j = 0; j < ui.tableWidget->rowCount(); j++)
        for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
            f[i + j * ui.tableWidget->columnCount()] = ui.tableWidget->item(j, i)->text().toInt();
        }
    return f;
}

void FilterSelector::customClicked(bool checked) {
    if(!checked) {
        disconnect(ui.comboBox, SIGNAL(activated(int)), this, SLOT(selectCustomFilter(int)));
        connect(ui.comboBox, SIGNAL(activated(int)), this, SLOT(selectFilter(int)));
        ui.comboBox->clear();
        ui.comboBox->addItems(filters);
        ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui.tableWidget->clear();
        selectFilter(ui.comboBox->currentIndex());
    } else {
        disconnect(ui.comboBox, SIGNAL(activated(int)), this, SLOT(selectFilter(int)));
        connect(ui.comboBox, SIGNAL(activated(int)), this, SLOT(selectCustomFilter(int)));
        ui.tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
        ui.tableWidget->setRowCount(ui.spinBox->value());
        ui.tableWidget->setColumnCount(ui.spinBox_2->value());
        ui.comboBox->clear();
        ui.comboBox->addItems(customFilters);
    }
    fixSize();
    check();
}

void FilterSelector::setRows(int rows) {
    ui.tableWidget->setRowCount(rows);
    if(ui.checkBox_2->checkState() == Qt::Checked) {
        ui.tableWidget->setColumnCount(rows);
        ui.spinBox_2->setValue(rows);
    }
    fixSize();
    check();
}

void FilterSelector::setCols(int cols) {
    ui.tableWidget->setColumnCount(cols);
    if(ui.checkBox_2->checkState() == Qt::Checked) {
        ui.tableWidget->setRowCount(cols);
        ui.spinBox->setValue(cols);
    }
    fixSize();
    check();
}

void FilterSelector::check() {
    bool ok;
    for (int j = 0; j < ui.tableWidget->rowCount(); j++)
        for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
            if(ui.tableWidget->item(j, i) == 0) {
                ui.okButton->setEnabled(false);
                ui.saveButton->setEnabled(false);
                return;
            }
            ui.tableWidget->item(j, i)->text().toInt(&ok, 10);
            if(!ok) {
                ui.okButton->setEnabled(false);
                ui.saveButton->setEnabled(false);
                return;
            }
        }
    ui.okButton->setEnabled(true);
    if(ui.checkBox->checkState() == Qt::Checked) ui.saveButton->setEnabled(true);
}

void FilterSelector::fixSize() {
    const int size = 30;
    QTableWidgetItem *item;
    for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
        ui.tableWidget->setRowHeight(i, size);
        item = new QTableWidgetItem(tr("%1").arg(i - ui.tableWidget->rowCount()/2));
        ui.tableWidget->setVerticalHeaderItem(i, item);
    }
    for (int i = 0; i < ui.tableWidget->columnCount(); i++) {
        ui.tableWidget->setColumnWidth(i, size);
        item = new QTableWidgetItem(tr("%1").arg(i - ui.tableWidget->columnCount()/2));
        ui.tableWidget->setHorizontalHeaderItem(i, item);
    }
}
