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

#include "effects.h"
#include <QImage>
#include <QColor>

QImage* toGray(const QImage* i) {
    QImage* newImage = new QImage(i->width(), i->height(), i->format());
    int gray, a;
    for (int x = 0; x < i->width(); x++)
        for (int y = 0; y < i->height(); y++) {
            gray = qGray(i->pixel(x, y));
            a = qAlpha(i->pixel(x, y));
            newImage->setPixel(x, y, qRgba(gray, gray, gray, a));
        }
    return newImage;
}

QImage* toMono(const QImage* i) {
    return new QImage(i->convertToFormat(QImage::Format_Mono).convertToFormat(i->format()));
}

QImage* negate(const QImage* i) {
    QImage* newImage = new QImage(*i);
    newImage->invertPixels();
    return newImage;
}

QImage* normalized(const QImage* i) {
    QImage* newImage = new QImage(i->width(), i->height(), i->format());
    const int yMin = 0, yMax = 255;
    int xMinR = 255, xMaxR = 0;
    int xMinG = 255, xMaxG = 0;
    int xMinB = 255, xMaxB = 0;
    double r, g, b;
    for (int x = 0; x < i->width(); x++)
        for (int y = 0; y < i->height(); y++) {
            if (xMinR > qRed(i->pixel(x, y))) xMinR = qRed(i->pixel(x, y));
            if (xMaxR < qRed(i->pixel(x, y))) xMaxR = qRed(i->pixel(x, y));
            if (xMinG > qGreen(i->pixel(x, y))) xMinG = qGreen(i->pixel(x, y));
            if (xMaxG < qGreen(i->pixel(x, y))) xMaxG = qGreen(i->pixel(x, y));
            if (xMinB > qBlue(i->pixel(x, y))) xMinB = qBlue(i->pixel(x, y));
            if (xMaxB < qBlue(i->pixel(x, y))) xMaxB = qBlue(i->pixel(x, y));
        }
    for (int x = 0; x < i->width(); x++)
        for (int y = 0; y < i->height(); y++) {
            r = yMin + (qRed(i->pixel(x, y)) - xMinR) * (yMax * 1.0 / (xMaxR - xMinR));
            g = yMin + (qGreen(i->pixel(x, y)) - xMinG) * (yMax * 1.0 / (xMaxG - xMinG));
            b = yMin + (qBlue(i->pixel(x, y)) - xMinB) * (yMax *1.0 / (xMaxB - xMinB));
            newImage->setPixel(x, y, qRgba(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), qAlpha(i->pixel(x, y))));
        }
    return newImage;
}

QImage* filterBf(const QImage* image, double f[], int size) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int n = size * size, t = 0;
    double div = 0, gray;
    for (int i = 0; i < size * size; i++) div += f[i];
    if (div != 0) for (int i = 0; i < size * size; i++) f[i] /= div;
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            gray = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < n ? t : t = 0;
                    gray += qGray(image->pixel(x + i, y + j)) * f[t];
                    t++;
                }
            gray = gray < 0 ? 0 : gray > 255 ? 255 : gray;
            newImage->setPixel(x, y, qRgb(static_cast<int>(gray), static_cast<int>(gray), static_cast<int>(gray)));
        }
    return newImage;
}

QImage* filterColorBf(const QImage* image, double f[], int size) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int n = size * size, t = 0;
    double div = 0, r, g, b;
    for (int i = 0; i < size * size; i++) div += f[i];
    if (div != 0) for (int i = 0; i < size * size; i++) f[i] /= div;
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            r = g = b = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < n ? t : t = 0;
                    r += qRed(image->pixel(x + i, y + j)) * f[t];
                    g += qGreen(image->pixel(x + i, y + j)) * f[t];
                    b += qBlue(image->pixel(x + i, y + j)) * f[t];
                    t++;
                }
            r = r < 0 ? 0 : r > 255 ? 255 : r;
            g = g < 0 ? 0 : g > 255 ? 255 : g;
            b = b < 0 ? 0 : b > 255 ? 255 : b;
            newImage->setPixel(x, y, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
        }
    return newImage;
}

QImage* filterSym(const QImage* image, double f[], int size) {
    QImage* midImage = new QImage(*image);
    QImage* newImage;

    double div = 0, gray;
    for (int i = 0; i < size; i++) div += f[i];
    if (div != 0) for (int i = 0; i < size; i++) f[i] /= div;

    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = 0; y < image->height(); y++) {
            gray = 0;
            for (int i = 0; i < size; i++)
                gray += qGray(image->pixel(x - ( (size-1) >> 1) + i, y)) * f[i];
            if (div == 0) gray = gray < 0 ? 0 : gray > 255 ? 255 : gray;
            midImage->setPixel(x, y, qRgb(static_cast<int>(gray), static_cast<int>(gray), static_cast<int>(gray)));
        }
    newImage = new QImage(*midImage);
    for (int x = 0; x < image->width(); x++)
        for (int y =  size/2; y < image->height() - size/2; y++) {
            gray = 0;
            for (int i = 0; i < size; i++)
                gray += qGray(midImage->pixel(x, y - ( (size-1) >> 1) + i)) * f[i];
            if (div == 0) gray = gray < 0 ? 0 : gray > 255 ? 255 : gray;
            newImage->setPixel(x, y, qRgb(static_cast<int>(gray), static_cast<int>(gray), static_cast<int>(gray)));
        }
    delete midImage;
    return newImage;
}

