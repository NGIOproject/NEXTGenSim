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
#include <utils/jobqueue.h>
namespace Utils {

    /***************************
      Constructor and destructors
     ***************************/

    /**
     * The default constructor for the class
     */
    JobQueue::JobQueue() {
        this->jobs = 0;
    }

    /**
     * The default destructor for the class 
     */
    JobQueue::~JobQueue() {
    }

    /**
     * Returns the number of jobs that are queued in the queue 
     * @return An integer containing the number of queued jobs 
     */
    int JobQueue::getJobs() const {
        return jobs;
    }

    /**
     * Sets the number of jobs queued in the queue 
     * @param theValue The number of queued jobs 
     */
    void JobQueue::setJobs(const int& theValue) {
        jobs = theValue;
    }

    void JobQueue::deleteSet(set<Job*>* theSet) {
        for (set<Job*>::iterator sIt = theSet->begin();
                sIt != theSet->end();
                sIt++)
            this->erase(*sIt);
    }
    
    void JobQueue::setLog(Log *l){
        this->log = l;
    }
}

