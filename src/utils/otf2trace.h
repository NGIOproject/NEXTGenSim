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
 * File:   otf2trace.h
 * Author: nick
 *
 * Created on 22 November 2017, 13:39
 */

#ifndef OTF2TRACE_H
#define OTF2TRACE_H

#include <string>
#include <deque>
#include <vector>

#include <scheduling/resourcebucket.h>
#include <scheduling/resourceschedulingpolicy.h>
#include <utils/log.h>
#include <utils/trace.h>

#include <otf2xx/otf2.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace Simulator;
using namespace boost::filesystem;

namespace Utils {

    class otf2trace : public trace {

        struct jobregion {
            otf2::definition::region region; // The region which is being operated on
            int jobid; // The job ID associated with this region
            uint32_t nodeid; // The node is (from bucket->getID) which this region runs on
        };

    public:
        otf2trace(void);
        otf2trace(const otf2trace& orig);
        ~otf2trace();
        //        OTF2_TimeStamp get_time(void);
        void jobStart(int job_id, deque<ResourceBucket*>& allocations, bool dummy) override;
        void jobEnd(int job_id, deque<ResourceBucket*>& allocations, bool dummy) override;
        void init(string, string, Partition*, int) override;
        void simFinished(uint32_t) override;
        void setLog(Log* log) override;
        
        void jobBeginCompute(int job_id) override;
        void jobEndCompute(int job_id) override;
        //        OTF2_FlushType pre_flush(void* userData, OTF2_FileType fileType, OTF2_LocationRef location, void* callerData, bool final);
        //        OTF2_TimeStamp post_flush(void* userData, OTF2_FileType fileType, OTF2_LocationRef location);

        /*
         * Quick sketch of what we need based on the example in doit()
         * Ideally, this should be a subset of a tracing class so elsewhere we just call "Trace::JobStart", "Trace::JobEnd" etc.
         * OTF2Init() - so we can open files, not in the constructor
         * OTF2JobStart()
         * OTF2JobEnd()
         * 
         * 
         */
    private:
        Log* log;
        string tracedir;
        string tracename;
        vector<struct jobregion> job_regions;
        vector<otf2::definition::system_tree_node> nodes;
        vector<otf2::definition::location_group> location_groups;
        vector<otf2::definition::location> locations;
        vector<bool> locations_used = {false};
        vector<int> node_locations = {0}; // node_locations[X] holds the index in locations that node X writes via.
        uint numNodes = 0;
        otf2::writer::archive* p_ar = NULL;
        otf2::definition::container<otf2::definition::string> strings;
        std::size_t regioncount = 0;
        ResourceSchedulingPolicy * rsp;
        unsigned int numJobs = 0;
    };

}

#endif /* OTF2TRACE_H */

