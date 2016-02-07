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

#include "net.h"
#include "node.h"
#include "maxnet.h"

#include <iostream>

// #include <QtDebug>

namespace darknocksBrain {
    Maxnet::Maxnet(int count, Net* parent) : parent(parent), count(count) {
        try {
            node = new Node*[count];
            Node* temp;
            for(int i = 0; i < count; ++i)
                try {
                    temp = new Node(count, this, Unipolar, 0);
                    node[i] = temp;
                } catch ( std::bad_alloc& ex ) {
                    std::cerr << ex.what() << std::endl;
                    for(int j = i - 1; j >= 0; --j) {
                        delete node[j];
                    }
                    this->cleanup();
                    exit(-1);
                } catch ( std::bad_exception& ex ) {
                    std::cerr << ex.what() << std::endl;
                    for(int j = i; j >= 0; --j) {
                        delete node[j];
                    }
                    this->cleanup();
                    exit(-1);
                }
        } catch ( std::bad_alloc& ex ) {
            std::cerr << ex.what() << std::endl;
            this->cleanup();
            exit(-1);
        }
        //initialize the weigths
        for(int i = 0; i < count; ++i)
            for(int j = 0; j < count; ++j) {
                if(i == j)
                    node[i]->weight[j] = 1;
                else
                    node[i]->weight[j] = -1.0 / ( count - 1 );
//                 qDebug() << "node[" << i <<"]->weight[" << j << "] = " << node[i]->weight[j];
            }

    }

    void Maxnet::cleanup() {
        delete [] node;
        if(parent) parent->cleanup();
    }

    Maxnet::~Maxnet() {
        for(int j = 0; j < count; ++j) {
            delete node[j];
        }
        delete [] node;
    }

    std::vector<double> Maxnet::reply() {
        std::vector<double> temp;
        for(int i = 0; i < count; ++i) {
            temp.push_back(node[i]->reply());
//             qDebug() << "currentState: temp[" << i << "] = " << temp[i];
        }
        return temp;
    }

    ///recives input from the first layer and returns the "Winner", if there is no convergence function returns an empty vector
    std::vector<double> Maxnet::resolve(std::vector<double> dh) {
        init(dh);
        bool t = false;
        int epoch = 0;
        if(!isConvergent()) {
//             qDebug() << "isConvergent = " << false;
            std::vector<double> temp;
            return temp;
        }
        while(!(t = updateState())) {
            epoch++;
//             qDebug() << "Maxnet::resolve: epoch = " << d++ << t;
        }
//         std::cerr << "resolved at epoch = " << epoch << "\n";
//         qDebug() << "Maxnet::resolve: epoch = " << d << t;
        return reply();
    }

    ///if "true" the recursion ended, else returns "false"
    bool Maxnet::updateState() {
        std::vector<double> newPotentials;
        double temp;
        int nonZeros = 0;

        for(int i = 0; i < count; ++i) {
            if(node[i]->potencial == 0)
                newPotentials.push_back(0);
            else {
                temp = node[i]->update();
                newPotentials.push_back(temp);
                if(temp != 0) nonZeros++;
            }
        }
        init(newPotentials);
//         qDebug() << "nonZeros = " << nonZeros;
        if(nonZeros > 1) return false;
        return true;
    }


    bool Maxnet::isConvergent() {
        std::vector<Node*> temp;
        for(int i = 0; i < count; ++i)
            if(node[i]->potencial != 0) temp.push_back(node[i]);

        double max = -1;
        int same = 1;
        for(unsigned i = 0; i < temp.size(); ++i) {
            if(max == temp[i]->potencial) same++;
            if(max < temp[i]->potencial) {
                max = temp[i]->potencial;
                same = 1;
            }
        }
        if(same > 1) return false;
        return true;
    }

    void Maxnet::init(std::vector<double> dh) {
        for(int i = 0; i < count; ++i)
            node[i]->potencial = dh[i];
    }
}
