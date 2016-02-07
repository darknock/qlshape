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

#include "qlshape.h"
#include "history.h"

#include <QAction>

History::History(QLShape *d) : max(20) {
    parent = d;
    iter = end();
}

void History::reset(QImage i) {
    clear();
    append(i);
    dscr.append("");
    iter = end();
    iterDscr = dscr.end();
}

bool History::hasNext() {
    return (iter != end() - 1);
}

bool History::hasPrevious() {
    return (iter != begin());
}

void History::undo() {
    if(hasPrevious()) {
        parent->setCurrentImage(new QImage(*--iter));
        --iterDscr;
    }
}

void History::redo() {
    if(hasNext()) {
        parent->setCurrentImage(new QImage(*++iter));
        ++iterDscr;
    }
}

void History::add(QImage i, QString ds) {
    iter = insert(iter + 1, i);
    erase(iter + 1 , end());
    if(size() >= max) removeFirst();

    iterDscr = dscr.insert(iterDscr + 1, ds);
    dscr.erase(iterDscr + 1 , dscr.end());
    if(size() >= max) dscr.removeFirst();
}

QString History::peekNextDscr() {
    if(hasNext()) return *(iterDscr + 1);
    return QString("");
}

QString History::peekPreviousDscr() {
    if(hasPrevious()) return *(iterDscr);
    return QString("");
}