QImage* filterColorSym(const QImage* image, double f[], int size) {
    QImage* midImage = new QImage(*image);
    QImage* newImage;

    double div = 0, r, g, b;
    int ind;
    for (int i = 0; i < size; i++) div += f[i];
    if (div != 0) for (int i = 0; i < size; i++) f[i] /= div;

    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = 0; y < image->height(); y++) {
            r = g = b = 0;
            for (int i = 0; i < size; i++) {
                ind = x - ( (size-1) >> 1) + i;
                r += qRed(image->pixel(ind, y)) * f[i];
                g += qGreen(image->pixel(ind, y)) * f[i];
                b += qBlue(image->pixel(ind, y)) * f[i];
            }
            if (div == 0) {
                r = r < 0 ? 0 : r > 255 ? 255 : r;
                g = g < 0 ? 0 : g > 255 ? 255 : g;
                b = b < 0 ? 0 : b > 255 ? 255 : b;
            }
            midImage->setPixel(x, y, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
        }
    newImage = new QImage(*midImage);
    for (int x = 0; x < image->width(); x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            r = g = b = 0;
            for (int i = 0; i < size; i++) {
                ind = y - ( (size-1) >> 1) + i;
                r += qRed(midImage->pixel(x, ind)) * f[i];
                g += qGreen(midImage->pixel(x, ind)) * f[i];
                b += qBlue(midImage->pixel(x, ind)) * f[i];
            }
            if (div == 0) {
                r = r < 0 ? 0 : r > 255 ? 255 : r;
                g = g < 0 ? 0 : g > 255 ? 255 : g;
                b = b < 0 ? 0 : b > 255 ? 255 : b;
            }
            newImage->setPixel(x, y, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
        }
    delete midImage;
    return newImage;
}

QImage* gaussianBlur(const QImage* image) {
    QImage* newImage;
    const int size = 3;
    double gaussCurve[] = {
                              1, 2, 1
                          };
    if (image->isGrayscale()) newImage = filterSym(image, gaussCurve, size);
    else newImage = filterColorSym(image, gaussCurve, size);
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* gaussianBlur(const QImage* image, double sigma, int size) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    double *gaussCurve = new double[size];
    makeGaussCurve(gaussCurve, size, sigma);
    if (image->isGrayscale()) newImage = filterSym(image, gaussCurve, size);
    else newImage = filterColorSym(image, gaussCurve, size);
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

//Ta funkcja wykorzystuje część kodu wtyczki DOG dla GIMP
//Copyright (C) 1995 Spencer Kimball and Peter Mattis
QImage* gaussianBlur(const QImage* image, double radius) {
    radius += 1.0;
    double std_dev = sqrt (-(radius * radius) / (2 * log (1.0 / 255.0)));
    double sigma = 2 * std_dev * std_dev;
    double l = sqrt (-sigma * log (1.0 / 255.0));
    int n = static_cast<int>(ceil(l) * 2);
    if ((n % 2) == 0) n ++;
    return gaussianBlur(image, sigma, n);
}

//Ta funkcja wykorzystuje część kodu wtyczki DOG dla GIMP
//Copyright (C) 1995 Spencer Kimball and Peter Mattis
QImage* diffOfGaussian(const QImage* image, double radius1, double radius2, bool negative, bool normalize) {
    radius1 += 1.0;
    double std_dev = sqrt (-(radius1 * radius1) / (2 * log (1.0 / 255.0)));
    double sigma1 = 2 * std_dev * std_dev;
    double l = sqrt (-sigma1 * log (1.0 / 255.0));
    int n1 = static_cast<int>(ceil(l) * 2);
    if ((n1 % 2) == 0) n1++;

    radius2 += 1.0;
    std_dev = sqrt (-(radius2 * radius2) / (2 * log (1.0 / 255.0)));
    double sigma2 = 2 * std_dev * std_dev;
    l = sqrt (-sigma2 * log (1.0 / 255.0));
    int n2 = static_cast<int>(ceil(l) * 2);
    if ((n2 % 2) == 0) n2++;

    const int thold = 0;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int size = n1 > n2 ? n1 : n2;
    QImage *g1 = gaussianBlur(image, sigma1, n1);
    QImage *g2 = gaussianBlur(image, sigma2, n2);

    if (!image->isGrayscale()) {
        double r, g, b, a;
        for (int x = size/2; x < image->width() - size/2; x++)
            for (int y = size/2; y < image->height() - size/2; y++) {
                r = qRed(g1->pixel(x,y)) - qRed(g2->pixel(x,y));
                r = r < thold ? 0 : r > 255 ? 255 : r;
                g = qGreen(g1->pixel(x,y)) - qGreen(g2->pixel(x,y));
                g = g < thold ? 0 : g > 255 ? 255 : g;
                b = qBlue(g1->pixel(x,y)) - qBlue(g2->pixel(x,y));
                b = b < thold ? 0 : b > 255 ? 255 : b;
                a = qAlpha(image->pixel(x,y));
                newImage->setPixel(x, y, qRgba(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a)));
            }
        for (int i = 0; i < image->width(); ++i)
            for (int j = 0; j < image->height(); ++j) {
                if (i < size/2 || i > image->width() - size/2 - 1)
                    qAlpha(image->pixel(i, j)) != 255 ? newImage->setPixel(i, j, qAlpha(image->pixel(i, j))) : newImage->setPixel(i, j, 0);
                if (j < size/2 || j > image->height() - size/2 - 1)
                    qAlpha(image->pixel(i, j)) != 255 ? newImage->setPixel(i, j, qAlpha(image->pixel(i, j))) : newImage->setPixel(i, j, 0);
            }
        delete g1;
        delete g2;
        if (normalize) {
            if (negative) newImage->invertPixels();
            QImage *normal = normalized(newImage);
            delete newImage;
            return normal;
        }
        if (negative) newImage->invertPixels();
        return newImage;
    } else { //if is gray
        double gray, a;
        for (int x = size/2; x < image->width() - size/2; x++)
            for (int y = size/2; y < image->height() - size/2; y++) {
                gray = qRed(g1->pixel(x,y)) - qRed(g2->pixel(x,y));
                gray = gray < thold ? 0 : gray > 255 ? 255 : gray;
                a = qAlpha(image->pixel(x,y));
                newImage->setPixel(x, y, qRgba(static_cast<int>(gray), static_cast<int>(gray), static_cast<int>(gray), static_cast<int>(a)));
            }
        for (int i = 0; i < image->width(); ++i)
            for (int j = 0; j < image->height(); ++j) {
                if (i < size/2 || i > image->width() - size/2 - 1)
                    qAlpha(image->pixel(i, j)) != 255 ? newImage->setPixel(i, j, qAlpha(image->pixel(i, j))) : newImage->setPixel(i, j, 0);
                if (j < size/2 || j > image->height() - size/2 - 1)
                    qAlpha(image->pixel(i, j)) != 255 ? newImage->setPixel(i, j, qAlpha(image->pixel(i, j))) : newImage->setPixel(i, j, 0);
            }
        delete g1;
        delete g2;
        if (normalize) {
            if (negative) newImage->invertPixels();
            QImage *normal = normalized(newImage);
            delete newImage;
            return normal;
        }
        if (negative) newImage->invertPixels();
        return newImage;
    }
}

void makeGauss(double f[], int size, double sigma) {
    for (int x = 0; x < size; x++)
        for (int y = 0; y < size; y++) {
            f[x * size + y] = gauss(x, y, size, sigma);
        }
}

void makeGaussCurve(double f[], int size, double sigma) {
    for (int x = 0; x < size; x++) {
        f[x] = gauss(x, size/2 + 1, size, sigma);
    }
}

QImage* laplace(const QImage* image, int mask, bool blur, bool negative, bool normalize) {
    const int size = 3;
    double lap4[] = {
                        0, -1,  0,
                        -1,  4, -1,
                        0, -1,  0
                    };
    double lap8[] = {
                        -1, -1, -1,
                        -1,  8, -1,
                        -1, -1, -1
                    };
    QImage *blured;
    QImage *newImage;
    if (blur) blured = gaussianBlur(image, 1.5);
    else blured = new QImage(*image);

    if (mask == 4)
        if (negative) {
            newImage = filterColorBf(blured, lap4, size);
            newImage->invertPixels();
            for (int i = 0; i < image->width(); ++i)
                for (int j = 0; j < image->height(); ++j) {
                    if (i < size/2 || i > image->width() - size/2 - 1)
                        newImage->setPixel(i, j, 0xFFFFFFFF);
                    if (j < size/2 || j > image->height() - size/2 - 1)
                        newImage->setPixel(i, j, 0xFFFFFFFF);
                }
        } else {
            newImage = filterColorBf(blured, lap4, size);
            for (int i = 0; i < image->width(); ++i)
                for (int j = 0; j < image->height(); ++j) {
                    if (i < size/2 || i > image->width() - size/2 - 1)
                        newImage->setPixel(i, j, 0);
                    if (j < size/2 || j > image->height() - size/2 - 1)
                        newImage->setPixel(i, j, 0);
                }
        }
    else
        if (negative) {
            newImage = filterColorBf(blured, lap8, size);
            newImage->invertPixels();
            for (int i = 0; i < image->width(); ++i)
                for (int j = 0; j < image->height(); ++j) {
                    if (i < size/2 || i > image->width() - size/2 - 1)
                        newImage->setPixel(i, j, 0xFFFFFFFF);
                    if (j < size/2 || j > image->height() - size/2 - 1)
                        newImage->setPixel(i, j, 0xFFFFFFFF);
                }
        } else {
            newImage = filterColorBf(blured, lap8, size);
            for (int i = 0; i < image->width(); ++i)
                for (int j = 0; j < image->height(); ++j) {
                    if (i < size/2 || i > image->width() - size/2 - 1)
                        newImage->setPixel(i, j, 0);
                    if (j < size/2 || j > image->height() - size/2 - 1)
                        newImage->setPixel(i, j, 0);
                }
        }
    delete blured;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    if (normalize) {
        QImage *normal = normalized(converted);
        delete converted;
        return normal;
    }
    return converted;
}

QImage* zeroCrossing(const QImage* image, bool blur, int thold) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    QImage* blured;
    if (blur) blured = gaussianBlur(image);
    else blured = new QImage(*image);

    int upper = 255, lower = -upper;
    quint32 tableSize = image->height() * image->width();
    qint8* lapTable = new qint8[tableSize];
    for (quint32 i = 0; i < tableSize; i++) lapTable[i] = 0;
    quint8* edgeTable = new quint8[tableSize];

    const int size = 3;
    int f[] = {
                  0, -1,  0,
                  -1,  4, -1,
                  0, -1,  0
              };

    int n = size * size, t = 0, g;
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            g = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < n ? t : t = 0;
                    g += qGray(blured->pixel(x + i, y + j)) * f[t];
                    t++;
                }
            g = g < lower ? 0 : g >= upper ? 255 : g;
            g = g > thold || g < -thold ? g : 0;
            lapTable[x * image->height() + y] = g;
        }

    bool minus = false, preMinus = false;
    for (quint32 i = 0; i < tableSize; i++) {
        if (lapTable[i] < 0) minus = true;
        else minus = false;
        if (preMinus != minus) edgeTable[i] = 0;
        else edgeTable[i] = 255;
        preMinus = minus;
    }
    for (int y = 0; y < image->height(); y++)
        for (int x = 0; x < image->width(); x++) {
            if (lapTable[x * image->height() + y] < 0) minus = true;
            else minus = false;
            if (preMinus != minus) edgeTable[x * image->height() + y] = 0;
            preMinus = minus;
        }

    for (int x = 0; x < image->width(); x++)
        for (int y = 0; y < image->height(); y++) {
            g = edgeTable[x * image->height() + y];
            newImage->setPixel(x, y, qRgba(g, g, g, qAlpha(image->pixel(x, y))));
        }

    delete lapTable;
    delete edgeTable;
    delete blured;
    return newImage;
}

