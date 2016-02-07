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

#include "previewlabel.h"
#include <QMouseEvent>

PreviewLabel::PreviewLabel(QWidget * parent, Qt::WFlags f) : QLabel(parent, f) {}

PreviewLabel::~PreviewLabel() {}

void PreviewLabel::mousePressEvent(QMouseEvent *event) {
    oldX = x + event->x();
    oldY = y + event->y();
    event->accept();
    setCursor(Qt::ClosedHandCursor);
}

void PreviewLabel::mouseMoveEvent(QMouseEvent *event) {
    newX = oldX - event->x();
    newY = oldY - event->y();
    if ( !outOfBounds( newX, newY ) ) {
        if ( !xOutOfBounds( newX ) ) x = newX;
        if ( !yOutOfBounds( newY ) ) y = newY;
        prevImage = image->copy(x, y, w, h);
        setPixmap(QPixmap::fromImage(prevImage));
        event->accept();
    } else event->ignore();
}

void PreviewLabel::mouseReleaseEvent(QMouseEvent *event) {
    if (oldX != event->x() || oldX != event->y())
        emit dragingFinished();
    setCursor(Qt::OpenHandCursor);
    event->accept();
}

void PreviewLabel::setPreview(QImage* prev) {
    image = prev;
    w = width() - 10 < prev->width() ? width() - 10 : prev->width();
    h = height() - 10 < prev->height() ? height() - 10 : prev->height();;
    x = image->width()/2 - w/2;
    x = x < 0 ? 0 : x;
    y = image->height()/2 - h/2;
    y = y < 0 ? 0 : y;
    prevImage = image->copy(x, y, w, h);
}

bool PreviewLabel::outOfBounds(int x, int y) {
    if ( x < 0 && y < 0 ) return true;
    if ( x + w > image->width() && y + h > image->height() ) return true;
    return false;
}

bool PreviewLabel::xOutOfBounds(int x) {
    if ( x < 0 ) return true;
    if ( x + w > image->width() ) return true;
    return false;
}

bool PreviewLabel::yOutOfBounds(int y) {
    if ( y < 0 ) return true;
    if ( y + h > image->height() ) return true;
    return false;
}
