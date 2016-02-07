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

#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>

class QImage;
class QString;
typedef unsigned char quint8;

QImage* toGray(const QImage* i);
QImage* toMono(const QImage* i);
QImage* negate(const QImage* i);
QImage* normalized(const QImage* i);

QImage* filterBf(const QImage* image, double f[], int size);
QImage* filterColorBf(const QImage* image, double f[], int size);
QImage* filterSym(const QImage* image, double f[], int size);
QImage* filterColorSym(const QImage* image, double f[], int size);

QImage* gaussianBlur(const QImage* image, double sigma, int size);
QImage* gaussianBlur(const QImage* image, double radius);
QImage* gaussianBlur(const QImage* image);
QImage* diffOfGaussian(const QImage* image, double radius1, double radius2, bool negative, bool normalize);

inline double gauss(int x, int y, int size, double sigma) {
    int xy0 = size / 2;
    return exp( -( pow(x - xy0, 2) / sigma + pow(y - xy0, 2) / sigma));
}
void makeGauss(double f[], int size, double sigma);
void makeGaussCurve(double f[], int size, double sigma);

QImage* laplace(const QImage* image, int mask, bool blur, bool negative, bool normalize);
QImage* zeroCrossing(const QImage* image, bool blur, int thold);
void check(int x, int height, int y, int lowThold, quint8 *gradient, bool *potencial, bool *checked, bool *edge);
QImage* canny(const QImage* image, int hiThold, int lowThold, bool blur);

QImage* filterGradientEdges(const QImage* image, int f[], int size, int thold);
QImage* filterColorGradientEdges(const QImage* image, int f[], int size, int thold, bool binary);
QImage* filterCompassEdges(const QImage* image, int f[], int size, int thold);
QImage* filterColorCompassEdges(const QImage* image, int f[], int size, int thold, bool binary);
QImage* edgesPrevitt(const QImage* image, int thold, bool binary, bool gradient);
QImage* edgesSobel(const QImage* image, int thold, bool binary, bool gradient);
QImage* edgesRoberts(const QImage* image, int thold, bool binary);
QImage* filterCompass8Edges(const QImage* image, int f[], int size, int thold, bool binary);
QImage* filterColorCompass8Edges(const QImage* image, int f[], int size, int thold, bool binary);
QImage* edgesRobinson(const QImage* image, int thold, bool binary);
QImage* edgesKirsch(const QImage* image, int thold, bool binary);
QImage* edgesDiff(const QImage* image, int thold, bool binary);

QImage* edges(const QImage* image, QString filter, int thold, bool blur, bool negative, bool binary);

QImage* filter(const QImage* image, double f[], int rows, int cols);

bool isBinary(const QImage* image);
QImage* filterBinary(const QImage* image, double f[], int size, bool keep);
QImage* completeLines(const QImage* image);
QImage* thinnerLines(const QImage* image);

#endif //EFFECTS_H
