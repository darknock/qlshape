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

#ifndef PREVIEWLABEL_H
#define PREVIEWLABEL_H

#include <QLabel>

class QMouseEvent;
class QPoint;
class QImage;

class PreviewLabel : public QLabel
{
    Q_OBJECT
public:
    PreviewLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~PreviewLabel();
    void setPreview(QImage *prev);
    void refresh() { setPixmap(QPixmap::fromImage(prevImage)); }
    const QImage *preview() const { return &prevImage; }
    QRect borders() { return QRect(x, y, image->width(), image->height()); }
signals:
    void dragingFinished();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    bool outOfBounds(int x, int y);
    bool xOutOfBounds(int x);
    bool yOutOfBounds(int y);
    int oldX, oldY, newX, newY;
    int x, y, w, h;
    QImage *image;
    QImage prevImage;
};

#endif //PREVIEWLABEL_H