void check(int x, int height, int y, int lowThold, quint8* gradient, bool *potencial, bool *checked, bool *edge) {
    checked[x * height + y] = true;
    if(gradient[x * height + y] > lowThold) {
        edge[x * height + y] = true;

        if(!checked[(x - 1) * height + y - 1] && potencial[(x - 1) * height + y - 1])
            check(x - 1, height, y - 1, lowThold, gradient, potencial, checked, edge);
        if(!checked[ x      * height + y - 1] && potencial[ x      * height + y - 1])
            check(x    , height, y - 1, lowThold, gradient, potencial, checked, edge);
        if(!checked[(x + 1) * height + y - 1] && potencial[(x + 1) * height + y - 1])
            check(x + 1, height, y - 1, lowThold, gradient, potencial, checked, edge);
        if(!checked[(x - 1) * height + y    ] && potencial[(x - 1) * height + y    ])
            check(x - 1, height, y    , lowThold, gradient, potencial, checked, edge);
//         if(!checked[ x      * height + y    ] && potencial[ x      * height + y    ])
//             check(x    , height, y    , lowThold, gradient, potencial, checked, edge);
        if(!checked[(x + 1) * height + y    ] && potencial[(x + 1) * height + y    ])
            check(x + 1, height, y    , lowThold, gradient, potencial, checked, edge);
        if(!checked[(x - 1) * height + y + 1] && potencial[(x - 1) * height + y + 1])
            check(x - 1, height, y + 1, lowThold, gradient, potencial, checked, edge);
        if(!checked[ x      * height + y + 1] && potencial[ x      * height + y + 1])
            check(x    , height, y + 1, lowThold, gradient, potencial, checked, edge);
        if(!checked[(x + 1) * height + y + 1] && potencial[(x + 1) * height + y + 1])
            check(x + 1, height, y + 1, lowThold, gradient, potencial, checked, edge);
    }
}

