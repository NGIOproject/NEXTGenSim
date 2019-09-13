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
#ifndef SIMULATORResourceSchedulingPolicy_H
#define SIMULATORResourceSchedulingPolicy_H

#include <scheduling/schedulingpolicy.h>

namespace Simulator {

    /**
     @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
     */

    class ResourceJobAllocation;

    /** This class is an intermediate class that aggregates all the methods for those Scheduling policies that are using a analogical reservation table. For instance the findAllocation methods */
    class ResourceSchedulingPolicy : public SchedulingPolicy {
    public:
        ResourceSchedulingPolicy();
        ~ResourceSchedulingPolicy();

    protected:
        /* protected methods */
        /* the following three methods are virtual since other easy backfilling variants may need to tunne the behavriour of the allocation procedure */
        virtual ResourceJobAllocation* findAllocation(Job* job, double startTime, double runtime);
        virtual ResourceJobAllocation* findBestAllocation(Job *job, ResourceJobAllocation* alloc) = 0;
        virtual void releaseJobResources(Job* job);

        /**
         * Allocates the job to the provided allocation 
         * @param job The job to allocated 
         * @param allocation The allocation for the job 
         */
        virtual void allocateJob(Job* job, ResourceJobAllocation* allocation) = 0;

    };

}

#endif
