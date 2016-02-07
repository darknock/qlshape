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

#ifndef AIEFFECTS_H
#define AIEFFECTS_H

#include <vector>

class QImage;
class QRect;
namespace darknocksBrain {
    class Hamming;
}

std::vector<double>* derivative(const QImage* image, bool blur);
QImage* hammingAiEdges(const QImage* image, darknocksBrain::Hamming* hamming, int thold, bool blur);
QImage* hammingAiEdges(const QImage* image, std::vector<double>* input, double xMin, double xMax, darknocksBrain::Hamming* hamming, int thold, bool blur);
QImage* previewHammingAiEdges(const QImage* image, std::vector<double>* input, QRect rect, double xMin, double xMax, darknocksBrain::Hamming* hamming, int thold, bool blur);

#endif //AIEFFECTS_H