QImage* canny(const QImage* image, int hiThold, int lowThold, bool blur) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    QImage* blured;
    if (blur) blured = gaussianBlur(image);
    else blured = new QImage(*image);

    const int size = 3;
    int sumX, sumY, g;

    int gX[] = {
                   1,0,-1,
                   2,0,-2,
                   1,0,-1
               };

    int gY[] = {
                   1, 2, 1,
                   0, 0, 0,
                   -1,-2,-1
               };

    int tableSize = image->height() * image->width();
    quint8* gradient = new quint8[tableSize];
    quint8* direction = new quint8[tableSize];
    bool *potencial = new bool[tableSize];
    bool *checked = new bool[tableSize];
    bool *edge = new bool[tableSize];
    for (int i = 0; i < tableSize; i++) {
        potencial[i] = false;
        checked[i] = false;
        edge[i] = false;
    }
    //qDebug("image->height() = %d,\nimage->width() = %d,\ntableSize = %d", image->height(), image->width(), tableSize);

//     Gradient Magnitude and edge direction
    int n = size * size, t = 0;
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            sumX = sumY = 0;
            for (int j = -size/2; j <= size/2; j++)
                for (int i = -size/2; i <= size/2; i++) {
                    t < n ? t : t = 0;
                    g = qGray(blured->pixel(x + i, y + j));
                    sumX += g * gX[t];
                    sumY += g * gY[t];
                    t++;
                }
            g = static_cast<int>(sqrt(sumX * sumX + sumY * sumY));
            gradient[x * image->height() + y] = g > 255 ? 255 : g;
            if (sumY != 0) {
                direction[x * image->height() + y] = (180 + static_cast<int>(atan2(1.0 * sumX, 1.0 * sumY) * 180.0 / 3.14159265)) % 180;
            } else if (sumX == 0)
                direction[x * image->height() + y] = 0;
            else direction[x * image->height() + y] = 90;
            //qDebug("(%d, %d)\n%d  %d\n", x+1, y+1, gradientTable[x * image->height() + y], directionTable[x * image->height() + y]);
        }

//     Non-maximum supression
    quint8 deg, m;
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            deg = direction[x * image->height() + y];
            m = gradient[x * image->height() + y];
            if(deg >= 158 || deg < 23) { // 0
                if(m > gradient[x * image->height() + y - 1] &&
                   m > gradient[x * image->height() + y + 1])
                    potencial[x * image->height() + y] = true;
            } else
                if(deg >= 23 && deg < 68) { // 45
                    if(m > gradient[(x - 1) * image->height() + y - 1] &&
                       m > gradient[(x + 1) * image->height() + y + 1])
                        potencial[x * image->height() + y] = true;
                } else
                    if(deg >= 68 && deg < 113) { // 90
                        if(m > gradient[(x + 1) * image->height() + y] &&
                           m > gradient[(x - 1) * image->height() + y])
                            potencial[x * image->height() + y] = true;
                    } else
                        if(deg >= 113 && deg < 158) { // 135
                            if(m > gradient[(x + 1) * image->height() + y - 1] &&
                               m > gradient[(x - 1) * image->height() + y + 1])
                                potencial[x * image->height() + y] = true;
                        }
        }

//     Hysteresis
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            if(!checked[x * image->height() + y] && potencial[x * image->height() + y] && gradient[x * image->height() + y] > hiThold) {
                check(x, image->height(), y, lowThold, gradient, potencial, checked, edge);
            }
        }



    for (int x = 0; x < image->width(); x++)
        for (int y = 0; y < image->height(); y++) {
//             g = gradient[x * image->height() + y];
            g = edge[x * image->height() + y] ? 0 : 255;
            newImage->setPixel(x, y, qRgba(g, g, g, qAlpha(image->pixel(x, y))));
        }

    delete gradient;
    delete direction;
    delete potencial;
    delete checked;
    delete edge;
    delete blured;
    return newImage;
}

QImage* filterGradientEdges(const QImage* image, int f[], int size, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int tn = size * size, t = 0, pixel, gray;
    int *S, s, *E, e;
    S = new int[tn];
    E = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i] = f[i];
        E[i] = f[i + tn * 2];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            s = e = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < tn ? t : t = 0;
                    pixel = qGray(image->pixel(x + i, y + j));
                    s += pixel * S[t];
                    e += pixel * E[t];
                    t++;
                }
            gray = static_cast<int>(sqrt(s * s + e * e));
            gray = gray < lower ? 0 : gray >= upper ? 255 : gray;
            newImage->setPixel(x, y, qRgb(gray, gray, gray));
        }
    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []E;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* filterColorGradientEdges(const QImage* image, int f[], int size, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int tn = size * size, t = 0;
    int r, g, b;
    int *S, rS, gS, bS, *E, rE, gE, bE;
    S = new int[tn];
    E = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i]  = f[i];
        E[i]  = f[i + tn * 2];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            rS = gS = bS = rE = gE = bE = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < tn ? t : t = 0;
                    r = qRed(image->pixel(x + i, y + j));
                    rS  += r * S[t];
                    rE  += r * E[t];
                    g = qGreen(image->pixel(x + i, y + j));
                    gS  += g * S[t];
                    gE  += g * E[t];
                    b = qBlue(image->pixel(x + i, y + j));
                    bS  += b * S[t];
                    bE  += b * E[t];
                    t++;
                }
            r = static_cast<int>(sqrt(rS * rS + rE * rE));
            r = r < lower ? 0 : r >= upper ? 255 : r;
            g = static_cast<int>(sqrt(gS * gS + gE * gE));
            g = g < lower ? 0 : g >= upper ? 255 : g;
            b = static_cast<int>(sqrt(bS * bS + bE * bE));
            b = b < lower ? 0 : b >= upper ? 255 : b;
            newImage->setPixel(x, y, qRgb(r, g, b));
        }
    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []E;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}


