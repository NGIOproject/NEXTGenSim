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
#ifndef SIMULATORFCFSSCHEDULINGPOLICY_H
#define SIMULATORFCFSSCHEDULINGPOLICY_H

#include <scheduling/schedulingpolicy.h>
#include <scheduling/resourcereservationtable.h>
#include <utils/fcfsjobqueue.h>
#include <utils/estimatefinishjobqueue.h>
#include <utils/realfinishjobqueue.h>

namespace Simulator
{

    
    typedef pair <int, Job*> pairJob;

    /**
     * @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>		
     */

    /**
     * This class implements the more simplest schedulingpolicy the first-come-first-serve
     * @see the class schedulingpolicy
     */
    class FCFSSchedulingPolicy : public SchedulingPolicy
    {
    public:
        FCFSSchedulingPolicy();
        FCFSSchedulingPolicy(Utils::ArchitectureConfiguration* config, Utils::Log* log, uint64_t globalTime, bool simulateResourceUsage, int numberOfReservations, bool malleableExpand, bool malleableShrink);
        ~FCFSSchedulingPolicy();

        /* iherited functions from the SchedulingPolicy class */
        void jobArrive(Job* job) override; //Adds a given job to the wait queue, usually when it arrives to the system
        void jobFinish(Job* job) override; //Removes a job from the RT
        Job* jobChoose() override; //Chooses the more appropiate job to run 
        void jobStart(Job* job) override; //Starts a job    
        double getJobsIntheWQ() override;
        double getLeftWork() override;
        double getJobsIntheRQ() override;
        Metric* EstimatePerformanceResponse(metric_t MetricTpye, Job* job) override;
        void setGlobalTime(uint64_t globalTime) override;
        void Reschedule(); // Not pure virtual
        void schedule() override;
        void jobAddToWaitQueue(Job *job);  // Not pure virtual
        void scheduleMalleableExpand();  // Not pure virtual
        void backfill() override;
        void jobTransitionToCompute(Job* job) override;
        void jobTransitionToOutput(Job* job) override;

        void setReservationTable(ResourceReservationTable *rt);

        virtual void allocateJob(Job* job, ResourceJobAllocation* allocation);
        virtual void expandJobAllocation(Job* job, ResourceJobAllocation* allocation);
        virtual bool shrinkJobAllocation(Job *job, int cpus);

        virtual bool searchResourcesInMalleableJobs(ResourceRequirements lack);

        size_t getWQSize();
    private:
        ResourceJobAllocation* findBestAllocation(Job *job, ResourceJobAllocation* alloc);
        ResourceJobAllocation* findBestExtraAllocation(Job *job, ResourceJobAllocation* alloc);
        ResourceJobAllocation* findAllocation(Job* job, double startTime, double runtime);
        
        FCFSJobQueue waitQueue; /**< The wait queue job list*/
        RealFinishJobQueue realFiniQueue; /**< The runing and finished queue job list ordered by real finish time*/
        FCFSJobQueue runningQueue; /**< The runing queue ordered by ESF*/
        FCFSJobQueue startNowQueue; /**< Jobs that will start now */
        FCFSJobQueue reservedQueue; /**< Jobs that are reserved to start now */

        bool simulateResourceUsage; /**< Simulate resource usage */
        double lastAllowedStartTime; /**< Indicates the last allowed start time for the job  for do not violate the FCFS restriction */
        int numberOfReservations; /**< Number of reservations for backfill */
        int malleableExpand; /**< If malleable expand should be enabled */
        int malleableShrink; /**< If malleable shrink should be enabled */

    };

}

#endif
