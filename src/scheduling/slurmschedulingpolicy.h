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
#ifndef SIMULATORSLURMSCHEDULINGPOLICY_H
#define SIMULATORSLURMSCHEDULINGPOLICY_H

#include <scheduling/schedulingpolicy.h>
#include <scheduling/resourcereservationtable.h>
#include <utils/slurmjobqueue.h>
#include <utils/estimatefinishjobqueue.h>
#include <utils/realfinishjobqueue.h>

using std::vector;
using std::map;

namespace Simulator {
    
    enum slurm_persistent_memory {
        AVAILABLE = 0,
        NOT_AVAILABLE,
        COWBOYNEAL
    };

    typedef pair <int, Job*> pairJob;

    /**
     * This class implements a scheduling policy that mimics the one used in SLURM.
     * See the super-class schedulingpolicy too.
     */
    class SLURMSchedulingPolicy : public SchedulingPolicy {
    public:
        SLURMSchedulingPolicy();
        SLURMSchedulingPolicy(Utils::ArchitectureConfiguration* config, Log* log, uint64_t globalTime, bool simulateResourceUsage, int numberOfReservations, bool malleableExpand, bool malleableShrink);
        ~SLURMSchedulingPolicy();

        /* Inherited functions from the SchedulingPolicy class */
        void jobArrive(Job* job) override; //Adds a given job to the wait queue, usually when it arrives to the system
        void jobFinish(Job* job) override; //Removes a job from the RT
        void jobTransitionToCompute(Job* job) override;
        void jobTransitionToOutput(Job* job) override;
        Job* jobChoose() override; //Chooses the more appropiate job to run
        void backfill() override; //Chooses the more appropiate job to run
        void jobStart(Job* job) override; //Starts a job
        double getJobsIntheWQ() override;
        double getLeftWork() override;
        double getJobsIntheRQ() override;
        void setGlobalTime(uint64_t globalTime) override;
//        void Reschedule();
        void schedule() override;
        size_t getWQSize();

    private:
        ResourceJobAllocation* findBestAllocation(Job *job, ResourceJobAllocation* alloc);
        ResourceJobAllocation* findFutureAllocation(Job* job, double startTime, double runtime);
        ResourceJobAllocation* findNowAllocation(Job* job, double startTime, double runtime);
        vector<uint32_t> getPreceedingNodes(Job* job);
        
        void scheduleJob(Job* job, ResourceJobAllocation* allocation);
        /*
         * This nomenclature is dumb.
         * What I need is:
         * waitingQueue; where jobs sit once they have arrived
         * scheduledQueue; where jobs go once they have been placed (ie allocated plus events set), but not yet started
         * runningQueue; where jobs go once they have begun running
         * finishedQueue; where jobs go once they have finished
         */
        
        SLURMJobQueue waitingQueue; // Jobs that are yet to be scheduled
        SLURMJobQueue scheduledQueue; // Jobs that are scheduled but have not yet started
        SLURMJobQueue runningQueue; // Jobs that are currently running
        SLURMJobQueue finishedQueue; // Jobs that have finished
        
        
        
        //RealFinishJobQueue realFiniQueue; /**< The runing and finished queue job list ordered by real finish time*/
        
        //SLURMJobQueue startNowQueue; /**< Jobs that will start now */
        

        bool simulateResourceUsage; /**< Simulate resource usage */
        double lastAllowedStartTime; /**< Indicates the last allowed start time for the job  for do not violate the FCFS restriction */
        int numberOfReservations; /**< Number of reservations for backfill */
        int malleableExpand; /**< If malleable expand should be enabled */
        int malleableShrink; /**< If malleable shrink should be enabled */
        double lastscheduletime; // When was the last time the schedule function ran - as to suppress multiple runs in a short period to mimic SLURM.
        int bf_counter; // Temp counter to ensure running back on Nth run of SCHEDULE.
        bool skip_backfill;
        map<uint32_t, vector<uint32_t> > jobnodes;
        enum slurm_persistent_memory persist;

    };

}

#endif