QImage* filterCompassEdges(const QImage* image, int f[], int size, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int tn = size * size, t = 0, pixel, g;
    int *S, s, *SE, se, *E, e, *NE, ne;
    S = new int[tn];
    SE = new int[tn];
    E = new int[tn];
    NE = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i] = f[i];
        SE[i] = f[i + tn];
        E[i] = f[i + tn * 2];
        NE[i] = f[i + tn * 3];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            s = se = e = ne = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < tn ? t : t = 0;
                    pixel = qGray(image->pixel(x + i, y + j));
                    s += pixel * S[t];
                    se += pixel * SE[t];
                    e += pixel * E[t];
                    ne += pixel * NE[t];
                    t++;
                }
            s  *= s;
            g = s;
            se *= se;
            g = g > se ? g : se;
            e *= e;
            g = g > e  ? g : e;
            ne *= ne;
            g = g > ne ? g : ne;
            g = static_cast<int>(sqrt(g));
            g = g < lower ? 0 : g >= upper ? 255 : g;
            newImage->setPixel(x, y, qRgb(g, g, g));
        }
    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []SE;
    delete []E;
    delete []NE;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* filterColorCompassEdges(const QImage* image, int f[], int size, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int tn = size * size, t = 0;
    int r, g, b;
    int *S, rS, gS, bS, *SE, rSE, gSE, bSE, *E, rE, gE, bE, *NE, rNE, gNE, bNE;
    S = new int[tn];
    SE = new int[tn];
    E = new int[tn];
    NE = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i]  = f[i];
        SE[i] = f[i + tn];
        E[i]  = f[i + tn * 2];
        NE[i] = f[i + tn * 3];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            rS = gS = bS = rSE = gSE = bSE = rE = gE = bE = rNE = gNE = bNE = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < tn ? t : t = 0;
                    r = qRed(image->pixel(x + i, y + j));
                    rS  += r * S[t];
                    rSE += r * SE[t];
                    rE  += r * E[t];
                    rNE += r * NE[t];
                    g = qGreen(image->pixel(x + i, y + j));
                    gS  += g * S[t];
                    gSE += g * SE[t];
                    gE  += g * E[t];
                    gNE += g * NE[t];
                    b = qBlue(image->pixel(x + i, y + j));
                    bS  += b * S[t];
                    bSE += b * SE[t];
                    bE  += b * E[t];
                    bNE += b * NE[t];
                    t++;
                }
            rS  *= rS;
            r = rS;
            rSE *= rSE;
            r = r > rSE ? r : rSE;
            rE  *= rE;
            r = r > rE  ? r : rE;
            rNE *= rNE;
            r = r > rNE ? r : rNE;
            r = static_cast<int>(sqrt(r));
            r = r < lower ? 0 : r >= upper ? 255 : r;
            gS  *= gS;
            g = gS;
            gSE *= gSE;
            g = g > gSE ? g : gSE;
            gE  *= gE;
            g = g > gE  ? g : gE;
            gNE  *= gNE;
            g = g > gNE ? g : gNE;
            g = static_cast<int>(sqrt(g));
            g = g < lower ? 0 : g >= upper ? 255 : g;
            bS  *= bS;
            b = rS;
            bSE *= bSE;
            b = b > bSE ? b : bSE;
            bE  *= bE;
            b = b > bE  ? b : bE;
            bNE *= bNE;
            b = b > bNE ? b : bNE;
            b = static_cast<int>(sqrt(b));
            b = b < lower ? 0 : b >= upper ? 255 : b;
            newImage->setPixel(x, y, qRgb(r, g, b));
        }
    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []SE;
    delete []E;
    delete []NE;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* edgesPrevitt(const QImage* image, int thold, bool binary, bool gradient) {
    int f[] = {
                  1, 1, 1,
                  0, 0, 0,
                  -1,-1,-1, //S

                  1, 1, 0,
                  1, 0,-1,
                  0,-1,-1, //SE

                  1, 0,-1,
                  1, 0,-1,
                  1, 0,-1, //E

                  0,-1,-1,
                  1, 0,-1,
                  1, 1, 0 //NE
              };
    if (image->isGrayscale()) {
        if (gradient) return filterGradientEdges(image, f, 3, thold, binary);
        return filterCompassEdges(image, f, 3, thold, binary);
    } else {
        if (gradient) return filterColorGradientEdges(image, f, 3, thold, binary);
        return filterColorCompassEdges(image, f, 3, thold, binary);
    }
}

QImage* edgesSobel(const QImage* image, int thold, bool binary, bool gradient) {
    int f[] = {
                  1, 2, 1,
                  0, 0, 0,
                  -1,-2,-1, //S

                  2, 1, 0,
                  1, 0,-1,
                  0,-1,-2, //SE

                  1, 0,-1,
                  2, 0,-2,
                  1, 0,-1, //E

                  0,-1,-2,
                  1, 0,-1,
                  2, 1, 0 //NE
              };
    if (image->isGrayscale()) {
        if (gradient) return filterGradientEdges(image, f, 3, thold, binary);
        return filterCompassEdges(image, f, 3, thold, binary);
    } else {
        if (gradient) return filterColorGradientEdges(image, f, 3, thold, binary);
        return filterColorCompassEdges(image, f, 3, thold, binary);
    }
}

