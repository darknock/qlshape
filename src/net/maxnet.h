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

#ifndef MAXNET_H
#define MAXNET_H

#include <vector>
#include "net.h"

namespace darknocksBrain {

    class Node;

    class Maxnet : public Net {
        public:
            Maxnet(int count, Net* parent);
            ~Maxnet();
            void cleanup();
            void init(std::vector<double> dh);
            std::vector<double> reply();
            bool isConvergent();
            bool updateState();
            std::vector<double> resolve(std::vector<double> dh);
        private:
            Net* parent;
            int count;
    };

}

#endif //MAXNET_H
