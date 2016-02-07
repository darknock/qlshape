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

#ifndef PATTERN_H
#define PATTERN_H

#include <vector>
#include <string>
#include <iostream>

namespace darknocksBrain {

    class Pattern {
      public:
        Pattern(std::vector<double> input, std::vector<double> output)
            : input(input), output(output) {}
        ~Pattern() {}
        std::vector<double> input;
        std::vector<double> output;
    };

    class PatternSet {
      public:
        PatternSet(unsigned inputSize, unsigned outputSize)
            : inputSize(inputSize), outputSize(outputSize) { 
            if(inputSize == 0 || outputSize == 0) {
                std::cerr << "patterns input and output should be bigger than zero!";
                exit(-1);
            }
        }

        PatternSet(std::string inputFile) {
            load(inputFile);
        }

        ~PatternSet() {}

        bool push_back(Pattern p);

        bool push_back(std::vector<double> input, std::vector<double> output);

        bool isEmpty() {
            return contents.size() == 0;
        }

        void load(std::string inputFile);

        std::vector<Pattern> contents;
        unsigned inputSize;
        unsigned outputSize;
    };
}

#endif //PATTERN_H