QImage* edgesRoberts(const QImage* image, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int r, g, b;
    int rGx, rGy, gGx, gGy, bGx, bGy;
    for (int x = 0; x < image->width() - 2; x++)
        for (int y = 0; y < image->height() - 2; y++) {
            r = g = b = 0;
            rGx = qRed(image->pixel(x, y)) - qRed(image->pixel(x + 1, y + 1));
            rGy = qRed(image->pixel(x + 1, y)) - qRed(image->pixel(x, y + 1));
            r = static_cast<int>(sqrt(rGx * rGx + rGy * rGy)) * 4;
            r = r < lower ? 0 : r >= upper ? 255 : r;
            gGx = qGreen(image->pixel(x, y)) - qGreen(image->pixel(x + 1, y + 1));
            gGy = qGreen(image->pixel(x + 1, y)) - qGreen(image->pixel(x, y + 1));
            g = static_cast<int>(sqrt(gGx * gGx + gGy * gGy)) * 4;
            g = g < lower ? 0 : g >= upper ? 255 : g;
            bGx = qBlue(image->pixel(x, y)) - qBlue(image->pixel(x + 1, y + 1));
            bGy = qBlue(image->pixel(x + 1, y)) - qBlue(image->pixel(x, y + 1));
            b = static_cast<int>(sqrt(bGx * bGx + bGy * bGy)) * 4;
            b = b < lower ? 0 : b >= upper ? 255 : b;
            newImage->setPixel(x, y, qRgb(r, g, b));
        }
    for (int i = 0; i < image->width(); ++i)
        newImage->setPixel(i, image->height() - 1, 0);
    for (int j = 0; j < image->height(); ++j)
        newImage->setPixel(image->width() - 1, j, 0);
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* filterCompass8Edges(const QImage* image, int f[], int size, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int tn = size * size, t = 0, pixel, g;
    int *S, s, *SE, se, *E, e, *NE, ne, *N, n, *NW, nw, *W, w, *SW, sw;
    S = new int[tn];
    SE = new int[tn];
    E = new int[tn];
    NE = new int[tn];
    N = new int[tn];
    NW = new int[tn];
    W = new int[tn];
    SW = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i] = f[i];
        SE[i] = f[i + tn];
        E[i]  = f[i + tn * 2];
        NE[i] = f[i + tn * 3];
        N[i]  = f[i + tn * 4];
        NW[i] = f[i + tn * 5];
        W[i]  = f[i + tn * 6];
        SW[i] = f[i + tn * 7];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            s = se = e = ne = n = nw = w = sw = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < tn ? t : t = 0;
                    pixel = qGray(image->pixel(x + i, y + j));
                    s += pixel * S[t];
                    se += pixel * SE[t];
                    e += pixel * E[t];
                    ne += pixel * NE[t];
                    n += pixel * N[t];
                    nw += pixel * NW[t];
                    w += pixel * W[t];
                    sw += pixel * SW[t];
                    t++;
                }
            s  *= s;
            g = s;
            se *= se;
            g = g > se ? g : se;
            e *= e;
            g = g > e  ? g : e;
            ne *= ne;
            g = g > ne ? g : ne;
            n *=n;
            g = g > n  ? g : n;
            nw *= nw;
            g = g > nw ? g : nw;
            w *= w;
            g = g > w  ? g : w;
            sw *= sw;
            g = g > sw ? g : sw;
            g = static_cast<int>(sqrt(g));
            g = g < lower ? 0 : g >= upper ? 255 : g;
            newImage->setPixel(x, y, qRgb(g, g, g));
        }
    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []SE;
    delete []E;
    delete []NE;
    delete []N;
    delete []NW;
    delete []W;
    delete []SW;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* filterColorCompass8Edges(const QImage* image, int f[], int size, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int tn = size * size, t = 0;
    int r, g, b;
    int *S, rS, gS, bS, *SE, rSE, gSE, bSE, *E, rE, gE, bE, *NE, rNE, gNE, bNE;
    int *N, rN, gN, bN, *NW, rNW, gNW, bNW, *W, rW, gW, bW, *SW, rSW, gSW, bSW;
    S = new int[tn];
    SE = new int[tn];
    E = new int[tn];
    NE = new int[tn];
    N = new int[tn];
    NW = new int[tn];
    W = new int[tn];
    SW = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i] = f[i];
        SE[i] = f[i + tn];
        E[i]  = f[i + tn * 2];
        NE[i] = f[i + tn * 3];
        N[i]  = f[i + tn * 4];
        NW[i] = f[i + tn * 5];
        W[i]  = f[i + tn * 6];
        SW[i] = f[i + tn * 7];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            rS = gS = bS = rSE = gSE = bSE = rE = gE = bE = rNE = gNE = bNE = 0;
            rN = gN = bN = rNW = gNW = bNW = rW = gW = bW = rSW = gSW = bSW = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                    t < tn ? t : t = 0;
                    r = qRed(image->pixel(x + i, y + j));
                    rS  += r * S[t];
                    rSE += r * SE[t];
                    rE  += r * E[t];
                    rNE += r * NE[t];
                    rN  += r * N[t];
                    rNW += r * NW[t];
                    rW  += r * W[t];
                    rSW += r * SW[t];
                    g = qGreen(image->pixel(x + i, y + j));
                    gS  += g * S[t];
                    gSE += g * SE[t];
                    gE  += g * E[t];
                    gNE += g * NE[t];
                    gN  += g * N[t];
                    gNW += g * NW[t];
                    gW  += g * W[t];
                    gSW += g * SW[t];
                    b = qBlue(image->pixel(x + i, y + j));
                    bS  += b * S[t];
                    bSE += b * SE[t];
                    bE  += b * E[t];
                    bNE += b * NE[t];
                    bN  += b * N[t];
                    bNW += b * NW[t];
                    bW  += b * W[t];
                    bSW += b * SW[t];
                    t++;
                }
            rS  *= rS;
            r = rS;
            rSE *= rSE;
            r = r > rSE ? r : rSE;
            rE  *= rE;
            r = r > rE  ? r : rE;
            rNE *= rNE;
            r = r > rNE ? r : rNE;
            rN *= rN;
            r = r > rN  ? r : rN;
            rNW *= rNW;
            r = r > rNW  ? r : rNW;
            rW *= rW;
            r = r > rW  ? r : rW;
            rSW *= rSW;
            r = r > rSW  ? r : rSW;
            r = static_cast<int>(sqrt(r));
            r = r < lower ? 0 : r >= upper ? 255 : r;
            gS  *= gS;
            g = gS;
            gSE *= gSE;
            g = g > gSE ? g : gSE;
            gE  *= gE;
            g = g > gE  ? g : gE;
            gNE  *= gNE;
            g = g > gNE ? g : gNE;
            gN *= gN;
            g = g > gN  ? g : gN;
            gNW *= gNW;
            g = g > gNW  ? g : gNW;
            gW *= gW;
            g = g > gW  ? g : gW;
            gSW *= gSW;
            g = g > gSW  ? g : gSW;
            g = static_cast<int>(sqrt(g));
            g = g < lower ? 0 : g >= upper ? 255 : g;
            bS  *= bS;
            b = rS;
            bSE *= bSE;
            b = b > bSE ? b : bSE;
            bE  *= bE;
            b = b > bE  ? b : bE;
            bNE *= bNE;
            b = b > bNE ? b : bNE;
            bN *= bN;
            b = b > bN  ? b : bN;
            bNW *= bNW;
            b = b > bNW  ? b : bNW;
            bW *= bW;
            b = b > bW  ? b : bW;
            bSW *= bSW;
            b = b > bSW  ? b : bSW;
            b = static_cast<int>(sqrt(b));
            b = b < lower ? 0 : b >= upper ? 255 : b;
            newImage->setPixel(x, y, qRgb(r, g, b));
        }
    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []SE;
    delete []E;
    delete []NE;
    delete []N;
    delete []NW;
    delete []W;
    delete []SW;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* edgesRobinson(const QImage* image, int thold, bool binary) {
    int f[] = {
                  1, 1, 1,
                  1,-2, 1,
                  -1,-1,-1, //S

                  1, 1, 1,
                  1,-2,-1,
                  1,-1,-1, //SE

                  1, 1,-1,
                  1,-2,-1,
                  1, 1,-1, //E

                  1,-1,-1,
                  1,-2,-1,
                  1, 1, 1, //NE

                  -1,-1,-1,
                  1, -2, 1,
                  1, 1, 1, //N

                  -1,-1, 1,
                  -1,-2, 1,
                  1, 1, 1, //NW

                  -1, 1, 1,
                  -1,-2, 1,
                  -1, 1, 1, //W

                  1, 1, 1,
                  -1,-2, 1,
                  -1,-1, 1 //SW
              };
    if (image->isGrayscale()) return filterCompass8Edges(image, f, 3, thold, binary);
    else return filterColorCompass8Edges(image, f, 3, thold, binary);
}

