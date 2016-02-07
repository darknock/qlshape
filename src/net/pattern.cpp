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

#include "pattern.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
// #include <QtDebug>

namespace darknocksBrain {

    bool PatternSet::push_back(Pattern p) {
        if(inputSize == p.input.size() && outputSize == p.output.size()) {
            contents.push_back(p);
            return true;
        } else {
            std::cerr << " invalid size of pattern\n";
            return false;
        }
    }

    bool PatternSet::push_back(std::vector<double> input, std::vector<double> output) {
        Pattern temp(input, output);
        return push_back(temp);
    }

    void PatternSet::load(std::string inputFile) {
        contents.clear();
        std::ifstream ifs(inputFile.c_str());
        if ( !ifs ) 
            throw std::runtime_error("Cannot open file " + inputFile);

        std::string s;
        char c;
        while( c != '$' && !ifs.fail() ) {
            ifs.get(c);
            if( c == '\n') continue;
            if( c == ' ') continue;

            if( c == '@' ) {
                s.clear();
                while( c != '!' && !ifs.fail() ) {
                    ifs.get(c);
                    s.append(1, c);
                }
                std::istringstream ss(s);
                ss >> inputSize;
//                 qDebug() << "load: inputSize = " << inputSize;
            }

            if( c == '!' ) {
                s.clear();
                while( c != '$' && !ifs.fail() ) {
                    ifs.get(c);
                    s.append(1, c);
                }
                std::istringstream ss(s);
                ss >> outputSize;
//                 qDebug() << "load: outputSize = " << outputSize;
            }
        }

        if ( c != '$' )
            throw std::runtime_error("Invalid file format in file: " + inputFile + "\n missing '$'?");

        bool in;
        std::vector<double> vIn, vOut;
        while(!ifs.fail()) { 
            ifs.get(c);

            if( c == '\n') continue;
            if( c == ' ') continue;

            if( c == '@' ) {
                in = true;
                push_back(vIn, vOut);
                vIn.clear();
                vOut.clear();
                continue;
            } else if( c == '!' ) {
                in = false;
                continue;
            }

            if( in ) {
                if( c == '#' )
                    vIn.push_back(1);
                else if( c == '.' )
                    vIn.push_back(-1);
            } else {
                if( c == '#' )
                    vOut.push_back(1);
                else if( c == '.' )
                    vOut.push_back(-1);
            }
        }

        if( c != '\n' )
            throw std::runtime_error("Invalid file format in file: " + inputFile + "\n missing End Of Line at the bottom?");
        push_back(vIn, vOut);

        ifs.close();
    }
}
