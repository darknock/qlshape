/***************************************************************************
 *   Copyright (C) 2007 - 2016 by Konrad Ciekot                            *
 *   darknock@gmail.com                                                    *
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


#ifndef NODE_H
#define NODE_H

#include "net.h"

namespace darknocksBrain {

    class Node {
        public:
            Node(int count, Net* parent, activationType type = None, double param = 0);
            ~Node() { delete [] weight; }
            double getPotencial();
            double update();
            double* weight;
            double potencial;
            double reply();
        private:
            Net* parent;
            int count;
            activationType type;
            double param;
            double dhamming();
            double unipolar();
            double bipolar();
            double sigmoidal();
            double tangensoidal();
            double signum();
            double linear();
    };

}

#endif //NODE_H
