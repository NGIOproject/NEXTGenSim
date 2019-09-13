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
#ifndef SIMULATORSLURMJOBQUEUE_H
#define SIMULATORSLURMJOBQUEUE_H

#include <scheduling/job.h>
#include <utils/jobqueue.h>

#include <assert.h>
#include <set>

#define SLURM_JOB_Qt 0

using namespace std;
using std::set;

using namespace Simulator;

namespace Utils {

    /*
     * Comparison of two jobs based on the slurm priority
     * If job1 has higher priority than job2, return true
     * Else return false
     * If equal priority, if job1 has lower jobid, return true
     * Else return false
     * */
    struct slurm_priority_t {

        bool operator()(Job* job1, Job* job2) const {

            uint32_t p1 = job1->getslurmprio();
            uint32_t p2 = job1->getslurmprio();
            assert(p1 >= 0 && p2 >= 0);
            
            if (p1 != p2){
                return p1 > p2;    
            }            
            else {
                return job1->getJobNumber() < job2->getJobNumber();
            }
        }
    };

    /* Set based on a priority order */
    typedef set<Job*, slurm_priority_t> SLURMQueue;

    /* Implements a job queue where the order is the Priority  */
    class SLURMJobQueue : public JobQueue {
    public:
        /* Constructors and destructors*/
        SLURMJobQueue();
        ~SLURMJobQueue();

        /* Auxiliar functions and main functions */
        virtual void insert(Job* job);
        virtual void erase(Job* job);
        virtual Job* headJob();
        virtual Job* next();
        virtual Job* begin();
        virtual void deleteCurrent();
        virtual bool contains(Job* job);
        virtual size_t size();
        virtual void clear();
        void dump();

        SLURMQueue* getQueue();

    private:
        SLURMQueue queue; /**< Contains the queue of jobs.*/
        SLURMQueue::iterator currentIterator; /**< The iterator that points to the current position that is being queried to the queue */


    };

}

#endif
