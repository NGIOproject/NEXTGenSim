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
#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <scheduling/job.h>
#include <set>
#include <utils/log.h>

/**
 * @author Francesc Guim,C6-E201,93 401 16 50,
 */

/** This function defines the methods that the job queues have to provide. This is a pure abstract class.*/
using namespace Simulator;

namespace Utils{

    class JobQueue {
    public:
        /*Constructors and destructors*/
        JobQueue();
        ~JobQueue();

        /*Auxiliar and main methods*/
        virtual void insert(Job* job) = 0;
        virtual void erase(Job* job) = 0;
        virtual Job* headJob() = 0;
        virtual Job* next() = 0;
        virtual void deleteCurrent() = 0;
        virtual Job* begin() = 0;

        virtual bool contains(Job* job) = 0;

        void deleteSet(set<Job*>* theSet);
        void setJobs(const int& theValue);
        int getJobs() const;
        void setLog(Log *log);
        Log* log;


    protected:
        int jobs; /**<Contains the number of jobs stored in the queue, is only for checking the sanity of the queue */
        
           

    };
}
#endif
