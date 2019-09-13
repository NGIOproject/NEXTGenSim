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
#include <scheduling/fcfsschedulingpolicy.h>
#include <scheduling/resourcejoballocation.h>
#include <scheduling/resourcereservationtable.h>
#include <scheduling/simulatorcharacteristics.h>


#include <scheduling/simulation.h>

#include <math.h>
#include <unordered_map>
#include <utility>

namespace Simulator {

    /**
     * The default constructor for the class
     */

    FCFSSchedulingPolicy::FCFSSchedulingPolicy() {
    }

    /**
     * This is the constructor that should be used, invoques that constructor for the parent classes
     * @param log A reference to the logging engine
     * @param globalTime The startTime of the simulation
     */
    FCFSSchedulingPolicy::FCFSSchedulingPolicy(ArchitectureConfiguration *config, Log* log, uint64_t globalTime, bool simulateResourceUsage, int numberOfReservations, bool malleableExpand, bool malleableShrink) : SchedulingPolicy() {
        this->architecture = config;
        this->log = log;
        this->globalTime = globalTime;
        this->simulateResourceUsage = simulateResourceUsage;
        this->lastAllowedStartTime = 0;
        this->schedulingPolicySimulated = FCFS;
        this->numberOfReservations = numberOfReservations;
        this->malleableExpand = malleableExpand;
        this->malleableShrink = malleableShrink;
    }

    /**
     * The default destructor for the class
     */

    FCFSSchedulingPolicy::~FCFSSchedulingPolicy() {
        /* free the  reservationTable */
        delete this->reservationTable;
    }


    //TODO: When re-defining the base class schedulingPolicy, put it there.

    void FCFSSchedulingPolicy::setReservationTable(ResourceReservationTable *rt) {
        this->reservationTable = rt;
    }

    void FCFSSchedulingPolicy::backfill() {
        // do something here if required.
    }

    void FCFSSchedulingPolicy::jobTransitionToCompute(Job* job) {

    }

    void FCFSSchedulingPolicy::jobTransitionToOutput(Job* job) {

    }

    void FCFSSchedulingPolicy::schedule() {
        int numberOfReservationsCur = 0;
        volatile int startcount = 0;
        int i = 0;
        volatile int test = -1;

        //if (waitQueue.size() == 0)
        //        log->debug("In FCFS schedule", 1);
        // First clear all previous reservatons
        for (Job* waitjob = this->waitQueue.begin(); waitjob != NULL; waitjob = this->waitQueue.next()) {
            //            cout << "DEALLOC job from WQ" << endl;
            this->reservationTable->deallocateJob(waitjob);
        }

        // 1. Schedule Now without and with shrink
        for (Job* job = this->waitQueue.begin(); job != NULL; job = this->waitQueue.next()) {
            log->debug("Scheduling for job " + to_string(job->getJobNumber()) + " and is preceeded by " + to_string(job->getPrecedingJobNumber()), 1);
            //        cout << "Starting scheduling iteration" << endl;
            //        cout << "Length of wq " << this->waitQueue.size() << endl;
            //            for (Job* job = this->waitQueue.begin(); job != NULL; job = this->waitQueue.next()) {
            //            cout << "Trying to schedule job " << job->getJobNumber() << " and startcount = " << startcount << endl;
schedstart:
            /*
             * If we want workflow:
             * for job, find precursor job
             * find precursor job's endtime
             * call findPossibleAllocation(job, precursor_job_endtime, job->getRequestedTime())
             * 
             * 
             * 
             * 
             * 
             * todo:
             * add something like job->(getpriorjob);
             * such that priorjob->endtime is a thing
             * 
             */
            /*
             * ResourceJobAllocation* allocation = ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, globalTime, job->getRequestedTime());
             */
            ResourceJobAllocation* allocation = ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, globalTime, job->getRequestedTime());
            if (test == 0 and startcount == 0)
                assert(false);
            test = 0;
            if (!allocation->getAllocationProblem()) {
                //cout << "here\n";
                ResourceJobAllocation *best = findBestAllocation(job, allocation);
                if (best->getAllocations().size() == 0) std::cout << "Job number: " << job->getJobNumber() << std::endl;
                assert(best->getAllocations().size() > 0);
                job->setNumSimNodes(best->getAllocations().size());
                job->setAllocatedWith(FF_AND_FCF);
                this->allocateJob(job, best);
                this->log->debug("The job " + to_string(job->getJobNumber()) + " will start now @ " + ftos(best->getStartTime()), 2);
                // create start event
                this->sim->simInsertJobStartEvent(job);
                // create finish event
                this->sim->simInsertJobFinishEvent(job);
                startcount++;
                //cout << "Allocated job " << job->getJobNumber() << " and startcount = " << startcount << endl;
                //this->reservedQueue.insert(job);
            } else {
                if (this->malleableShrink == true) {
                    //We check if any other malleable job can be shrunk and given resources
                    ResourceRequirements lack = allocation->getAllocationLack();
                    if (searchResourcesInMalleableJobs(lack) == false)
                        break;
                    goto schedstart;
                } else {
                    delete allocation;
                    break;
                }
            }
        }
        for (Job *job = this->waitQueue.begin(); job != NULL; job = this->waitQueue.next()) {
            if (i == startcount)
                break;
            reservedQueue.insert(job);
            i++;
            //cout << "Adding job " << job->getJobNumber() << endl;
        }
        for (Job *job = this->reservedQueue.begin(); job != NULL; job = this->reservedQueue.next()) {
            this->waitQueue.erase(job);
        }

