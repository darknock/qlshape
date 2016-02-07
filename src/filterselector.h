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

#ifndef FILTERSELECTOR_H
#define FILTERSELECTOR_H

#include "ui_filterselector.h"

#include <QVector>

using namespace Ui;

class FilterSelector : public QDialog
{
    Q_OBJECT
public:
    FilterSelector(QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~FilterSelector() {}
    double* mask();
    QString filter() const {
        if(ui.checkBox->checkState() != Qt::Checked) 
            return ui.comboBox->currentText(); 
        return QString("custom");
    }
    int rows() const { return ui.tableWidget->rowCount(); }
    int cols() const { return ui.tableWidget->columnCount(); }
    void saveData();
private slots:
    void customClicked(bool checked);
    void setRows(int rows);
    void setCols(int cols);
    void selectFilter(int i);
    void selectCustomFilter(int i);
    void check();
    void finishEditing();
    void saveButtonClick();
    void delButtonClick();
private:
    QLineEdit *lineEdit;
    QDialog *editDialog;
    FilterDialog ui;
    QStringList filters;
    QStringList customFilters;
    QVector< QVector<qint32> > customFiltersData;
    QString dataFile;
    void fixSize();
    void setMask(int *f, int rowCount, int columnCount);
};

#endif //FILTERSELECTOR_H
