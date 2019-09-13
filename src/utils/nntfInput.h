/*
* Copyright 2007 Francesc Guim Bernat & Barcelona Supercomputing Centre (fguim@pcmas.ac.upc.edu)
* Copyright 2019 Daniel Rivas & Barcelona Supercomputing Centre (daniel.rivas@bsc.es)
* Copyright 2015-2019 NEXTGenIO Project [EC H2020 Project ID: 671951] (www.nextgenio.eu)
*
* This file is part of NEXTGenSim.
*
* NEXTGenSim is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEXTGenSim is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with NEXTGenSim.  If not, see <https://www.gnu.org/licenses/>.
*/
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   nntfInput.h
 * Author: nick
 *
 * Created on 27 June 2018, 14:21
 */

#ifndef NNTFINPUT_H
#define NNTFINPUT_H

#include <scheduling/tracefile.h>
#include <utils/utilities.h>

using namespace Utils;

namespace Simulator {

    class nntfInput : public TraceFile {
    public:
        nntfInput();
        nntfInput(const nntfInput& orig);
        nntfInput(string FilePath, Log* log, bool is_output = false);
        virtual ~nntfInput();
        
        bool loadTrace();
        void processHeaderEntry(string headerLine);
              
        
    private:


    };

}
#endif /* NNTFINPUT_H */