QImage* edgesKirsch(const QImage* image, int thold, bool binary) {
    int f[] = {
                  3, 3, 3,
                  3, 0, 3,
                  -5,-5,-5, //S

                  3, 3, 3,
                  3, 0,-5,
                  3,-5,-5, //SE

                  3, 3,-5,
                  3, 0,-5,
                  3, 3,-5, //E

                  3,-5,-5,
                  3, 0,-5,
                  3, 3, 3, //NE

                  -5,-5,-5,
                  3,  0, 3,
                  3, 3, 3, //N

                  -5,-5, 3,
                  -5, 0, 3,
                  3, 3, 3, //NW

                  -5, 3, 3,
                  -5, 0, 3,
                  -5, 3, 3, //W

                  3, 3, 3,
                  -5, 0, 3,
                  -5,-5, 3 //SW
              };
    if (image->isGrayscale()) return filterCompass8Edges(image, f, 3, thold, binary);
    else return filterColorCompass8Edges(image, f, 3, thold, binary);
}

QImage* edgesDiff(const QImage* image, int thold, bool binary) {
    int lower, upper;
    lower = thold;
    if (binary) upper = lower;
    else upper = 255;
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int r, g, b;
    int Gx, Gy, Gv, Gz;
    for (int x = 0; x < image->width() - 2; x++)
        for (int y = 1; y < image->height() - 2; y++) {
            Gx = qRed(image->pixel(x, y)) - qRed(image->pixel(x,     y - 1));
            Gv = qRed(image->pixel(x, y)) - qRed(image->pixel(x + 1, y - 1));
            Gy = qRed(image->pixel(x, y)) - qRed(image->pixel(x + 1, y));
            Gz = qRed(image->pixel(x, y)) - qRed(image->pixel(x + 1, y + 1));
            Gx *= Gx;
            r = Gx;
            Gv *= Gv;
            r = r > Gv ? r : Gv;
            Gy *= Gy;
            r = r > Gy ? r : Gy;
            Gz *= Gz;
            r = r > Gz ? r : Gz;
            r = static_cast<int>(sqrt(r));
            r = r < lower ? 0 : r >= upper ? 255 : r;

            Gx = qGreen(image->pixel(x, y)) - qGreen(image->pixel(x,     y - 1));
            Gv = qGreen(image->pixel(x, y)) - qGreen(image->pixel(x + 1, y - 1));
            Gy = qGreen(image->pixel(x, y)) - qGreen(image->pixel(x + 1, y));
            Gz = qGreen(image->pixel(x, y)) - qGreen(image->pixel(x + 1, y + 1));
            Gx *= Gx;
            g = Gx;
            Gv *= Gv;
            g = g > Gv ? g : Gv;
            Gy *= Gy;
            g = g > Gy ? g : Gy;
            Gz *= Gz;
            g = g > Gz ? g : Gz;
            g = static_cast<int>(sqrt(g));
            g = g < lower ? 0 : g >= upper ? 255 : g;

            Gx = qBlue(image->pixel(x, y)) - qBlue(image->pixel(x,     y - 1));
            Gv = qBlue(image->pixel(x, y)) - qBlue(image->pixel(x + 1, y - 1));
            Gy = qBlue(image->pixel(x, y)) - qBlue(image->pixel(x + 1, y));
            Gz = qBlue(image->pixel(x, y)) - qBlue(image->pixel(x + 1, y + 1));
            Gx *= Gx;
            b = Gx;
            Gv *= Gv;
            b = b > Gv ? b : Gv;
            Gy *= Gy;
            b = b > Gy ? b : Gy;
            Gz *= Gz;
            b = b > Gz ? b : Gz;
            b = static_cast<int>(sqrt(b));
            b = b < lower ? 0 : b >= upper ? 255 : b;

            newImage->setPixel(x, y, qRgb(r, g, b));
        }
    for (int i = 0; i < image->width(); ++i) {
        newImage->setPixel(i, image->height() - 1, 0);
        newImage->setPixel(i, 0, 0);
    }
    for (int j = 0; j < image->height(); ++j)
        newImage->setPixel(image->width() - 1, j, 0);
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* edges(const QImage* image, QString filter, int thold, bool blur, bool negative, bool binary) {
    QImage* blured;
    QImage* newImage;

    if (blur) blured = gaussianBlur(image, 1.5);
    else blured = new QImage(*image);

    if (filter == "Sobel (compass)") newImage = edgesSobel(blured, thold, binary, false);
    else if (filter == "Previtt (compass)") newImage = edgesPrevitt(blured, thold, binary, false);
    else if (filter == "Robinson") newImage =  edgesRobinson(blured, thold, binary);
    else if (filter == "Kirsch") newImage =  edgesKirsch(blured, thold, binary);
    else if (filter == "Differential") newImage = edgesDiff(blured, thold, binary);
    else if (filter == "Sobel (gradient)") newImage = edgesSobel(blured, thold, binary, true);
    else if (filter == "Previtt (gradient)") newImage = edgesPrevitt(blured, thold, binary, true);
    else if (filter == "Roberts") newImage = edgesRoberts(blured, thold, binary);
    else newImage = new QImage(*image);

    delete blured;
//     if (negative) {
//         QImage *negated = negate(newImage);
//         delete newImage;
//         return negated;
//     }
    if (negative) newImage->invertPixels();
    return newImage;
}

QImage* filter(const QImage* image, double f[], int rows, int cols) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int n = rows * cols, t = 0;
    double div = 0, r, g, b;
    for (int i = 0; i < n; i++) div += f[i];
    if (div != 0) for (int i = 0; i < n; i++) f[i] /= div;
    for (int x = cols/2; x < image->width() - cols/2; x++)
        for (int y = rows/2; y < image->height() - rows/2; y++) {
            r = g = b = 0;
            for (int j = -rows/2; j<=rows/2; j++)
                for (int i = -cols/2; i <= cols/2; i++) {
                    t < n ? t : t = 0;
                    r += qRed(image->pixel(x + i, y + j)) * f[t];
                    g += qGreen(image->pixel(x + i, y + j)) * f[t];
                    b += qBlue(image->pixel(x + i, y + j)) * f[t];
                    t++;
                }
            r = r < 0 ? 0 : r > 255 ? 255 : r;
            g = g < 0 ? 0 : g > 255 ? 255 : g;
            b = b < 0 ? 0 : b > 255 ? 255 : b;
            newImage->setPixel(x, y, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
        }
    delete []f;
    return newImage;
}

bool isBinary(const QImage* image) {
    int r;
    if (image->isGrayscale())
        for (int x = 0; x < image->width(); x++)
            for (int y = 0; y < image->height(); y++) {
                r = qRed(image->pixel(x, y));
                if (r != 0 && r != 255)
                    return false;
            }
    else return false;
    return true;
}

QImage* filterBinary(const QImage* image, int f[], int size, bool keep) {
    QImage* newImage = new QImage(*image);
    if (!keep)
        for (int x = 0; x < image->width(); x++)
            for (int y = 0; y < image->height(); y++)
                newImage->setPixel(x, y, qRgb(255, 255, 255));
    int tn = size * size, t = 0, r;
    bool s, se, e, ne, n, nw, w, sw;
    int *S, *SE, *E, *NE, *N, *NW, *W, *SW;
    S = new int[tn];
    SE = new int[tn];
    E = new int[tn];
    NE = new int[tn];
    N = new int[tn];
    NW = new int[tn];
    W = new int[tn];
    SW = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i] = f[i];
        SE[i] = f[i + tn];
        E[i]  = f[i + tn * 2];
        NE[i] = f[i + tn * 3];
        N[i]  = f[i + tn * 4];
        NW[i] = f[i + tn * 5];
        W[i]  = f[i + tn * 6];
        SW[i] = f[i + tn * 7];
    }
    for (int x = size/2; x < image->width() - size/2; x++)
        for (int y = size/2; y < image->height() - size/2; y++) {
            s = se = e = ne = n = nw = w = sw = true;
            t = 0;
            for (int j = -size/2; j <=size/2; j++) {
                for (int i = -size/2; i <= size/2; i++) {
                    t < tn ? t : t = 0;
                    r = qRed(image->pixel(x + i, y + j));
                    if (S[t] != 128) if (S[t] != r) s = false;
                    if (SE[t] != 128) if (SE[t] != r) se = false;
                    if (E[t] != 128) if (E[t] != r) e = false;
                    if (NE[t] != 128) if (NE[t] != r) ne = false;
                    if (N[t] != 128) if (N[t] != r) n = false;
                    if (NW[t] != 128) if (NW[t] != r) nw = false;
                    if (W[t] != 128) if (W[t] != r) w = false;
                    if (SW[t] != 128) if (SW[t] != r) sw = false;
                    t++;
                }
                if (!(s || se || e || ne || n || nw || w || sw)) break;
            }
            if (s || se || e || ne || n || nw || w || sw) newImage->setPixel(x, y, qRgb(0, 0, 0));
        }

    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
        }
    delete []S;
    delete []SE;
    delete []E;
    delete []NE;
    delete []N;
    delete []NW;
    delete []W;
    delete []SW;
    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* completeLines(const QImage* image) {
    QImage* newImage;
    int f[] = {
                  255, 0, 255,
                  255, 255, 255,
                  255, 0, 255, //S

                  255, 255, 255,
                  0, 255, 0,
                  255, 255, 255, //SE

                  0, 255, 255,
                  255, 255, 255,
                  255, 0, 255, //E

                  255, 255, 0,
                  0, 255, 255,
                  255, 255, 255, //NE

                  255, 0, 255,
                  255, 255, 255,
                  255, 255, 0, //N

                  255, 255, 255,
                  255, 255, 0,
                  0, 255, 255, //NW

                  0, 255, 255,
                  255, 255, 255,
                  255, 255, 0, //W

                  255, 255, 0,
                  255, 255, 255,
                  0, 255, 255 //SW
              };
    newImage = filterBinary(image, f, 3, true);
    return newImage;
}

QImage* thinnerLines(const QImage* image) {
    QImage* newImage;
    int f[] = {
                  0, 128, 255,
                  0, 0, 255,
                  128, 255, 255, //S

                  128, 0, 0,
                  255, 0, 128,
                  255, 255, 255, //SE

                  128, 0, 128,
                  0, 0, 128,
                  128, 128, 255, //E

                  128, 0, 128,
                  128, 0, 255,
                  255, 128, 255, //NE

                  255, 255, 255,
                  128, 0, 255,
                  0, 0, 128, //N

                  255, 255, 128,
                  255, 0, 0,
                  255, 128, 0, //NW

                  128, 128, 255,
                  0, 0, 128,
                  128, 0, 128, //W

                  255, 128, 128,
                  128, 0, 0,
                  128, 0, 128 //SW
              };
    newImage = filterBinary(image, f, 3, false);
    return newImage;
}
