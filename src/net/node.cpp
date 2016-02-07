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

#include <iostream>
// #include <QtDebug>
#include <math.h>
#include "net.h"
#include "node.h"

namespace darknocksBrain {

    Node::Node(int count, Net* parent, activationType type, double param)
        : parent(parent), count(count), type(type), param(param) {
        try {
            weight = new double[count];
        } catch ( std::bad_alloc& ex ) {
            std::cerr << ex.what() << std::endl;
            throw new std::bad_exception;
        }
        potencial = 0;
    }


    double Node::update() {
        double newPotencial = 0;
        for(int i = 0; i < count; ++i) {
            newPotencial += parent->node[i]->potencial * weight[i];
//             qDebug() << "parent->node[" << i << "] == " << parent->node[i]->potencial << "* weight[" << i << "] == " << weight[i] << " --> = " << parent->node[i]->potencial * weight[i];
        }
//         qDebug() << "node& = " << this << "->oldPotencial = " << (potencial > 0 ? potencial : 0);
//         qDebug() << "node& = " << this << "->newPotencial = " << (newPotencial > 0 ? newPotencial : 0);
        return newPotencial > 0 ? newPotencial : 0;
    }

    double Node::reply() {
        switch(type) {
            case None:
                return potencial;
            case DHamming:
                return dhamming();
            case Unipolar:
                return unipolar();
            case Bipolar:
                return bipolar();
            case Sigmoidal:
                return sigmoidal();
            case Tangensoidal:
                return tangensoidal();
            case Signum:
                return signum();
            case Linear:
                return linear();
            default:
                std::cerr << "Node::reply() -> something strange happened";
                exit(-1);
        }
        return 0;
    }

    double Node::dhamming() {
        double r = (potencial + count) / (2 * count);
//         qDebug() << this << "potencial =" << potencial << endl << "dhamming" << r;
        return r;
    }

    double Node::unipolar() {
        return potencial > param ? 1 : 0;
    }

    double Node::bipolar() {
        return potencial > param ? 1 : -1;
    }

    double Node::sigmoidal() {
        return 1. / ( 1. + exp(-param * potencial));
    }

    double Node::tangensoidal() {
        return tanh(param * potencial / 2) * (1 - exp(-param * potencial)) / (1 + exp(-param * potencial));
    }

    double Node::signum() {
        return potencial > 0 ? 1 : -1;
    }

    double Node::linear() {
        return param * potencial;
    }
}
