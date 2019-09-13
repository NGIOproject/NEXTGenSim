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
#include <scheduling/slurmschedulingpolicy.h>
#include <scheduling/resourcejoballocation.h>
#include <scheduling/resourcereservationtable.h>
#include <scheduling/simulatorcharacteristics.h>

#include <scheduling/simulation.h>
#include <utils/simulationconfiguration.h>

#include <cmath>
#include <unordered_map>
#include <utility>
#include <algorithm>

#include "systemorchestrator.h"

namespace Simulator {

    /*
     * The default constructor for the class
     */

    SLURMSchedulingPolicy::SLURMSchedulingPolicy() {
    }

    /*
     * This is the constructor that should be used, invoques that constructor for the parent classes
     * @param log A reference to the logging engine
     * @param globalTime The startTime of the simulation
     */
    SLURMSchedulingPolicy::SLURMSchedulingPolicy(Utils::ArchitectureConfiguration *config, Log* log, uint64_t globalTime, bool simulateResourceUsage, int numberOfReservations, bool malleableExpand, bool malleableShrink) : SchedulingPolicy() {
        this->architecture = config;
        this->log = log;
        this->globalTime = globalTime;
        this->simulateResourceUsage = simulateResourceUsage;
        this->lastAllowedStartTime = 0;
        this->schedulingPolicySimulated = SLURM;
        this->numberOfReservations = numberOfReservations;
        this->malleableExpand = malleableExpand;
        this->malleableShrink = malleableShrink;
        this->waitingQueue.setLog(log);
        this->runningQueue.setLog(log);
        //        this->startNowQueue.setLog(log);
        this->scheduledQueue.setLog(log);
        this->lastscheduletime = globalTime;
        this->bf_counter = 0;
        if (this->architecture->getSystemOrchestrator()->getsimuConfig()->SLURMPmem != 0) {
            this->persist = AVAILABLE;
        } else {
            this->persist = NOT_AVAILABLE;
        }

    }

    /*
     * The default destructor for the class
     */

    SLURMSchedulingPolicy::~SLURMSchedulingPolicy() {
        /* free the  reservationTable */
        delete this->reservationTable;
    }

    void SLURMSchedulingPolicy::backfill() {

        log->debug("BACKFILL Started", 1);
        if (this->skip_backfill) {
            log->debug("BACKFILL Skipped", 1);
            return;
        }
        this->skip_backfill = true;

        

        /*
         * Back-filling algorithm.
         * Try to place jobs from the waitQueue. Probably needs a backfilling cut-off too.
         */

        // Get the size of the wait queue to parse on each run of schedule() from the simulation configuration.
        int WQlimit = this->architecture->getSystemOrchestrator()->getsimuConfig()->SLURMWQsize;

        // Track how many reservations we allocate (in this pass).
        int numberOfReservationsCur = 0; // Counts the number of jobs assigned with the backfill algorithm.

        // Try to skip past jobs already dealt with in the PRIORITY Q.
        Job* job = this->waitingQueue.begin();

        for (int i = 0; i < WQlimit && job != NULL; i++) {
            job = waitingQueue.next();
        }

        for (Job* wlim = job; wlim != NULL; wlim = this->waitingQueue.next()) {
            this->reservationTable->deallocateJob(wlim);
        }


        // Try to skip past jobs already dealt with in the PRIORITY Q.
        job = this->waitingQueue.begin();

        for (int i = 0; i < WQlimit && job != NULL; i++) {
            job = waitingQueue.next();
        }

        for (; job != NULL; job = this->waitingQueue.next()) {
            log->debug("BACKFILL Scheduling for job " + to_string(job->getJobNumber()) + " and is preceeded by " + to_string(job->getPrecedingJobNumber()), 1);
            log->debug("BACKFILL Scheduling and currently have " + to_string(numberOfReservationsCur) + " reservations.", 2);
            ResourceJobAllocation* allocation = ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, globalTime, job->getRequestedTime());
            if (!allocation->getAllocationProblem()) {
                // The allocations that can run now.
                ResourceJobAllocation *best = findBestAllocation(job, allocation);
                assert(best->getAllocations().size() > 0);
                this->scheduleJob(job, best);
                this->sim->backfillcounter++;

            } else {
                if ((this->numberOfReservations != -1) && (numberOfReservationsCur == this->numberOfReservations)) {
                    log->debug("BACKFILL Hit the max number of reservations.", 2);
                    delete allocation;
                    continue;
                }
                //                 find a later allocation and reserve it
                log->debug("Failed to find possible, trying findAllocation.", 2);
                delete allocation;
                ResourceJobAllocation* allocation = findFutureAllocation(job, globalTime, job->getRequestedTime());
                assert(allocation->getStartTime() != globalTime);
                this->log->debug("The job " + to_string(job->getJobNumber()) + " will start later.", 2);
                if (!allocation->getAllocationProblem()) {
                    ResourceJobAllocation* best = findBestAllocation(job, allocation);
                    assert(best->getAllocations().size() > 0);
                    this->scheduleJob(job, best);
                    this->log->debug("The job " + to_string(job->getJobNumber()) + " can start @ " + ftos(allocation->getStartTime()), 2);
                    numberOfReservationsCur++;
                    log->debug("BACKFILL scheduled job " + to_string(job->getJobNumber()) + "and resCur is " + to_string(numberOfReservationsCur), 2);
                } else {
                    delete allocation;
                }

            }
        }

