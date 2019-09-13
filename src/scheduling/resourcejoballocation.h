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
#ifndef RESOURCEJOBALLOCATION_H
#define RESOURCEJOBALLOCATION_H

#include <scheduling/joballocation.h>
#include <scheduling/resourcebucket.h>

#include <deque>
#include <vector>

namespace Simulator {

    /**
     * @author Daniel Rivas, daniel.rivas@bsc.es
     */

    /**
     * This class represents an resource allocation of a job. 
     * @see The joballocation class
     */
    class ResourceJobAllocation : public JobAllocation {
    public:
        ResourceJobAllocation(const std::deque< ResourceBucket * >& allocs, double startTime, double endTime);
        ResourceJobAllocation();
        virtual ~ResourceJobAllocation();

        void setAllocations(const deque<ResourceBucket*>& allocs);
        void addAllocations(deque< ResourceBucket* >& allocs);
        std::deque<ResourceBucket*> getAllocations() const;

        void setNextStartTimeToExplore(double nextStartTime);
        double getNextStartTimeToExplore() const;
        

        

    private:
        std::deque<ResourceBucket*> allocations; /**< Buckets associated to the current allocation */
        double nextStartTimeToExplore; /**< this variable inidicates where to explore if this allocation is not satisfactory for the ones who asked for this */
        

    };

}

#endif
