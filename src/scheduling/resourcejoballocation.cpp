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
#include <scheduling/resourcejoballocation.h>
#include <utils/utilities.h>

namespace Simulator {

    /**
     * The constructor for the class. 
     * @param theValue A reference to the set of buckets that model the allocation for the job
     * @param startTime The startTime for the allocation 
     * @param endTime The endTime for the allocation 
     */
    ResourceJobAllocation::ResourceJobAllocation(const deque<ResourceBucket*>& allocs, double startTime, double endTime)
    : JobAllocation(startTime, endTime) {
        this->allocations = allocs;
        this->nextStartTimeToExplore = 0;
    }

    /**
     * The default constructor for the class
     */
    ResourceJobAllocation::ResourceJobAllocation() {
        this->nextStartTimeToExplore = 0;
    }

    /**
     * The default destructor for the class 
     */
    ResourceJobAllocation::~ResourceJobAllocation() {
        for (deque<ResourceBucket*>::iterator iter = this->allocations.begin(); iter != this->allocations.end(); ++iter) {
            ResourceBucket* current = *iter;
            delete current;
        }
        //delete allocations;
    }

    /**
     * Returns the analogical buckets of the allocation 
     * @return A deque with the buckets 
     */
    std::deque<ResourceBucket*> ResourceJobAllocation::getAllocations() const {
        return allocations;
    }

    /**
     * Sets the analogical buckets of the allocation 
     * @param allocs A deque with the allocation 
     */
    void ResourceJobAllocation::setAllocations(const deque< ResourceBucket * >& allocs) {
        allocations = allocs;

        //we have to check that all the starts and endTimes matches to the global ones ..
        for (deque<ResourceBucket*>::iterator iter = this->allocations.begin(); iter != this->allocations.end(); ++iter) {
            ResourceBucket* current = *iter;
            assert(current->getStartTime() >= this->startTime && current->getEndTime() == this->endTime);
        }
    }

    //allocations = alloc 8 cpus + alloc 8 cpus
    //			resourcebuckdet	+  resourcebucket
    //allocations - alloc - 4 cpus
    //allocations - alloc - 4 cpus + alloc - 1 cpu + alloc - 1 cpu
    //allocations - alloc - 6 cpus 

    /**
     * Add more allocations 
     * @param new allocations
     */
    void ResourceJobAllocation::addAllocations(deque< ResourceBucket* >& allocs) {
        int i = 0;
        for (deque<ResourceBucket*>::iterator iter = allocs.begin(); iter != allocs.end(); ++iter) {
            ResourceBucket* current = *iter;
            int curBID = current->getID();
            int same = 0;
            deque<ResourceBucket*>::iterator allociter = allocations.begin();
            for (; allociter != allocations.end(); ++allociter) {
                ResourceBucket* alloccurrent = *allociter;
                int allocBID = alloccurrent->getID();
                if (curBID == allocBID) {
                    same = 1;
                    break;
                }
            }
            if (same != 1) {
                //cout << "adding allocation ID = " << current->getID() << endl;
                allocations.push_back(current);
            } else {
                //cout << "BID already allocated. Just expanding" << endl;
                (*allociter)->setStartTime(current->getStartTime());
            }

            assert(current->getEndTime() == this->endTime);
        }

        //cout << " After adding allocation " << allocations.size() << endl;
        for (deque<ResourceBucket*>::iterator iter = allocations.begin(); iter != allocations.end(); ++iter) {
            //cout << "Allcation number: " << i << " iter ID " << (*iter)->getID() << endl;
            i++;
        }

    }

    /**
     * When an allocation returned by the reservation table or by the scheduling policies has some problem it will return a double indicating when a possible allocation would be available that matches the requirement . For instance, that there are not enough cpus at the required time.
     * @return A double indicating a proposal of a time stamp when there would be an allocation matching the requirements.
     */
    double ResourceJobAllocation::getNextStartTimeToExplore() const {
        return nextStartTimeToExplore;
    }

    /**
     * Sets the next time to explore. 
     * @param nextStartTime The next time to explore 
     * @see getNextStartTimeToExplore
     */
    void ResourceJobAllocation::setNextStartTimeToExplore(double nextStartTime) {
        nextStartTimeToExplore = nextStartTime;
    }

}