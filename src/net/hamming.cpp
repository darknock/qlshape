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

#include "hamming.h"

#include <iostream>

#include <QtDebug>

namespace darknocksBrain {

    Hamming::Hamming(PatternSet* patternSet) : patternSet(patternSet) {
        if(patternSet->isEmpty()) {
            std::cerr << "cannot learn from empty pattern set!";
            exit(-1);
        }

        inputsCount = patternSet->inputSize;
        patternsCount = patternSet->contents.size();
        outputsCount = patternSet->outputSize;

        ///initialization of the FIRST (dh) layer
        try {
            node = new Node*[patternsCount];
            Node* temp;
            for(int i = 0; i < patternsCount; ++i)
                try {
                    temp = new Node(inputsCount, this, DHamming);
                    node[i] = temp;
                } catch ( std::bad_alloc& ex ) {
                    std::cerr << ex.what() << std::endl;
                    for(int j = i - 1; j >= 0; --j) {
                        delete node[j];
                    }
                    exit(-1);
                } catch ( std::bad_exception& ex ) {
                    std::cerr << ex.what() << std::endl;
                    for(int j = i; j >= 0; --j) {
                        delete node[j];
                    }
                    exit(-1);
                }
        } catch ( std::bad_alloc& ex ) {
            std::cerr << ex.what() << std::endl;
            exit(-1);
        }

        ///initialization of weights for the first layer
        for(int i = 0; i < patternsCount; ++i) 
            for(int j = 0; j < inputsCount; ++j){
            node[i]->weight[j] = patternSet->contents[i].input[j];
//             qDebug() << "node[" << i << "]->weight[" << j << "] = " << patternSet->contents[i].input[j];
        }

        maxnet = new Maxnet(patternsCount, this);

        ///initialization of the OUTPUT layer
        try {
            outputNodes = new Node*[outputsCount];
            Node* temp;
            for(int i = 0; i < outputsCount; ++i)
                try {
                    temp = new Node(patternsCount, this);
                    outputNodes[i] = temp;
                } catch ( std::bad_alloc& ex ) {
                    std::cerr << ex.what() << std::endl;
                    for(int j = i - 1; j >= 0; --j) {
                        delete outputNodes[j];
                    }
                    exit(-1);
                } catch ( std::bad_exception& ex ) {
                    std::cerr << ex.what() << std::endl;
                    for(int j = i; j >= 0; --j) {
                        delete outputNodes[j];
                    }
                    exit(-1);
                }
        } catch ( std::bad_alloc& ex ) {
            std::cerr << ex.what() << std::endl;
            exit(-1);
        }

        ///initialization of weights for the output layer
        for(int i = 0; i < patternsCount; ++i) 
            for(int j = 0; j < outputsCount; ++j){
                outputNodes[j]->weight[i] = patternSet->contents[i].output[j];
//                 qDebug() << "outputNodes[" << j << "]->weight[" << i << "] = " << patternSet->contents[i].output[j];
            }
    }

    Hamming::~Hamming() {
        delete maxnet;

        for(int i = 0; i < patternsCount; ++i) {
            delete node[i];
        }
        delete [] node;

        for(int i = 0; i < outputsCount; ++i) {
            delete outputNodes[i];
        }
        delete [] outputNodes;
    }

    /// the main function, resolving class of the input
    std::vector<double> Hamming::resolve(std::vector<double> input) {
        if(input.size() != unsigned(inputsCount)) {
            std::cerr << "Hamming::resolve(): size of input is incorrect!";
            exit(-1);
        }

        ///counting the potencial of the first (dh) layer (this should be done by the nodes itself I know!)
        double net;
        std::vector<double> dh;
        for(int i = 0; i < patternsCount; ++i) {
            net = 0;
            for(int j = 0; j < inputsCount; ++j) {
                net += node[i]->weight[j] * input[j];
            }
            node[i]->potencial = net;
            dh.push_back(node[i]->reply());
        }

        ///getting tha maxnet response
        std::vector<double> max = maxnet->resolve(dh);
        std::vector<double> output;

        ///if the network is not shure of what class is the input returns empty vector
        if(max.size() == 0) {
            max.push_back(.5);
            return max;
        }

        for(int i = 0; i < outputsCount; ++i) {
            net = 0;
            for(int j = 0; j < patternsCount; ++j) {
                net += outputNodes[i]->weight[j] * max[j];
            }
            outputNodes[i]->potencial = net;
            output.push_back(outputNodes[i]->reply());
        }

        return output;
    }
}