        // 2. Expand malleable jobs
        if (this->malleableExpand == true)
            scheduleMalleableExpand();


        std::vector<Job*> tmpJobvec;

        // 3. Backfill
        i = 0;
        if (this->numberOfReservations != -1) {
            for (Job* job = this->waitQueue.begin(); job != NULL; job = this->waitQueue.next()) {
                //cout << "Trying to backfill job " << job->getJobNumber() << endl;
                ResourceJobAllocation* allocation = ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, globalTime, job->getRequestedTime());
                if (!allocation->getAllocationProblem()) {
                    // the allocation is now
                    ResourceJobAllocation *best = findBestAllocation(job, allocation);
                    assert(best->getAllocations().size() > 0);
                    job->setAllocatedWith(FF_AND_FCF);
                    this->allocateJob(job, best);
                    this->log->debug("The job " + to_string(job->getJobNumber()) + " will start now @ " + ftos(allocation->getStartTime()), 2);
                    // create start event
                    this->sim->simInsertJobStartEvent(job);
                    // create finish event
                    this->sim->simInsertJobFinishEvent(job);
                    //cout << "job " << job->getJobNumber() << " is being backfilled" << endl;
                    //job->setJobSimssBackfilled(job);
                    this->sim->backfillcounter++;
                    tmpJobvec.push_back(job);
                } else {
                    if ((this->numberOfReservations != 0) && (numberOfReservationsCur == this->numberOfReservations)) {
                        delete allocation;
                        continue;
                    }
                    // find a later allocation and reserve it
                    delete allocation;
                    ResourceJobAllocation* allocation = findAllocation(job, globalTime, job->getRequestedTime());
                    assert(allocation->getAllocations().size() > 0);
                    assert(allocation->getStartTime() != globalTime);
                    if (!allocation->getAllocationProblem()) {
                        ResourceJobAllocation* best = findBestAllocation(job, allocation);
                        assert(best->getAllocations().size() > 0);
                        job->setAllocatedWith(FF_AND_FCF);
                        this->allocateJob(job, best);
                        this->log->debug("The job " + to_string(job->getJobNumber()) + " can start at " + ftos(allocation->getStartTime()), 2);
                        numberOfReservationsCur++;
                        //cout << "job " << job->getJobNumber() << " reserved for future" << endl;
                    } else {
                        delete allocation;
                    }
                    i++;
                }
            }
        }

        for (std::vector<Job*>::iterator it = tmpJobvec.begin(); it != tmpJobvec.end(); ++it) {
            reservedQueue.insert(*it);
            waitQueue.erase(*it);
        }

        //cout << "Size of reserved queue " << reservedQueue.size() << endl;

        //Jobs that will be started right now through FCFS are NOT in the waitQueue. They reside in reservedQueue.
        //Jobs backfilled and will start right now are residing in waitQueue.
        //Jobs that are reserved for future point of time - residing in WaitQueue.

        // Clear startNowQueue
        this->startNowQueue.clear();
    }

    void FCFSSchedulingPolicy::Reschedule() {
        for (Job* job = this->waitQueue.begin(); job != NULL; job = this->waitQueue.next()) {
            double startTime = (lastAllowedStartTime < globalTime) ? globalTime : lastAllowedStartTime; //we will try to allocate the job from now
            assert(startTime >= 0);
            ResourceJobAllocation* allocation = findAllocation(job, startTime, job->getRunTime());
            assert(allocation->getAllocations().size() > 0);
            if (!allocation->getAllocationProblem()) {
                ResourceJobAllocation *best = findBestAllocation(job, allocation);
                assert(best->getAllocations().size() > 0);
                //TODO: Consider different RS_policy_type used.
                job->setAllocatedWith(FF_AND_FCF);
                this->allocateJob(job, best);
                this->log->debug("The job " + to_string(job->getJobNumber()) + " can start at " + ftos(allocation->getStartTime()), 2);

                //we update the last allowed start time to the start time of this job
                this->lastAllowedStartTime = allocation->getStartTime();
            } else
                assert(false);
        }

    }

    void FCFSSchedulingPolicy::jobAddToWaitQueue(Job * job) {
        this->waitQueue.insert(job);
        if (waitQueue.size() == 1)
            schedule();
    }

    /**
     * Adds a given job to the wait queue, usually when it arrives to the system.
     * @see The class SchedulingPolicy
     * @param job the job to be added to the wait queue
     */

    void FCFSSchedulingPolicy::jobArrive(Job * job) {
        //first we call to the upper class that will make some arrangements in the jobs, like updated the job run time  according the jobruntime factor
        SchedulingPolicy::jobArrive(job);

        //otherwise to the queue
        this->waitQueue.insert(job);

        //cout << "Wait queues size after queueing is " << waitQueue.size() << endl;

#if 0
        double startTime = (lastAllowedStartTime < globalTime) ? globalTime : lastAllowedStartTime; //we will try to allocate the job from now
        assert(startTime >= 0);
        ResourceJobAllocation* allocation = findAllocation(job, startTime, job->getRunTime());
        printf("still executed\n");

        assert(allocation->getAllocations().size() > 0);
        /* should not be any problem */
        if (!allocation->getAllocationProblem()) {
            ResourceJobAllocation *best = findBestAllocation(job, allocation);
            assert(best->getAllocations().size() > 0);
            //TODO: Consider different RS_policy_type used.
            job->setAllocatedWith(FF_AND_FCF);
            this->allocateJob(job, best);
            this->log->debug("The job " + to_string(job->getJobNumber()) + " can start at " + ftos(allocation->getStartTime()), 2);

            //we update the last allowed start time to the start time of this job
            this->lastAllowedStartTime = allocation->getStartTime();
        } else
            assert(false);
#endif

    }

    /**
     * Find the best allocation from all the possible allocations according to the scheduling policy (FCFS in this case)
     */
    ResourceJobAllocation * FCFSSchedulingPolicy::findBestAllocation(Job *job, ResourceJobAllocation * alloc) {

        ResourceRequirements rr = {
            .cpus = (uint32_t) job->getNumberProcessors(),
            .cpusPerNode = (job->getCPUsPerNode()) == -1 ? 0 : (uint32_t) job->getCPUsPerNode(),
            .mem = (job->getRequestedMemory() == -1) ? 0 : (uint64_t) job->getRequestedMemory() * (uint32_t) job->getNumberProcessors(),
            .memPerNode = (job->getMemPerNode() == -1) ? 0 : (uint32_t) job->getMemPerNode(),
            .disk = (job->getDiskUsed() == -1) ? 0 : (uint32_t) job->getDiskUsed(),
            .diskPerNode = 0, //job->getDiskPerNode(),
            //TODO: Not used for the moment.
            .diskBW = 0.0,
            .memBW = 0.0,
            .fabBW = 0.0
        };

        assert(rr.cpusPerNode == 0);
        assert(rr.memPerNode == 0);
        assert(rr.diskPerNode == 0);
        ResourceRequirements avail = {
            .cpus = 0, .cpusPerNode = 0, .mem = 0,
            .memPerNode = 0, .disk = 0, .diskPerNode = 0,
            .diskBW = 0.0, .memBW = 0.0, .fabBW = 0.0
        };

        std::deque<ResourceBucket*> allocations = alloc->getAllocations();

        ResourceJobAllocation *best = new ResourceJobAllocation();
        best->setStartTime(alloc->getStartTime());
        best->setEndTime(alloc->getEndTime());

        std::deque<ResourceBucket*> bestBuckets;

        for (uint32_t i = 0; i < allocations.size(); i++) {

            ReservationList rl = {0};
            bool reserve = false;
            uint32_t neededcpus = rr.cpus - avail.cpus;

            if ((rr.cpusPerNode > allocations[i]->getFreeCPUs()) or
                    (rr.memPerNode > allocations[i]->getFreeMemory()) or
                    (rr.diskPerNode > allocations[i]->getFreeDisk())) continue;
            if (neededcpus > 0) {
                uint64_t memPerCore = job->getRequestedMemory();
                uint32_t maxcpus = (rr.mem == 0) ? rr.cpus : allocations[i]->getFreeMemory() / memPerCore;
                //Memory is specified by processor. Therefore, free memory could limit the number of processors to reserve.
                maxcpus = (maxcpus < allocations[i]->getFreeCPUs()) ? maxcpus : allocations[i]->getFreeCPUs();

                uint32_t reserveCPUs = (neededcpus > maxcpus) ? maxcpus : neededcpus;
                avail.cpus += reserveCPUs;
                avail.mem += reserveCPUs * memPerCore;
                rl.cpus = reserveCPUs;
                rl.mem = reserveCPUs * memPerCore;
                if (reserveCPUs > 0) reserve = true;
            }

            if (avail.disk < rr.disk) {
                uint64_t freedisk = allocations[i]->getFreeDisk();
                rl.disk = (rr.disk > freedisk) ? freedisk : rr.disk;
                avail.disk += rl.disk;

                if (rl.disk > 0) reserve = true;
            }

            if (reserve) {
                ResourceBucket *rb = new ResourceBucket(rl, allocations[i]->getStartTime(), allocations[i]->getEndTime());
                rb->setID(allocations[i]->getID());
                bestBuckets.push_back(rb);
            }

            if (not (avail < rr)) //Requirements already met. Break loop.
                break;
        }
        best->setAllocations(bestBuckets);
        delete alloc;
        return best;
    }

    /**
     * Find the best allocation from all the possible allocations according to the scheduling policy (FCFS in this case)
     */
    ResourceJobAllocation * FCFSSchedulingPolicy::findBestExtraAllocation(Job *job, ResourceJobAllocation * alloc) {
        assert(false); //TODO: Fix the PerNode option.

        ResourceRequirements rr = {
            .cpus = 1, //job->getNumberProcessors(),
            .cpusPerNode = 0, //job->getCPUsPerNode(),
            .mem = 0, //(job->getRequestedMemory() == -1)? 0: job->getRequestedMemory(),
            .memPerNode = 0, //job->getMemPerNode(),
            .disk = 0, //(job->getDiskUsed() == -1)? 0: job->getDiskUsed(),
            .diskPerNode = 0, //job->getDiskPerNode(),
            //TODO: Not used for the moment.
            .diskBW = 0.0,
            .memBW = 0.0,
            .fabBW = 0.0
        };

        ResourceRequirements avail = {
            .cpus = 0, .cpusPerNode = 0, .mem = 0,
            .memPerNode = 0, .disk = 0, .diskPerNode = 0,
            .diskBW = 0.0, .memBW = 0.0, .fabBW = 0.0
        };

        std::deque<ResourceBucket*> allocations = alloc->getAllocations();

        ResourceJobAllocation *best = new ResourceJobAllocation();
        best->setStartTime(alloc->getStartTime());
        best->setEndTime(alloc->getEndTime());

        std::deque<ResourceBucket*> bestBuckets;

        for (uint32_t i = 0; i < allocations.size(); i++) {

            ReservationList rl = {0};
            bool reserve = false;
            uint32_t neededcpus = rr.cpus - avail.cpus;
            if ((rr.cpusPerNode > allocations[i]->getFreeCPUs()) or
                    (rr.memPerNode > allocations[i]->getFreeMemory()) or
                    (rr.diskPerNode > allocations[i]->getFreeDisk())) continue;

            if (neededcpus > 0) {

                uint32_t maxcpus = (rr.mem == 0) ? rr.cpus : allocations[i]->getFreeMemory() / rr.mem;
                //Memory is specified by processor. Therefore, free memory could limit the number of processors to reserve.
                maxcpus = (maxcpus < allocations[i]->getFreeCPUs()) ? maxcpus : allocations[i]->getFreeCPUs();

                uint32_t reserveCPUs = (neededcpus > maxcpus) ? maxcpus : neededcpus;
                avail.cpus += reserveCPUs;
                avail.mem += reserveCPUs * rr.mem;
                rl.cpus = reserveCPUs;
                rl.mem = reserveCPUs * rr.mem;
                if (reserveCPUs > 0) reserve = true;
            }

            if (avail.disk < rr.disk) {
                uint64_t freedisk = allocations[i]->getFreeDisk();
                rl.disk = (rr.disk > freedisk) ? freedisk : rr.disk;
                avail.disk += rl.disk;

                if (rl.disk > 0) reserve = true;
            }

            if (reserve) {
                ResourceBucket *rb = new ResourceBucket(rl, allocations[i]->getStartTime(), allocations[i]->getEndTime());
                rb->setID(allocations[i]->getID());
                bestBuckets.push_back(rb);
            }

            if (not (avail < rr)) //Requirements already met. Break loop.
                break;
        }
        best->setAllocations(bestBuckets);
        delete alloc;
        return best;
    }

    /**
     * Moves a job from teh wait queue to the reservation table, using an specified set of resources
     * @see The class SchedulingPolicy
     * @param job The job that has started
     */

    void FCFSSchedulingPolicy::jobFinish(Job * job) {

        log->debug("Finishing Job " + to_string(job->getJobNumber()) + " at globalTime: " + to_string((unsigned int) globalTime), 1);

        //We call to the base class - important it has to update some info
        SchedulingPolicy::jobFinish(job);

        ResourceJobAllocation* allocation = (ResourceJobAllocation*) ((ResourceReservationTable*)this->reservationTable)->getJobAllocation(job);
        deque<ResourceBucket*> allocs = allocation->getAllocations();

        deque<ResourceBucket*> forParaver;

        /*carrying out the deallocation to the processors
          it must be done externally to the deallocate policy due to this
          function may be called while the jobs is still not running !
         */

        //TODO: Is there something to do here? There's no CPUMapping anymore.
        for (deque< ResourceBucket * >::iterator iter = allocs.begin();
                iter != allocs.end(); ++iter) {
            ResourceBucket* bucket = *iter;

            /* for sanity  we check that the cpu is not used by any job ..*/
            //assert(((ResourceReservationTable*)this->reservationTable)->CPUMapping[bucket->getCpu()] != 0);

            //((ResourceReservationTable*)this->reservationTable)->CPUMapping[bucket->getCpu()] = 0;

            /*if the paraver trace is not null we notify for such deallocation */
            if (this->outputTrace)
                forParaver.push_back((ResourceBucket*) bucket);

        }

        if (this->outputTrace) {
            //            cout << "USE PARAVER FCFS jobRemovefromRT" << endl;
            this->opTrace->jobEnd(job->getJobNumber(), forParaver, false);

        }

        /* we free the corresponding buckets*/
        //        cout << "Removing from RT after fininsh job" << endl;
        assert(this->reservationTable->deallocateJob(job));

        /* the job has finished so we just set the status */
        job->setJobSimStatus(COMPLETED);

        runningQueue.erase(job);

        //lastAllowedStartTime = globalTime;
        this->sim->simInsertScheduleEvent();

        //        cout << "job " << job->getJobNumber() << " finishing at globalTime: " << (unsigned int) globalTime << endl;
    }

    /**
     * Allocates the job to the provided allocation
     * @param job The job to allocated
     * @param allocation The allocation for the job
     */
    void FCFSSchedulingPolicy::allocateJob(Job* job, ResourceJobAllocation * allocation) {
        double runtime = job->getRunTime();

        /*We create an start time and end time according the allocation.*/
        ((ResourceReservationTable*)this->reservationTable)->allocateJob(job, allocation);
        job->setJobSimStartTime(allocation->getStartTime());
        job->setJobSimFinishTime(allocation->getStartTime() + runtime);
        job->setJobSimEstimateFinishTime(allocation->getStartTime() + runtime);

    }

    void FCFSSchedulingPolicy::expandJobAllocation(Job* job, ResourceJobAllocation * allocation) {
        ((ResourceReservationTable*)this->reservationTable)->expandJobAllocation(job, allocation);
        //delete allocation;
        job->setNumberProcessors(job->getNumberProcessors() + 1);
        //cout << "Expanding Job " << job->getJobNumber() << " at globalTime: " << globalTime << endl;
        double newfinishtime = ((job->getNumberProcessors() - 1) * (job->getJobSimFinishTime() - globalTime)) / job->getNumberProcessors();
        job->setJobSimFinishTime(globalTime + newfinishtime);
        //cout << "Expanding Job " << job->getJobNumber() << " at globalTime: " << (unsigned int)globalTime << endl;
    }

    bool FCFSSchedulingPolicy::shrinkJobAllocation(Job *job, int cpus) {
        //cout << "Shrinking Job " << job->getJobNumber() << " at globalTime: " << globalTime << endl;
        int oldprocs = job->getNumberProcessors();
        //if (job->getJobNumber() == 1174)
        //				cout << "oldprocs = " << oldprocs << " and cpus taken away  = " << cpus << endl;
        job->setNumberProcessors(oldprocs - cpus);
        //if (job->getJobNumber() == 1174)
        //				cout << "new procs = " << job->getNumberProcessors() << endl;
        assert(((ResourceReservationTable*)this->reservationTable)->shrinkJobAllocation(job, cpus));
        double newfinishtime = (oldprocs * (job->getJobSimFinishTime() - globalTime)) / job->getNumberProcessors();
        job->setJobSimFinishTime(globalTime + newfinishtime);
        //cout << "Shrinking Job " << job->getJobNumber() << " at globalTime: " << globalTime << endl;
        return true;
    }

    /**
     * Chooses the more appropiate job to run .
     * @see The class SchedulingPolicy
     * @return The appropiate job to run
     */
    Job * FCFSSchedulingPolicy::jobChoose() {
        //in this policy should'nt be called
        assert(false);
    }

    /**
     * Function that is called once the job should start
     * @see The class SchedulingPolicy
     * @param job The job that to start
     * This is essentially a hook place to do tracing.
     */
    void FCFSSchedulingPolicy::jobStart(Job * job) {


        /* the job should start now !
       first we find the allocation that is associated to the job and move it to the queue
         */
        this->realFiniQueue.insert(job);
        this->runningQueue.insert(job);
        //cout << "Starting job " << job->getJobNumber() << " and finiqueue size = " << this->realFiniQueue.size() << endl;
        /* as this is a FCFS policy the job is allocated definitively in the queue, so we can delete it from the wait queue */
        this->waitQueue.erase(job);
        this->reservedQueue.erase(job);
        //cout << "Wait queue size after starting job " << job->getJobNumber() << " is " << this->waitQueue.size() << endl;


        ResourceJobAllocation* allocation = (ResourceJobAllocation*) ((ResourceReservationTable*)this->reservationTable)->getJobAllocation(job);
        deque<ResourceBucket*> allocs = allocation->getAllocations();
        //        deque<ResourceBucket*> forParaver;

        //        if (this->useParaver) {
        ////            cout << "CALLED " << allocs.size() << endl;
        //            log->debug("Pushing " + to_string(allocs.size()) + " buckets to Paraver handler.", 4);
        //
        //            for (deque< ResourceBucket * >::iterator iter = allocs.begin(); iter != allocs.end(); ++iter) {
        //                ResourceBucket* bucket = *iter;
        //                forParaver.push_back((ResourceBucket*) bucket);
        //            }
        //        }

        //        this->prvTrace->ParaverJobRuns(job->getJobNumber(), forParaver);
        //        if (this->useParaver) {
        //            log->debug("Pushing " + to_string(allocs.size()) + " buckets to Paraver handler.", 4);
        //            this->prvTrace->ParaverJobRuns(job->getJobNumber(), allocs);
        //        }
        if (this->outputTrace) {
            log->debug("Pushing " + to_string(allocs.size()) + " buckets to output Trace handler.", 4);
            this->opTrace->jobStart(job->getJobNumber(), allocs, false);
        }
        //cout << "Starting Job " << job->getJobNumber() << " at globalTime: " << (unsigned int)globalTime << endl;
    }

    /**
     * Returns the number of jobs currently allocated in the waitQueue
     * @return
     */

    double FCFSSchedulingPolicy::getJobsIntheWQ() {
        return this->waitQueue.getJobs();

    }

    /**
     * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
     * @return A integer containing the pending work
     */
    double FCFSSchedulingPolicy::getLeftWork() {
        //TODO: See if there's anything to change with the new resources.
        double pending_work = 0;

        for (Job* waitjob = this->waitQueue.begin();
                waitjob != NULL; waitjob = this->waitQueue.next()) {
            pending_work += waitjob->getRequestedTime() * waitjob->getNumberProcessors();
        }

        return pending_work;
    }

    /**
     * Return the number of jobs in the run queue
     * @return A integer containing the number of running jobs
     */
    double FCFSSchedulingPolicy::getJobsIntheRQ() {
        return this->realFiniQueue.getJobs();
    }

    /**
     * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now
     * @param MetricTpye The metric that indicates which performance variable has to be computed
     * @param job The job that will be checked
     * @return The metric containing the performance that the given job would experiment if it would be submitted right now
     */

    Metric * FCFSSchedulingPolicy::EstimatePerformanceResponse(metric_t MetricType, Job * job) {
        //TODO: Understand what this is doing.
        //we estimate when the job would start
        //ALSO USED IN THE EASY SCHEDULING .. IN CACSE OF MODIFICATION PLEASE UPDTATE THE easy STUFF
        assert(false);
        ResourceJobAllocation* allocation = findAllocation(job, this->lastAllowedStartTime, ceil(job->getRunTime() * this->EmulateCPUFactor));
        double wouldStart = allocation->getStartTime();
        double waitTime = wouldStart - globalTime;
        double RT = job->getRequestedTime();
        Metric* metric = new Metric();

        //since the allocation won't be used any more we delete it
        delete allocation;

        log->debug("The job " + to_string(job->getJobNumber()) + " would start at " + ftos(wouldStart) + " and arrives at " + ftos(globalTime) + " in the FCFS", 2);

        switch (MetricType) {
            case SLD:
            {
                metric->setNativeDouble(RT / (RT + waitTime));
                break;
            }
            case BSLD:
            {
                metric->setNativeDouble(RT / (max(RT, (double) BSLD_THRESHOLD) + waitTime));
            }
            case WAITTIME:
            {
                metric->setNativeDouble(waitTime);
                break;
            }
            case JOBS_IN_WQUEUE:
            {
                metric->setNativeDouble(this->getJobsIntheWQ());
                break;
            }
            case SUBMITED_JOBS:
            {
                metric->setNativeDouble(this->submittedJobs);
                break;
            }
            case LEAST_WORK_LEFT:
            {
                double pending_work = 0;

                for (Job* waitjob = this->waitQueue.begin();
                        waitjob != NULL; waitjob = this->waitQueue.next()) {
                    pending_work += waitjob->getRequestedTime() * waitjob->getNumberProcessors();
                }

                metric->setNativeDouble(pending_work);

                break;
            }
            default:
                assert(false);
        }

        metric->setType(MetricType);

        return metric;
    }

    void FCFSSchedulingPolicy::setGlobalTime(uint64_t globalTime) {
        SchedulingPolicy::setGlobalTime(globalTime);

        if (this->lastAllowedStartTime < globalTime)
            this->lastAllowedStartTime = globalTime;

    }

    void FCFSSchedulingPolicy::scheduleMalleableExpand() {
        int allocated = 0;
        FCFSJobQueue runningMalQueue;

        // 1. Queue up all malleable jobs
        for (Job* job = runningQueue.begin(); job != NULL; job = runningQueue.next()) {
            if ((job->getMalleable() == true) && (job->getNumberProcessors() < job->getMaxProcessors())) {
                runningMalQueue.insert(job);
            }

        }
        for (Job* job = reservedQueue.begin(); job != NULL; job = reservedQueue.next()) {
            if (job->getMalleable() == true) {
                runningMalQueue.insert(job);
            }

        }

        // 2. Creata a map
        unordered_map < Job*, bool> malyesno;
        for (Job* job = runningMalQueue.begin(); job != NULL; job = runningMalQueue.next()) {
            malyesno.insert(make_pair(job, true));
        }

        // 3. Search for resources for these malleable jobs one by one
malfind:
        allocated = 0;
        for (Job *job = runningMalQueue.begin(); job != NULL; job = runningMalQueue.next()) {
            //std::unordered_map<Job*,bool>::const_iterator malyesno_iterator = malyesno.find(job);
            //if ((malyesno_iterator == malyesno.end()) || (malyesno_iterator->second == false))
            //	continue;
            if (malyesno[job] == false)
                continue;

            ResourceJobAllocation* allocation = ((ResourceReservationTable*)this->reservationTable)->findPossibleExtraAllocation(job, globalTime, job->getJobSimStartTime() + job->getRequestedTime() - globalTime);
            if (allocation->getAllocationProblem() == true) {
                malyesno[job] = false;
                delete allocation;
                continue;
            } else {
                ResourceJobAllocation *best = findBestExtraAllocation(job, allocation);
                assert(best->getAllocations().size() > 0);
                this->expandJobAllocation(job, best);
                this->sim->simModifyJobFinishEvent(job);
                allocated++;
                if (job->getNumberProcessors() == job->getMaxProcessors())
                    malyesno[job] = false;
            }
        }
        if (allocated > 0)
            goto malfind;

    }

    size_t FCFSSchedulingPolicy::getWQSize() {
        if (waitQueue.size() > 0)
            std::cout << "Top job: " << waitQueue.headJob()->getJobNumber() << std::endl;
        return this->waitQueue.size();
    }

    bool FCFSSchedulingPolicy::searchResourcesInMalleableJobs(ResourceRequirements lack) {
        // Search for the lack in malleable jobs
        FCFSJobQueue runningMalQueue;

        std::map<Job*, int> shrinkmap;

        // 1. Queue up all malleable jobs
        for (Job* job = runningQueue.begin(); job != NULL; job = runningQueue.next()) {
            if ((job->getMalleable() == true) && (job->getNumberProcessors() > job->getMinProcessors()))
                runningMalQueue.insert(job);
        }

        // 2. Now we have all the running malleable jobs that have higher number of processors in a queue. Now match the lack.
        int rcpus = lack.cpus;
        for (Job *job = runningMalQueue.begin(); job != NULL; job = runningMalQueue.next()) {
            int availcpus = job->getNumberProcessors() - job->getMinProcessors();
            if (rcpus > availcpus) {
                shrinkmap[job] = availcpus;
                rcpus -= availcpus;
            } else if (rcpus <= availcpus) {
                shrinkmap[job] = rcpus;
                rcpus = 0;
            }
            if (rcpus == 0)
                break;
        }

        if (rcpus != 0) {
            return false;
        }

        // Found the required number of cpus. Therefore execute shrink.
        for (std::map<Job*, int>::iterator it = shrinkmap.begin(); it != shrinkmap.end(); ++it) {
            assert(shrinkJobAllocation(it->first, it->second));
        }

        return true;
    }

    ResourceJobAllocation* FCFSSchedulingPolicy::findAllocation(Job* job, double startTime, double runtime) {

        //ALSO USED IN THE FCFS (AND WITH EASY COLLISION POLICY) SCHEDULING .. IN CASE OF MODIFICATION PLEASE UPDTATE THE fcfs STUFF

        //while there are problems of finding resources in a given startTime, we will iterate and incrementing the startime for the search.

        bool allocated = false;

        while (!allocated) {
            ResourceJobAllocation* possibleAllocation =
                    ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, startTime, runtime);

            //TODO: This has been oversimplified. Check how to go back to add different policies and act accordingly.
            if (possibleAllocation->getAllocationProblem()) {
                startTime = possibleAllocation->getNextStartTimeToExplore();
                delete possibleAllocation;
                continue;
            } else {
                /* If there was no problem, we found a superset with feasible allocations.
                 ** We now need to find an actual allocation according to the scheduling policy. */
                bool useFirstFit = this->getRS_policyUsed() == FIRST_FIT or this->getRS_policyUsed() == FF_AND_FCF;
                if (not useFirstFit) {
                    std::cout << "RS policy Used: " << this->getRS_policyUsed() << std::endl;
                }
                assert(useFirstFit);

                allocated = true;
                return possibleAllocation;
            }
        }

        return NULL;

    }


}