        // For jobs that have been dealt with by the PRIORITY queue and allocated, remove them from the waitQueue.
        for (Job *job = this->scheduledQueue.begin(); job != NULL; job = this->scheduledQueue.next()) {
            this->waitingQueue.erase(job);
        }
        
        this->lastscheduletime = globalTime;
        log->debug("BACKFILL Finished", 1);


    }

    /*
     * The function that does the scheduling.
     * Note that this function cares *only* about the logic of starting and not-starting jobs.
     * It doesn't deal with placement onto the system, that's what the RRT functions do.
     * In standard nomenclature, this is scheduling, RRT is placement.
     */
    void SLURMSchedulingPolicy::schedule() {
        log->debug("Running SLURM Schedule.", 2);
        log->debug("Current number of reservations: " + to_string(numberOfReservations), 4);
        log->debug("Last Schedule Time: " + std::to_string(this->lastscheduletime), 4);
        log->debug("SLURM Skip Time: " + std::to_string(this->architecture->getSystemOrchestrator()->getsimuConfig()->SLURMSkipTime), 4);


        // If there was a scheduling event within "SLURMSKipTime" seconds, don't run. This mimics SLURM and stops over-scheduling which eats time in real systems.
        if ((this->lastscheduletime > 0) && ((this->globalTime - this->lastscheduletime) < this->architecture->getSystemOrchestrator()->getsimuConfig()->SLURMSkipTime)) {
            log->debug("Skipping Schedule due to short time period since last run.", 2);
            return;
        }
        /*
         * Reminder of which queues we have in play...
         *        SLURMJobQueue waitQueue;      The wait queue job list
         *        SLURMJobQueue runningQueue;   The running queue ordered by ESF
         *        SLURMJobQueue startNowQueue;  Jobs that will start now
         *        SLURMJobQueue reservedQueue;  Jobs that are reserved to start now
         */

        // First clear all previous reservations that have been allocated.
        // This should be a no-op as jobs in the waitingQueue should never have allocations.
        for (Job* waitjob = this->waitingQueue.begin(); waitjob != NULL; waitjob = this->waitingQueue.next()) {
            log->debug("Deallocating Job " + to_string(waitjob->getJobNumber()), 2);
            this->reservationTable->deallocateJob(waitjob);
        }


        // Get the size of the wait queue to parse on each run of schedule() from the simulation configuration.
        int WQlimit = this->architecture->getSystemOrchestrator()->getsimuConfig()->SLURMWQsize;

        if (log->getLevel() >= 8) {
            waitingQueue.dump();
            scheduledQueue.dump();
        }

        // Really, I want to delete the allocation but hold onto it, call findFutureAllocation, see if the start time is earlier.
        // If so, go with the new and forget the old. If not, stick with the old.
        log->debug("Moving all jobs from scheduleQ to waitingQ for re-scheduling.", 2);

        // If we do this, really, there is no need for a separate scheduledQ and waitingQ
        // It merely serves to remind that it's possible to do it for, say, some guaranteed scheduling
        for (Job* job = this->scheduledQueue.begin(); job != NULL; job = this->scheduledQueue.next()) {
            this->sim->deleteJobEvents(job);
            this->reservationTable->deallocateJob(job);
            this->scheduledQueue.erase(job);
            this->waitingQueue.insert(job);
        }

        int i = 0;
        for (Job* job = this->waitingQueue.begin(); job != NULL && i < WQlimit; job = this->waitingQueue.next(), ++i) {
            log->debug("PRIORITY Scheduling for job " + to_string(job->getJobNumber()) + " which is preceeded by " + vtos(job->getPreceedingJobs()), 1);


            /*
             * This keeps jobs in a workflow from being scheduled until the preceeding job is finished.
             */
            vector<int> _prej = job->getPreceedingJobs();
            int _prej_count = _prej.size();
            if (_prej.empty() == false) {

                // Look through the finished queue, this is very slow, but currently necessary
                for (Job* finjob = this->finishedQueue.begin(); finjob != NULL; finjob = this->finishedQueue.next()) {

                    if ((std::find(_prej.begin(), _prej.end(), finjob->getJobNumber()) != _prej.end()) == true) {
                        if (finjob->getJobSimStatus() == COMPLETED) {
                            log->debug("Job " + to_string(job->getJobNumber()) + " has preceeding jobs [" + vtos(job->getPreceedingJobs()) + "]. Job " + itos(finjob->getJobNumber()) + " is COMPLETED.", 2);
                            _prej_count--; // Found a completed, preceeding job, decr counter
                        } else {
                            log->debug("Job " + to_string(job->getJobNumber()) + " has preceeding jobs [" + vtos(job->getPreceedingJobs()) + "]. Job " + itos(finjob->getJobNumber()) + " is NOT COMPLETED.", 2);
                        }


                    }
                }

                // If we have examined all jobs in the finished queue, and all preceeding jobs and determined that
                // the current job has some unfinished preceeding jobs, skip to the next job in the WAITING QUEUE
                if (_prej_count != 0) {
                    log->debug("Job " + to_string(job->getJobNumber()) + " has preceeding jobs [" + vtos(job->getPreceedingJobs()) + "]. Some are NOT COMPLETED, moving to next job in WQ.", 2);
                    continue;
                }


            } else {
                log->debug("Job " + to_string(job->getJobNumber()) + " has no preceeding jobs.", 2);
            }

            if (this->persist == AVAILABLE){
                // Shorten jobs if the workflow status is '2' which means 'CONTINUE'
                if (job->getworkflowStatus() == 2){
                    double _jobrt = job->getRequestedTime();
                    double _jobiorat = job->getIOCompRatio();
                    _jobiorat = 1.0 - _jobiorat;
                    _jobrt = _jobiorat * _jobrt;
                    job->setIOCompRatio(0.0);
                    log->debug("Job " + to_string(job->getJobNumber()) + " used to have runtime of " + to_string(job->getRequestedTime()) + " now shortened to " + to_string(_jobrt), 2);
                    job->setRequestedTime(_jobrt);
                    job->setRunTime(_jobrt);
                }
            }

            ResourceJobAllocation* possible_nodes = findFutureAllocation(job, globalTime, job->getRequestedTime());
            log->debug("Tried to find FUTURE for " + to_string(job->getJobNumber()) + " and result was " + to_string(possible_nodes->getallocationReason()), 2);
            if (!possible_nodes->getAllocationProblem()) {
                ResourceJobAllocation *best_nodes = findBestAllocation(job, possible_nodes); // This finds a future reservation and essentially just picks something from the array of possibles
                assert(best_nodes->getAllocations().size() > 0); // Make sure the allocation for this job is >0 nodes.
                log->debug("Job " + to_string(job->getJobNumber()) + " has best nodes of size " + to_string(best_nodes->getAllocations().size()), 2);
                this->scheduleJob(job, best_nodes); // Place the job in the RRT.
            } else {
                log->debug("Cannot allocate FUTURE job " + to_string(job->getJobNumber()) + " due to " + to_string(possible_nodes->getallocationReason()), 2);
                delete possible_nodes;
            }

            /* For reference...
            enum allocation_problem_t {
            NOT_ENOUGH_RESOURCES = 0,
            NOT_ENOUGH_CPUS,
            JOBS_COLLISION,
            NOPROBLEM,
            };
             */
        }

        // For jobs that have been dealt with by the PRIORITY queue and allocated, remove them from the waitQueue.
        for (Job *job = this->scheduledQueue.begin(); job != NULL; job = this->scheduledQueue.next()) {
            log->debug("Updating priority for job: " + to_string(job->getJobNumber()) + " to be: " + to_string(job->getslurmprio() + 1), 2);
            job->setslurmprio((uint32_t) (job->getslurmprio() + 1));
            this->waitingQueue.erase(job);
        }


        for (Job* job = this->waitingQueue.begin(); job != NULL; job = this->waitingQueue.next()) {
            log->debug("Updating priority for job: " + to_string(job->getJobNumber()) + " to be: " + to_string(job->getslurmprio() + 1), 2);
            job->setslurmprio((uint32_t) (job->getslurmprio() + 1));
        }


        this->lastscheduletime = globalTime;
        log->debug("Scheduling Finished", 1);

        // Backfilling needs to happen somewhere here.
        if ((this->bf_counter > 0) && (this->bf_counter % 10 == 0)) {
            //backfill();
        }
        this->bf_counter++;
        this->skip_backfill = false;
    }


    /**
     * Adds a given job to the wait queue, usually when it arrives to the system.
     * @see The class SchedulingPolicy
     * @param job the job to be added to the wait queue
     */
    // This is where jobs are added after first going to the partition.

    void SLURMSchedulingPolicy::jobArrive(Job * job) {
        //first we call to the upper class that will make some arrangements in the jobs, like updated the job run time  according the jobruntime factor
        SchedulingPolicy::jobArrive(job);
        //otherwise to the queue
        this->waitingQueue.insert(job);
    }

    /*
     * Find the best allocation from all the possible allocations according to the scheduling policy (FCFS in this case)
     * By allocations, we mean nodes, so we find the "best" set of nodes on which to run <job> from a big list of possibles in <alloc>
     */
    ResourceJobAllocation * SLURMSchedulingPolicy::findBestAllocation(Job *job, ResourceJobAllocation * alloc) {

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
     * Moves a job from the wait queue to the reservation table, using an specified set of resources
     * @see The class SchedulingPolicy
     * @param job The job that has started
     */
    void SLURMSchedulingPolicy::jobFinish(Job * job) {

        log->debug("Finishing Job " + to_string(job->getJobNumber()) + " at globalTime: " + to_string((unsigned int) globalTime), 1);

        //We call to the base class - important it has to update some info
        SchedulingPolicy::jobFinish(job);

        ResourceJobAllocation* allocation = (ResourceJobAllocation*) ((ResourceReservationTable*)this->reservationTable)->getJobAllocation(job);
        deque<ResourceBucket*> allocs = allocation->getAllocations();

        //        deque<ResourceBucket*> forOutputTrace;

        /*
         *  Send the relevant buckets to the output trace function for plotting etc.
         */
        if (this->outputTrace) {

            //            for (deque< ResourceBucket * >::iterator iter = allocs.begin(); iter != allocs.end(); ++iter) {
            //                forOutputTrace.push_back((ResourceBucket*) *iter);
            //            }

            this->opTrace->jobEnd(job->getJobNumber(), allocs, false);

        }


        /* we free the corresponding buckets*/
        assert(this->reservationTable->deallocateJob(job));

        /* the job has finished so we just set the status */
        job->setJobSimStatus(COMPLETED);
        runningQueue.erase(job);
        finishedQueue.insert(job);

        //lastAllowedStartTime = globalTime;
        this->sim->simInsertScheduleEvent();
        this->skip_backfill = false;


    }

    /*
     * This function schedules the job, ie:
     * it places the job in the reservationtable
     * it inserts the relevant start and finish events
     * it updates the job's own information on start, finish etc.
     */
    void SLURMSchedulingPolicy::scheduleJob(Job* job, ResourceJobAllocation * allocation) {
        double runtime = job->getRunTime();

        /*We create an start time and end time according the allocation.*/
        ((ResourceReservationTable*)this->reservationTable)->allocateJob(job, allocation);
        job->setJobSimStartTime(allocation->getStartTime());
        job->setJobSimFinishTime(allocation->getStartTime() + runtime);
        job->setJobSimEstimateFinishTime(allocation->getStartTime() + runtime);
        job->setNumSimNodes(allocation->getAllocations().size());
        job->setAllocatedWith(FF_AND_FCF); // Need to check the options here.



        this->log->debug("Job " + to_string(job->getJobNumber()) + " scheduled to start @ " + ftos(allocation->getStartTime()), 2);
        this->log->debug("Job " + to_string(job->getJobNumber()) + " runtime = " + to_string(job->getRunTime()) + " requested time = " + to_string(job->getRequestedTime()), 8);
        // Insert JOB_START event
        this->sim->simInsertJobStartEvent(job);
        // Insert JOB_FINSH event
        this->sim->simInsertJobFinishEvent(job);

        if (this->persist == AVAILABLE && job->getworkflowStatus() == 2) {
            log->debug("Job " + to_string(job->getJobNumber()) + " skipping IO events due to workflowyness.");
        } else {
            log->debug("Job " + to_string(job->getJobNumber()) + " adding IO events.");
            this->sim->simInsertComputeBeginEvent(job);
            this->sim->simInsertComputeEndEvent(job);
        }

        // Do not remove from waitingQueue here, as scheduling loop still running and it could cause a race.
        this->scheduledQueue.insert(job);


    }

    /**
     * Chooses the more appropiate job to run .
     * @see The class SchedulingPolicy
     * @return The appropiate job to run
     */
    Job * SLURMSchedulingPolicy::jobChoose() {
        //in this policy should'nt be called
        assert(false);
    }

    void SLURMSchedulingPolicy::jobStart(Job * job) {
        /*
         * Move the job from the scheduledQueue to the runningQueue.
         * Do tracing if required.
         * Re-enable backfill as we've done some work.
         */
        this->runningQueue.insert(job);
        this->scheduledQueue.erase(job);
        ResourceJobAllocation* allocation = ((ResourceReservationTable*)this->reservationTable)->getJobAllocation(job);
        deque<ResourceBucket*> allocs = allocation->getAllocations();
        if (this->outputTrace) {
            log->debug("Pushing " + to_string(allocs.size()) + " buckets to output Trace handler.", 4);
            if (this->persist == AVAILABLE && job->getworkflowStatus() == 2){
                this->opTrace->jobStart(job->getJobNumber(), allocs, true);
            } else {
                this->opTrace->jobStart(job->getJobNumber(), allocs, false);    
            }
        }
        this->skip_backfill = false;
        std::vector<uint32_t> nnodes;
        ResourceBucket* current;
        for (deque<ResourceBucket*>::iterator alloc_it = allocs.begin(); alloc_it != allocs.end(); ++alloc_it) {
            current = *alloc_it;
            uint32_t bId = current->getID();
            nnodes.push_back(bId);
        }
        this->jobnodes.insert(std::pair<uint32_t, std::vector < uint32_t >> ((uint32_t) job->getJobNumber(), nnodes));

    }

    /**
     * Returns the number of jobs currently allocated in the waitQueue
     * @return
     */
    double SLURMSchedulingPolicy::getJobsIntheWQ() {
        return this->waitingQueue.getJobs();
    }

    /**
     * Return the the amount of pending work (area of requested time x requested processors for the queued jobs)
     * @return A integer containing the pending work
     */
    double SLURMSchedulingPolicy::getLeftWork() {
        //TODO: See if there's anything to change with the new resources.
        double pending_work = 0;

        for (Job* waitjob = this->waitingQueue.begin();
                waitjob != NULL; waitjob = this->waitingQueue.next()) {
            pending_work += waitjob->getRequestedTime() * waitjob->getNumberProcessors();
        }

        return pending_work;
    }

    /**
     * Return the number of jobs in the run queue
     * @return A integer containing the number of running jobs
     */
    double SLURMSchedulingPolicy::getJobsIntheRQ() {
        return this->runningQueue.getJobs();
    }

    void SLURMSchedulingPolicy::setGlobalTime(uint64_t globalTime) {
        SchedulingPolicy::setGlobalTime(globalTime);

        if (this->lastAllowedStartTime < globalTime)
            this->lastAllowedStartTime = globalTime;

    }

    size_t SLURMSchedulingPolicy::getWQSize() {
        if (waitingQueue.size() > 0)
            std::cout << "Top job: " << waitingQueue.headJob()->getJobNumber() << std::endl;
        return this->waitingQueue.size();
    }

    ResourceJobAllocation* SLURMSchedulingPolicy::findFutureAllocation(Job* job, double startTime, double runtime) {
        bool allocated = false;

        while (!allocated) {
            
            vector<uint32_t> _nodes;
            if (this->persist == AVAILABLE){
                _nodes = getPreceedingNodes(job);
            }
            ResourceJobAllocation* possibleAllocation = ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, startTime, runtime, _nodes);

            //TODO: This has been oversimplified. Check how to go back to add different policies and act accordingly.
            if (possibleAllocation->getAllocationProblem()) {
                startTime = possibleAllocation->getNextStartTimeToExplore();
                delete possibleAllocation;

                continue;
                //            } else if (possibleAllocation->getAllocationProblem() && tripcount >= 3) {
                //                // We tried too hard, just give up.
                //                return possibleAllocation;
            } else {
                /* If there was no problem, we found a superset with feasible allocations.
                 ** We now need to find an actual allocation according to the scheduling policy. */
                bool useFirstFit = this->getRS_policyUsed() == FIRST_FIT or this->getRS_policyUsed() == FF_AND_FCF;
                //                if (not useFirstFit) {
                //                    std::cout << "RS policy Used: " << this->getRS_policyUsed() << std::endl;
                //                }
                assert(useFirstFit);

                allocated = true;
                return possibleAllocation;
            }

        }

        return NULL;

    }

    vector<uint32_t> SLURMSchedulingPolicy::getPreceedingNodes(Job* job) {
        vector<int> _pj = job->getPreceedingJobs();
        vector<uint32_t> _nodes;
        vector<uint32_t> _nn;


        if (_pj.empty() == false) {
            for (vector<int>::iterator _pj_it = _pj.begin(); _pj_it != _pj.end(); _pj_it++) {
                std::map<uint32_t, std::vector<uint32_t> > ::iterator iter = this->jobnodes.find(uint32_t(*_pj_it));
                _nn = iter->second;
                _nodes.insert(_nodes.end(), _nn.begin(), _nn.end());
            }
        }
        sort(_nodes.begin(), _nodes.end());
        _nodes.erase(unique(_nodes.begin(), _nodes.end()), _nodes.end());

        return _nodes;

    }

    ResourceJobAllocation* SLURMSchedulingPolicy::findNowAllocation(Job* job, double startTime, double runtime) {
        /*
         * This is just a wrapper around the call the RRT which finds an allocation at the given time (ie now)
         * The purpose is to remove the ambiguity in the schedule() + backfill() function about calling functions in the RRT directly.
         */       
        vector<uint32_t> _nodes = getPreceedingNodes(job);
        return ((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, startTime, runtime, _nodes);
    }

    void SLURMSchedulingPolicy::jobTransitionToCompute(Job* job) {
        log->debug("SLURMSchedulingPolicy::jobTransitionToCompute for the job " + to_string(job->getJobNumber()), 2);
        if (this->outputTrace) {
            this->opTrace->jobBeginCompute(job->getJobNumber());
        }
    }

    void SLURMSchedulingPolicy::jobTransitionToOutput(Job* job) {
        log->debug("SLURMSchedulingPolicy::jobTransitionToOutput for the job " + to_string(job->getJobNumber()), 2);
        if (this->outputTrace) {
            this->opTrace->jobEndCompute(job->getJobNumber());
        }
    }




}
