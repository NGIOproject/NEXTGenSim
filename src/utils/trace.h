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
 * File:   trace.h
 * Author: nick
 *
 * Created on 15 March 2018, 14:34
 */

#ifndef TRACE_H
#define TRACE_H

#include <deque>
#include <vector>

#include <scheduling/resourcebucket.h>
#include <scheduling/partition.h>
#include <utils/log.h>

using namespace std;
using namespace Simulator;

namespace Utils {

    class trace {
    public:
        trace();
        trace(const trace& orig);
        virtual ~trace();
        virtual void setLog(Log* log);
        virtual void jobStart(int job_id, deque<ResourceBucket*>& allocations, bool do_io);
        virtual void jobEnd(int job_id, deque<ResourceBucket*>& allocations, bool do_io);
        
        virtual void jobBeginCompute(int job_id);
        virtual void jobEndCompute(int job_id);
        virtual void init(string, string, Partition*, int);
        virtual void simFinished(uint32_t);
//    private:
//        Log* log;
//        std::string tracedir;
//        std::string tracename;

    };

}
#endif /* TRACE_H */

