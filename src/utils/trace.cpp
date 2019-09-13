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
 * File:   Trace.cpp
 * Author: nick
 * 
 * Created on 15 March 2018, 14:34
 */

#include <utils/trace.h>

#include "trace.h"

namespace Utils {

    trace::trace() {
    }

    trace::trace(const trace& orig) {
    }

    trace::~trace() {
    }

    void trace::jobStart(int job_id, deque<ResourceBucket*>& allocations, bool do_io) {
    }
    //

    void trace::jobEnd(int job_id, deque<ResourceBucket*>& allocations, bool do_io) {
    }
    

    void trace::init(string, string, Partition*, int) {
    }

    void trace::simFinished(uint32_t) {
    }

    void trace::jobBeginCompute(int) {
    }

    void trace::jobEndCompute(int) {
    }


    void trace::setLog(Log* log) {
        //        this->log = log;
        //        log->debug("Log set", 1);
        //        log->debug("Output filename for TRACE set to " + this->tracename, 1);
        //        log->debug("Output directory for TRACE set to " + this->tracedir, 1);
    }


}