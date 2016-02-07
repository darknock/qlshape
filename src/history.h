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

#ifndef HISTORY_H
#define HISTORY_H

#include <QList>
#include <QImage>

class QLShape;

class History : private QList<QImage>
{
    public:
        History(QLShape *d);
        ~History() { clear(); }
        void reset(QImage i);
        void undo();
        void redo();
        bool hasNext();
        bool hasPrevious();
        QString peekNextDscr();
        QString peekPreviousDscr();
        void add(QImage i, QString ds);
    private:
        QList<QString> dscr;
        QList<QString>::iterator iterDscr;
        QList<QImage>::iterator iter;
        QLShape *parent;
        const int max;
};

#endif //HISTORY_H
