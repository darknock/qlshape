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

#include "aieffects.h"
#include "effects.h"
#include "net/hamming.h"

#include <vector>
#include <math.h>

#include <QImage>
#include <QRect>
#include <QtDebug>

using namespace darknocksBrain;

std::vector<double>* derivative(const QImage* image, bool blur) {
    QImage* blured;
    if(blur) blured = gaussianBlur(image);
    else blured = new QImage(*image);
    std::vector<double>* output = new std::vector<double>;
    int f[] = {
        1, 1, 1,
        0, 0, 0,
        -1,-1,-1, //S

        1, 0,-1,
        1, 0,-1,
        1, 0,-1, //E
    };
    int size = 3;
    int tn = size * size, t = 0, pixel;
    double gray;
    int *S, s, *E, e;
    S = new int[tn];
    E = new int[tn];
    for (int i = 0; i < tn; i++) {
        S[i] = f[i];
        E[i] = f[i + tn];
    }

    for (int x = 0; x < image->width(); x++) output->push_back(0);
    for (int y = size/2; y < image->height() - size/2; y++) {
        output->push_back(0);
        for (int x = size/2; x < image->width() - size/2; x++) {
            s = e = 0;
            for (int i = -size/2; i <= size/2; i++)
                for (int j = -size/2; j<=size/2; j++) {
                t < tn ? t : t = 0;
                pixel = qGray(blured->pixel(x + i, y + j));
                s += pixel * S[t];
                e += pixel * E[t];
                t++;
            }
            gray = sqrt(s * s + e * e);
            output->push_back( gray );
        }
        output->push_back(0);
    }
    for (int x = 0; x < image->width(); x++) output->push_back(0);

    delete blured;
    delete []S;
    delete []E;
    return output;
}

void normalize(std::vector<double>* temp, double thold) {
    const double yMin = 0., yMax = 1.;
    double xMin = temp->at(0), xMax = temp->at(0);
    for( unsigned i = 0; i < temp->size(); ++i) {
        xMin = xMin > temp->at(i) ? temp->at(i) : xMin;
        xMax = xMax < temp->at(i) ? temp->at(i) : xMax;
    }
    double n =  thold * 0.01 * (xMax - xMin);
    for( unsigned i = 0; i < temp->size(); ++i) {
        if( temp->at(i) > n )
            temp->at(i) = yMin + (temp->at(i) - n) * (yMax * 1.0 / (xMax - n));
        else
            temp->at(i) = yMin + (temp->at(i) - xMin) * (yMax * 1.0 / (n - xMin)) - 1;
    }
}

void normalize(std::vector<double>* temp, double thold, double xMin, double xMax) {
    const double yMin = 0., yMax = 1.;
    double n =  thold * 0.01 * (xMax - xMin);
    for( unsigned i = 0; i < temp->size(); ++i) {
        if( temp->at(i) > n )
            temp->at(i) = yMin + (temp->at(i) - n) * (yMax * 1.0 / (xMax - n));
        else
            temp->at(i) = yMin + (temp->at(i) - xMin) * (yMax * 1.0 / (n - xMin)) - 1;
    }
}

QImage* hammingAiEdges(const QImage* image, Hamming* hamming, int thold, bool blur) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int size = 3;
    std::vector<double>* input = derivative(image, blur);
    normalize(input, thold);

    std::vector<double> temp;
    std::vector<double> reply;
    double response;
    for (int y = size/2; y < image->height() - size/2; y++)
        for (int x = size/2; x < image->width() - size/2; x++) {
            temp.clear();
            for (int j = -size/2; j<=size/2; j++)
                for (int i = -size/2; i <= size/2; i++) {
                    temp.push_back( input->at( (x + i) + image->width() * (y + j) ) );
                }

            reply = hamming->resolve(temp);
            response = reply.at(0);
            if( response == 0) newImage->setPixel(x, y, qRgb(255, 255, 255));
            if( response == 0.5) newImage->setPixel(x, y, qRgb(255, 255, 255));
            if( response == 1) newImage->setPixel(x, y, qRgb(0, 0, 0));
            if( response == -1) newImage->setPixel(x, y, qRgb(255, 255, 255));
        }

    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
        if (i < size/2 || i > image->width() - size/2 - 1)
            newImage->setPixel(i, j, 0);
        if (j < size/2 || j > image->height() - size/2 - 1)
            newImage->setPixel(i, j, 0);
        }

    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* previewHammingAiEdges(const QImage* image, std::vector<double>* input, QRect rect, double xMin, double xMax, darknocksBrain::Hamming* hamming, int thold, bool blur) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int size = 3;
    normalize(input, thold, xMin, xMax);
    std::vector<double> temp;
    std::vector<double> reply;
    double response;
    for (int y = size/2; y < image->height() - size/2; y++)
        for (int x = size/2; x < image->width() - size/2; x++) {
            temp.clear();
            for (int j = -size/2; j<=size/2; j++)
                for (int i = -size/2; i <= size/2; i++) {
                        temp.push_back( input->at( (x + i + rect.x()) + rect.width() * (y + j + rect.y()) ) );
                }
                reply = hamming->resolve(temp);
                response = reply.at(0);
                if( response == 0) newImage->setPixel(x, y, qRgb(255, 255, 255));
                if( response == 0.5) newImage->setPixel(x, y, qRgb(255, 255, 255));
                if( response == 1) newImage->setPixel(x, y, qRgb(0, 0, 0));
                if( response == -1) newImage->setPixel(x, y, qRgb(255, 255, 255));
            }

    for (int i = 0; i < image->width(); ++i)
        for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            }

    newImage->setAlphaChannel(image->alphaChannel());
    QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
    delete newImage;
    return converted;
}

QImage* hammingAiEdges(const QImage* image, std::vector<double>* input, double xMin, double xMax, Hamming* hamming, int thold, bool blur) {
    QImage* newImage = new QImage(image->width(), image->height(), image->format());
    int size = 3;
    normalize(input, thold, xMin, xMax);

    std::vector<double> temp;
    std::vector<double> reply;
    double response;
    for (int y = size/2; y < image->height() - size/2; y++)
        for (int x = size/2; x < image->width() - size/2; x++) {
        temp.clear();
        for (int j = -size/2; j<=size/2; j++)
            for (int i = -size/2; i <= size/2; i++) {
            temp.push_back( input->at( (x + i) + image->width() * (y + j) ) );
            }

            reply = hamming->resolve(temp);
            response = reply.at(0);
            if( response == 0) newImage->setPixel(x, y, qRgb(255, 255, 255));
            if( response == 0.5) newImage->setPixel(x, y, qRgb(255, 255, 255));
            if( response == 1) newImage->setPixel(x, y, qRgb(0, 0, 0));
            if( response == -1) newImage->setPixel(x, y, qRgb(255, 255, 255));
        }

        for (int i = 0; i < image->width(); ++i)
            for (int j = 0; j < image->height(); ++j) {
            if (i < size/2 || i > image->width() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            if (j < size/2 || j > image->height() - size/2 - 1)
                newImage->setPixel(i, j, 0);
            }

            newImage->setAlphaChannel(image->alphaChannel());
            QImage *converted = new QImage(newImage->convertToFormat(QImage::Format_ARGB32));
            delete newImage;
            return converted;
}

