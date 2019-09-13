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
#include <scheduling/simulation.h>
#include <statistics/statistics.h>
#include <scheduling/systemorchestrator.h>
#include <scheduling/swftracefile.h>
#include <scheduling/partition.h>

#include "resourceschedulingpolicy.h"

namespace Simulator {

    /**
     * The default constructor for the class
     */

    Simulation::Simulation() {
    }

    /**
     * The constructor that should be used.  
     * @param SimInfo A reference to the simulation configuration 
     * @param log A reference to the logging engince
     * @param sysOrch A reference to the system orchestrator of the simulation 
     */
    Simulation::Simulation(SimulatorCharacteristics* SimInfo, Log* log, SystemOrchestrator *sysOrch) {
        this->sysOrch = sysOrch;
        this->lastEventId = 0;
        this->globalTime = 0;
        this->SimInfo = SimInfo;
        this->log = log;
        this->ArrivalFactor = 1;
        this->pendigStatsCollection = 0;
        this->SlowdownBound = -1;
        //we copy a reference of the job list in to the scheduling policy that will be used by some scheduler policies
        this->collectStatisticsInterval = 10 * 60; //by default each 10 minutes the policy statistics will be collected
        this->policyCollector = new PolicyEntityCollector((SchedulingPolicy*) sysOrch->getPolicy(), log);
        this->JobsSimulationPerformance = NULL;
        this->PolicySimulationPerformance = NULL;
        this->StopSimulation = false;
        //regarding the progress stuff
        this->firstShow = true;
        this->showSimulationProgress = false; // Default to false to minimise stdout usage. Also pipes badly to files.
        this->totalItems = 100;
        this->lastEventType = EVENT_OTHER;
        this->lastProcessedJob = 0;
        this->usefull = 0;
        this->numberOfFinishedJobs = 0;
    }

    /**
     * The default destructor for the class 
     */

    Simulation::~Simulation() {
    }

    /**
     * Function that initialize the simulation. It mainly create all the arrival events for all the jobs that have to be simulated. 
     * @return A boolean indicating if the initialization has been done with  no errors.
     * Doctored by Nick for <reasons>
     * 1. Over generates scheduling events as jobs which can never run still generate schedule calls.
     * 2. ...
     * 94. Profit.
     */
    bool Simulation::initSimulation() {
        /******************************
         * initialize global data structures.
         * this includes loading all jobs into the event queue according to their
         * arrival times.
         */

        double previous_schedule_time = -1;
        TraceFile* workload = this->SimInfo->getWorkload();

        Job* firstArrival = NULL;

        log->debug("JobList size: " + to_string(workload->JobList.size()), 4);
        uint32_t jobnum = 0;
        
        double firstarrivaltime = 100000000;


        for (map<int, Job*>::iterator iter = workload->JobList.begin(); iter != workload->JobList.end(); ++iter) {
            Job* job = (Job*) iter->second;

            /*
             * If a job has a runtime of zero, we don;t bother with it.
             */
            if (job->getRunTime() == 0) {
                log->debug("The job " + to_string(job->getJobNumber()) + " has runtime zero, we skip it", 2);
                continue;
            }

            /*
             * Generally, we use ArrivalFactor of 1, ie no tampering with time.
             */
            double submittime = ceil(job->getJobSimSubmitTime() * this->ArrivalFactor);
            job->setJobSimSubmitTime(submittime);
            
            


            /*
             * This part quickly checks to see if any jobs uses more CPUs than exists in the system.
             * If so, the jobs doesn't ARRIVE, and is dropped.
             */
            if ((this->SimInfo->getarchConfiguration() == 0 || job->getNumberProcessors() <= (int) this->SimInfo->getarchConfiguration()->getNumberCPUs()) && job->getNumberProcessors() != -1) {


                // Arrive this job
                SimulationEvent* arrivalEvent = new SimulationEvent(EVENT_ARRIVAL, job, ++this->lastEventId, submittime);
                log->debug("Adding an EVENT_ARRIVAL event for the job " + to_string(job->getJobNumber()) + " the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(submittime), 1);
                this->events.insert(arrivalEvent);

                /*
                 * This part adds a scheduling event if (and only if) we have advanced in time since the last arrival
                 * What does this mean? The idea is to not have a schedule event for every job that arrives at exactly the same time, just one to cover them all.
                 */
                if (submittime != previous_schedule_time) {
                    SimulationEvent* scheduleEvent = new SimulationEvent(EVENT_SCHEDULE, NULL, ++this->lastEventId, submittime);
                    log->debug("Adding an EVENT_SCHEDULE event for the job " + to_string(job->getJobNumber()) + " the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(submittime), 1);
                    this->events.insert(scheduleEvent);
                    previous_schedule_time = submittime;
                } else {
                    log->debug("Skipping an EVENT_SCHEDULE event for the job " + to_string(job->getJobNumber()) + " the time for the event is " + ftos(submittime), 1);
                }
                
                if (submittime < firstarrivaltime){
                    firstArrival = job;
                    firstarrivaltime = submittime;
                }


            } else {
                job->setJobSimStatus(KILLED_NOT_ENOUGH_RESOURCES);
                log->error("The job " + to_string(job->getJobNumber()) + " uses " + to_string(job->getNumberProcessors()) + " and the architecture has fewer processors.");
            }
            jobnum++;
        }


        SimulationEvent* scheduleEvent = new SimulationEvent(EVENT_SCHEDULE, NULL, ++this->lastEventId, previous_schedule_time);
        //        log->debug("GlobalTime[" + to_string(globalTime) + "] Adding an EVENT_SCHEDULE event for the LAST job the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(previous_schedule_time), 1);
        log->debug("Adding an EVENT_SCHEDULE event for the LAST job the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(previous_schedule_time), 1);
        this->events.insert(scheduleEvent);

        bool do_backfills = true;
        double bf_timer = firstArrival->getJobSimSubmitTime();
        double bf_stride = 30;
        do_backfills = false;
        if (do_backfills) {
            for (double t = bf_timer + 1; t < previous_schedule_time; t += bf_stride) {
                SimulationEvent* backfillEvent = new SimulationEvent(EVENT_BACKFILL, NULL, ++this->lastEventId, t);
                log->debug("Adding an EVENT_BACKFILL event. The event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(t), 1);
                this->events.insert(backfillEvent);
            }

            // Best option is to do some clever pruning of events here, to suppress all the backfills.
        }




        //we set the simulation start time to the initial startTime
        this->globalTime = firstArrival->getJobSimSubmitTime();
        this->sysOrch->setGlobalTime(globalTime);
        //we create the event for collecting the statistics
        if (this->collectStatisticsInterval > 0) {
            SimulationEvent* nextCollection = new SimulationEvent(EVENT_COLLECT_STATISTICS, 0, ++this->lastEventId, globalTime + this->collectStatisticsInterval);
            this->events.insert(nextCollection);
            this->pendigStatsCollection++;
        }

        this->backfillcounter = 0;

        return true; //TODO: Return object
    }

    /**
     * This is the main loop simulation function. Here all the events are being treated by the simulator. 
     * @return A bool indicating if the simulation has been run successfully.
     */
    bool Simulation::doSimulation() {
        bool getStartTimeFirstJob = false;
        double startTimeFirstJob;
        double total_usage = 0;
        double total_usage_mem = 0;
        double full_total_usage = 0;
        double full_total_usage_mem = 0;
        double total_slowdown = 0;
        double total_wait = 0;
        //we tell to the policy that the scheduling will start now
        this->sysOrch->PreProcessScheduling();
        int ConsecutiveStatisticsCollection = 0;
        double terminatedJobs = 0;
        double startedJobs = 0;
        double totalJobs = this->SimInfo->getWorkload()->getLoadedJobs();
        log->debug("WORKLOAD LENGTH " + to_string(totalJobs), 1);

        //         For debugging out of order errors. Generates lots of stdout output.
        //        if (this->events.size() > 0) {
        //            for (EventQueue::iterator iter = this->events.begin(); iter != this->events.end(); ++iter) {
        //                SimulationEvent* event = *iter;
        //                cout << "[" << globalTime << "] ET: " << event->getTime() << "  TYPE: " << event->getType() << "   GT: " << globalTime << endl;
        //            }
        //        }

        //return true;
        /* As we are working with an FCFSF scheduling class we convert the pointer for commodity*/
        while (this->events.size() > 0 && continueSimulation()) {
            /*event treatment */
            EventQueue::iterator iter = this->events.begin();
            SimulationEvent* event = *iter;
            this->currentEvent = event;
            /* update the global to the current time for all the instances that use this time */
            log->debug("GlobalTime = " + to_string(globalTime) + "  EventTime " + to_string(event->getTime()), 2);
            assert(globalTime <= event->getTime());
            this->globalTime = event->getTime();
            this->sysOrch->setGlobalTime(event->getTime());
            Job* job = event->getJob();
            this->currentJob = job;
            if (log->getLevel() > 8)
                this->dumpEvents();
            //Create vars for the switch case
            double rtime = 0, usage = 0, usage_mem = 0, full_usage = 0, full_usage_mem = 0;
            //            double rtime, usage, usage_mem, terminationTime, full_usage, full_usage_mem;
            int cpus;
            double mem;
            double maxbound;
            double slowdown;

            //		Partition *p;

            /*we update the global time to the current event time .. */
            switch (event->getType()) {
                case EVENT_TERMINATION:
                    log->debug("EVENT_TERMINATION for the job " + to_string(job->getJobNumber()), 2);
                    log->debug("QUEUE LENGTH " + to_string(this->events.size()), 4);
                    setJobPerformanceVariables(job);
                    this->sysOrch->termination(job);
                    //                    if (swfout != NULL) swfout->logJob(job);
                    rtime = job->getRunTime();
                    cpus = job->getNumberProcessors();
                    mem = job->getRequestedMemory() / 1024 / 1024;
                    usage = cpus*rtime;
                    usage_mem = mem * cpus*rtime;
                    maxbound = (job->getRunTime() < SlowdownBound) ? SlowdownBound : job->getRunTime();
                    slowdown = (job->getJobSimWaitTime() + job->getRunTime()) / maxbound;
                    total_slowdown += slowdown;
                    total_wait += job->getJobSimWaitTime();
                    job->setDelaySec(globalTime - (job->getJobSimSubmitTime() + job->getRunTime()));
                    job->setArcherRatio(rtime / (rtime + job->getJobSimWaitTime()));
                    total_usage += usage;
                    total_usage_mem += usage_mem;
                    full_total_usage += full_usage;
                    full_total_usage_mem += full_usage_mem;
                    terminatedJobs++;
                    numberOfFinishedJobs++;
                    break;

                case EVENT_ABNORMAL_TERMINATION:
                    log->debug("EVENT_ABNORMAL_TERMINATION for the job " + to_string(job->getJobNumber()), 2);
                    sysOrch->abnormalTermination(job);
                    setJobPerformanceVariables(job);
                    break;

                case EVENT_START:
                    /* we just more the job from the wait queue to the runque
                                    and also allocate the processes to the real processors
                     */
                    if (!getStartTimeFirstJob) {
                        getStartTimeFirstJob = true;
                        startTimeFirstJob = globalTime;
                        // policy can play with this kind of data
                        sysOrch->setFirstStartTime(startTimeFirstJob);
                    }
                    log->debug("EVENT_START for the job " + to_string(job->getJobNumber()), 2);
                    sysOrch->start(job);
                    startedJobs++;
                    /* we also will show the progress information at this point */
                    /* This should be replaced with something which just dumps a progress and a timestamp rather than scrolly text. */
                    if (this->showSimulationProgress)
                        showProgress(terminatedJobs, totalJobs);
                    break;

                case EVENT_ARRIVAL:
                    log->debug("EVENT_ARRIVAL for the job " + to_string(job->getJobNumber()), 2);
                    //cout << "Arrival of job " << job->getJobNumber()  << " at globaltime: " << this->globalTime << endl;
                    /* first put the job to the wait queue and allocate it to the reservationTable */
                    sysOrch->arrival(job);
                    break;

                case EVENT_COLLECT_STATISTICS:
                    log->debug("EVENT_COLLECT_STATISTICS", 2);
                    this->pendigStatsCollection--;
                    //we generate and event for the next collection only if there is more than one event , coz in case there is one element in the event queue
                    //will mean that this event is the collection event
                    if (this->events.size() > 1 && pendigStatsCollection <= MAX_ALLOWED_STATSISTIC_EVENTS) {
                        SimulationEvent* nextCollection = new SimulationEvent(EVENT_COLLECT_STATISTICS, 0, ++this->lastEventId, globalTime + this->collectStatisticsInterval);
                        this->events.insert(nextCollection);
                        this->pendigStatsCollection++;
                    }
                    if (lastEventType == EVENT_COLLECT_STATISTICS)
                        ConsecutiveStatisticsCollection++;
                    else
                        ConsecutiveStatisticsCollection = 0;
                    //we collect the current statistics
                    if (ConsecutiveStatisticsCollection < 5)
                        this->collectStatistics();
                    break;

                case EVENT_SCHEDULE:
                    //                    cout << "Schedule from sysorch" << endl;
                    log->debug("EVENT_SCHEDULE", 2);
                    sysOrch->schedule();
                    break;

                case EVENT_BACKFILL:
                    log->debug("EVENT_BACKFILL", 2);
                    sysOrch->backfill();
                    break;

                case EVENT_TRANSITION_TO_COMPUTE:
                    log->debug("EVENT_TRANSITION_TO_COMPUTE for the job " + to_string(job->getJobNumber()), 2);
                    sysOrch->transitiontoCompute(job);
                    break;

                case EVENT_TRANSITION_TO_OUTPUT:
                    log->debug("EVENT_TRANSITION_TO_OUTPUT for the job " + to_string(job->getJobNumber()), 2);
                    sysOrch->transitiontoOutput(job);
                    break;

                default:
                    log->debug("DEFAULT CASE", 1);
                    assert(false);
                    break;
            }
            this->events.erase(iter);
            lastEventType = event->getType();
            delete event;
            log->debug("EVENTS SIZE (after) " + to_string(this->events.size()), 4);
            if (job != NULL)
                /* this is only for debug stuff for check the last processed job in case of failure */
                lastProcessedJob = job;
        }
        log->debug("Simulation has finished, we generate all the statistics and information for the analysis.", 1);
        /*
        double wtime = job->getJobSimWaitTime();
        double rtime = job->getRunTime();
        int cpus = job->getNumberProcessors();
        double response = wtime + rtime;

        double usage = cpus*rtime;
         */

        //TODO: IMPORTANT - redefine utilization
        double utilization = total_usage * 100 / ((globalTime - sysOrch->getFirstStartTime()) * sysOrch->getArchitecture()->getNumberCPUs());
        double utilization_mem = total_usage_mem * 100 / ((globalTime - sysOrch->getFirstStartTime())*(sysOrch->getArchitecture()->getTotalMemory() / 1024 / 1024));
        double full_utilization = full_total_usage * 100 / ((globalTime - sysOrch->getFirstStartTime()) * sysOrch->getArchitecture()->getNumberCPUs());
        double full_utilization_mem = full_total_usage_mem * 100 / ((globalTime - sysOrch->getFirstStartTime()) * sysOrch->getArchitecture()->getTotalMemory() / 1024 / 1024);
        std::cout << std::endl << std::endl;
        std::cout << "Total usage: " << total_usage * 100 << std::endl;
        std::cout << "Total Memory usage: " << total_usage_mem * 100 << std::endl;
        std::cout << "Total full usage: " << full_total_usage * 100 << std::endl;
        std::cout << "Total full Memory usage: " << full_total_usage_mem * 100 << std::endl;
        std::cout << "globalTime: " << (unsigned int) globalTime << std::endl;
        std::cout << "SysOrch startTime: " << sysOrch->getFirstStartTime() << std::endl;
        std::cout << "Number CPUs (sysOrch): " << sysOrch->getArchitecture()->getNumberCPUs() << std::endl;
        std::cout << "Total Memory: " << (double) sysOrch->getArchitecture()->getTotalMemory() / 1024 / (double) 1024 << std::endl;

        cout << endl << endl << "Total usage in cpus*s=" << ftos(total_usage) << ", CPUs util percentage is:" << utilization << "%. Full utilization is: " << full_utilization << "%." << std::endl;
        std::cout << "Total usage in mem*s=" << ftos(total_usage_mem) << ", Memory util percentage is: " << utilization_mem << "%. Full memory utilization is: " << full_utilization_mem << endl << endl;
        std::cout << "Average Bounded slowdown: " << total_slowdown / numberOfFinishedJobs << std::endl;
        std::cout << "Average waiting time: " << total_wait / numberOfFinishedJobs << std::endl;
        std::cout << "Jobs to be scheduled: " << this->SimInfo->getWorkload()->JobList.size() << ". Jobs started: " << startedJobs << ". Jobs finished: " << numberOfFinishedJobs << std::endl;
        std::cout << "Event queue size: " << this->events.size() << std::endl;
        std::cout << "Total number of jobs to be scheduled: " << this->SimInfo->getWorkload()->JobList.size() << endl;
        std::cout << "Total number of jobs finished: " << numberOfFinishedJobs << endl;
        std::cout << "Total number of backfilled jobs: " << backfillcounter << endl;


        this->collectFinalStatistics();
        this->sysOrch->PostProcessScheduling();

        //TODO: Use computePower.
        //if (policy->computePower())
        /* print some information about power consumed during workload */
        //policy->dumpGlobalPowerInformation(globalTime, usefull);
        //Delete events objects
        while (!events.empty()) {
            SimulationEvent* foundEvent = *(events.begin());
            events.erase(events.begin());
            delete foundEvent;
        }
        return true; //TODO: Return object
    }

    /**
     * Function currently not used - it would do some pre-processing before the simulation start
     * @return A boolean indicating if some problem has raised.
     */
    bool Simulation::loadSimulation() {
        return true; //TODO: Return object
    }

    /**
     * Shows that current statistics concerning the jobs that have been simulated and to the policy. The statistics are shown in the STDOUT.
     */
    void Simulation::showCurrentStatistics() {

        /* first we generate the statistics */
        SimStatistics* stats = this->SimInfo->getsimStatistics();
        TraceFile* workload = this->SimInfo->getWorkload();
        stats->computeJobMetrics(&workload->JobList);
        stats->computePolicyMetrics(this->policyCollector);
        stats->ShowCurrentStatisticsStd();
        std::cout << "Current global time:" + ftos(this->globalTime) << endl;
        if (lastProcessedJob != NULL)
            std::cout << "Last Job treated: " + to_string(lastProcessedJob->getJobNumber()) << endl;
        std::cout << "Current Event:" + getStringEvent(lastEventType) << endl;
        this->firstShow = true;
    }

    /**
     * Returns the human readable description of an event type.
     * @param type The type for the event.
     * @return The string with the human readable 
     */
    string Simulation::getStringEvent(event_t type) {
        switch (type) {
            case EVENT_TERMINATION:
                return "EVENT_TERMINATION";
            case EVENT_ABNORMAL_TERMINATION:
                return "EVENT_ABNORMAL_TERMINATION";
            case EVENT_START:
                return "EVENT_START";
            case EVENT_ARRIVAL:
                return "EVENT_ARRIVAL";
            case EVENT_COLLECT_STATISTICS:
                return "EVENT_COLLECT_STATISTICS";
            default:
                return "OTHER";
        }
    }

    /**
     * Function that gathers both job and policy statistics and saves to the output file if specified, otherwise to the standard output.
     */
    void Simulation::collectFinalStatistics() {

        if (numberOfFinishedJobs < 1) {
            log->debug("No finished jobs. We do not compute the statistics.");
            return;
        }
        SimStatistics* stats = this->SimInfo->getsimStatistics();
        TraceFile* workload = this->SimInfo->getWorkload();
        /*in case of required we dump the performance for the jobs */
        if (this->JobsSimulationPerformance != NULL)
            this->JobsSimulationPerformance->createJobsCSVInfoFile(&workload->JobList);
        if (this->PolicySimulationPerformance != NULL)
            this->PolicySimulationPerformance->createPolicyCSVInfoFile(this->policyCollector);
        /*  we generate the statistics */
        stats->computeJobMetrics(&workload->JobList);
        stats->computePolicyMetrics(this->policyCollector);
        stats->saveGlobalStatistics();
        stats->doHeatmap(&workload->JobList);
    }

    /**
     * Indicates if the simulation should be finished 
     * @return A bool indicating if the simulation should be stopped 
     */
    bool Simulation::continueSimulation() {
        bool continueSimulation = !this->getStopSimulation();
        return continueSimulation;
    }

    /**
     * Sets the paraver trace for the current simulation
     * @param trace A reference to the paraver trace 
     */
    //    void Simulation::setParaverTrace(ParaverTrace* trace) {
    //        //TODO: SysOrch Set Paraver Trace
    //        //this->policy->setPrvTrace(trace);
    //    }

    /**
     * Returns the current globalTime of the simulation 
     * @return An integer with the global time 
     */
    double Simulation::getglobalTime() const {
        return globalTime;
    }

    /**
     * Sets the globalTime for the simulation 
     * @param theValue The integer containing the global time for the simulation 
     */
    void Simulation::setGlobaltime(double theValue) {
        globalTime = theValue;
    }

    /**
     * This function is mainly used for debugging or for state that the simulation is performing as expected. It will dump all the pending events to the debug engine.
     */
    void Simulation::dumpEvents() {
        EventQueue::iterator it = this->events.begin(); // should be optimized !
        for (; it != this->events.end(); ++it) {
            SimulationEvent* cEvent = *it;
            /* debug stuff */
            if (this->log->getLevel() > 8 && (cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION || cEvent->getType() == EVENT_TRANSITION_TO_OUTPUT || cEvent->getType() == EVENT_TRANSITION_TO_COMPUTE))
                this->log->debug("Event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled for " + ftos(cEvent->getTime()) + " for the job " + to_string(cEvent->getJob()->getJobNumber()), 2);
        }
    }

    /**
     * Delete all the events that concerns the provided job. This is used in situations when the job has been rescheduled and the events that was previously generated have lost its validity.
     * @param job The job to whom events have to be deleted 
     */
    void Simulation::deleteJobEvents(Job* job) {
        vector<SimulationEvent*> toDelete;
        EventQueue::iterator it = this->events.begin(); // should be optimized !
        log->debug("Deleting events for job " + to_string(job->getJobNumber()), 2);
        dumpEvents();

        for (; it != this->events.end(); ++it) {

            SimulationEvent* cEvent = *it;
            /* debug stuff */
            if (this->log->getLevel() > 8 && (cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION)) {
                this->log->debug("Event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled by " + ftos(cEvent->getTime()) + " for the job " + to_string(cEvent->getJob()->getJobNumber()), 2);
            }



            // SCHEDULE events don't have an associated job, so don't try to print it!
            // Plus, we skip excessive SCHEDULEs in quick succession, so no need to worry about them wasting resource.
            //            if (this->log->getLevel() > 8 && (cEvent->getType() == EVENT_SCHEDULE)) {
            //                this->log->debug("GlobalTime[" + to_string(globalTime) + "]" + "Event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled by " + ftos(cEvent->getTime())  + " for the job " + itos( cEvent->getJob()->getJobNumber() ), 2);
            //            }

            if ((cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION || cEvent->getType() == EVENT_TRANSITION_TO_COMPUTE || cEvent->getType() == EVENT_TRANSITION_TO_OUTPUT) && cEvent->getJob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid()) {
                this->log->debug("Deleting the event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled by " + ftos(cEvent->getTime()) + " for the job " + to_string(cEvent->getJob()->getJobNumber()), 2);
                toDelete.push_back(cEvent);
                continue;
            }

            //            if (cEvent->getType() != EVENT_COLLECT_STATISTICS && cEvent->getType() != EVENT_SCHEDULE && cEvent->getJob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid()) {
            //                log->debug("dsdfsdfsf", 2);
            //                //this->log->debug("GlobalTime[" + to_string(globalTime) + "]" + "Deleting the event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled by " + ftos(cEvent->getTime()) + " for the job " + to_string(cEvent->getJob()->getJobNumber()), 2);
            //                toDelete.push_back(cEvent);
            //                continue;
            //            }

            //            /* debug stuff */
            //            if (this->log->getLevel() > 8 && (cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION)){
            //                this->log->debug("GlobalTime[" + to_string(globalTime) + "]" + "Event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled by " + ftos(cEvent->getTime()) + " for the job " + to_string(cEvent->getJob()->getJobNumber()), 2);
            //            }
        }



        for (vector<SimulationEvent*>::iterator itdel = toDelete.begin(); itdel != toDelete.end(); ++itdel) {
            SimulationEvent* ev = *itdel;
            EventQueue::iterator it = this->events.find(ev); // should be optimized !
            assert(it != this->events.end());
            this->events.erase(it);
        }
    }

    /**
     * Delete the finish pending events for the required job. 
     * @param job The job to whom events have to be deleted
     */
    void Simulation::deleteJobFinishEvent(Job* job) {
        if (job == this->currentJob && this->currentEvent->getType() == EVENT_ARRIVAL)
            return;
        EventQueue::iterator it = this->events.begin(); // should be optimized !
        for (; it != this->events.end(); ++it) {
            SimulationEvent* cEvent = *it;
            if (cEvent->getType() != EVENT_COLLECT_STATISTICS && cEvent->getJob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid()
                    && (cEvent->getType() == EVENT_TERMINATION || cEvent->getType() == EVENT_ABNORMAL_TERMINATION)
                    ) {
                this->log->debug("Deleting the event type " + to_string(cEvent->getType()) + " with the id " + to_string(cEvent->getid()) + " that is scheduled by " + ftos(cEvent->getTime()) + " for the job " + to_string(cEvent->getJob()->getJobNumber()), 2);
                this->events.erase(it);
                delete cEvent;
                //we can return due to there should not be more than one event termination per jobs
                return;
            }
        }
    }

    /**
     * Function that generate all the required events for force a job finish in the provided time. 
     * @param endTime The time when the job will finish 
     * @param job The job to whom finish event has to be created
     */
    void Simulation::forceJobFinish(double endTime, Job* job) {
        //debugging stuff
        if (this->log->getLevel() > 8)
            this->dumpEvents();
        /* If the job is the one that is currently processed and it is a start time we do not have to create a finish event
         * it will be created in function.
         * Carefull! If the user runtime estimation was accurate (aka: requested runtime < real runtime). We will use the getJobSimFinishTime) instead
         * Search for note EVENT_TERMINATION_NOTE1 in this same document.
         */
        if (!(job == this->currentJob && this->currentEvent->getType() == EVENT_START)) {
            log->debug("Rescheduling the job " + to_string(job->getJobNumber()) + ". Adding an EVENT_TERMINATE " + ftos(endTime), 2);
            /*we create a job termination for the job .. if it fails, or must be killed*/
            SimulationEvent* finishjob = NULL;
            double terminationTime = -1;
            //TODO: Use getAllowUserEstimatedRuntimeViolation
            //if(policy->getAllowUserEstimatedRuntimeViolation() ||
            //		(job->getJobSimStartTime()+job->getOriginalRequestedTime()) > endTime //good user estimation :)
            //)
            //	terminationTime = endTime;
            //else
            terminationTime = job->getJobSimStartTime() + job->getOriginalRequestedTime();
            if (job->getStatus() == COMPLETED) {
                finishjob = new SimulationEvent(EVENT_TERMINATION, job, ++this->lastEventId, terminationTime);
                log->debug("Adding an EVENT_TERMINATION event for the job " + to_string(job->getJobNumber()) + " the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(endTime), 2);
            } else {
                finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION, job, ++this->lastEventId, terminationTime);
                log->debug("Adding an EVENT_ABNORMAL_TERMINATION event for the job " + to_string(job->getJobNumber()) + " the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(endTime), 2);
            }
            this->events.insert(finishjob);
        }
        //debugging stuff
        if (this->log->getLevel() > 8)
            this->dumpEvents();
    }

    /**
     * Function that generate all the required events for force a job start in the provided time. 
     * @param startTime The start time for the new start event 
     * @param endTime The end time for the new  finialization event 
     * @param job The job to whom finish event has to be created 
     */
    void Simulation::forceJobStart(double startTime, double endTime, Job* job) {
        //debugging stuff
        if (this->log->getLevel() > 8)
            this->dumpEvents();
        //In case that this job is the once that is being scheduled and the current event is EVENT_ARRIVAL we do not
        //create any EVENT TERMINATION due to this events will be created in the main doSimulation method
        if (!(job == this->currentJob && this->currentEvent->getType() == EVENT_ARRIVAL)) {
            /*create  event for the job start*/
            SimulationEvent* startjob = new SimulationEvent(EVENT_START, job, ++this->lastEventId, startTime);
            /*we create a job termination for the job .. if it fails, or must be killed*/
            SimulationEvent* finishjob = NULL;
            log->debug("Rescheduling the job " + to_string(job->getJobNumber()) + ". Adding an EVENT_START  the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(startTime), 2);
            if (job->getStatus() == COMPLETED) {
                finishjob = new SimulationEvent(EVENT_TERMINATION, job, ++this->lastEventId, endTime);
                log->debug("Adding an EVENT_TERMINATION event for the job " + to_string(job->getJobNumber()) + " the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(endTime), 2);
            } else {
                finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION, job, ++this->lastEventId, endTime);
                log->debug("Adding an EVENT_ABNORMAL_TERMINATION event for the job " + to_string(job->getJobNumber()) + " the event id is " + to_string(this->lastEventId) + " the time for the event is " + ftos(endTime), 2);
            }
            this->events.insert(startjob);
            this->events.insert(finishjob);
        }
        //debugging stuff
        if (this->log->getLevel() > 8)
            this->dumpEvents();
    }

    /**
     * This function updates the performance variables for the job 
     * @param job A reference to the job that has finished 
     */
    void Simulation::setJobPerformanceVariables(Job* job) {
        job->setJobSimWaitTime(job->getJobSimStartTime() - job->getJobSimSubmitTime());
        //TODO: Use getCostAllocationUnit
        // if(policy->getCostAllocationUnit() > 0)
        // job->setMonetaryCost((job->getRunTime()/3600)*policy->getCostAllocationUnit());
        //compute the BSLD considering the frequency and set it to the job
        // it is more fair to divide with the original runtime (execution time at the top frequency)
        //TODO: Adapt reduced frequency to sysorch.
        //if(job->getRunsAtReducedFrequency()) {
        // double runtime = job->getRunTime()* this->policy->getRatio(job->getJobNumber(),job->getRunsAtFrequency());
        // job->setJobSimSLD((job->getJobSimWaitTime()+runtime)/job->getRunTime());
        // job->setJobSimBSLD((job->getJobSimWaitTime()+runtime)/max(job->getRunTime(),(double)BSLD_THRESHOLD));
        // }
    }

    /**
     * Function that shows to the standard output the progress of the simulation , the progress of the simulation is the ratio of jobs that have run and the jobs that have already finishied 
     * @param current The current number of finished jobs 
     * @param total The total number of jobs in the simulation
     */
    void Simulation::showProgress(double current, double total) {
        double progress = ceil((current / total)*100);
        double items = ceil(totalItems * (current / total));
        string prg_str = ftos(progress);
        if (prg_str.size() == 1)
            prg_str = "  " + prg_str;
        else if (prg_str.size() == 2)
            prg_str = " " + prg_str;
        if (this->firstShow) {
            this->firstShow = false;
            std::cout << "Progress simulation:";
        } else {
            std::cout << ("\b\b\b\b\b\b");
            string curr_str = ftos(current);
            for (int i = 0; i < this->totalItems + curr_str.size() + 3; i++)
                std::cout << ("\b");
        }
        std::cout.flush();
        std::cout << prg_str + " %";
        for (int i = 0; i < this->totalItems; i++) {
            if (i < items)
                std::cout << "*";
            else
                std::cout << " ";
        }
        std::cout << "| (" << current << ")";
        std::cout.flush();
    }

    /**
     * Returns the amount of time between two statistics collection 
     * @return  A double containing the invertal between two statistics collection 
     */
    double Simulation::getcollectStatisticsInterval() const {
        return collectStatisticsInterval;
    }

    /**
     * Sets the interval of statistics collection 
     * @param theValue The interval of statiscs collection
     */
    void Simulation::setCollectStatisticsInterval(double theValue) {
        collectStatisticsInterval = theValue;
    }

    /**
     * Invoqued when the statistics event is raised. 
     */
    void Simulation::collectStatistics() {
        this->policyCollector->pullValues(this->globalTime);
    }

    double Simulation::getArrivalFactor() const {
        return ArrivalFactor;
    }

    /**
     * Sets the arrival factor for the jobs 
     * @param theValue The arrival factor
     */
    void Simulation::setArrivalFactor(double theValue) {
        ArrivalFactor = theValue;
    }

    double Simulation::getSlowdownBound() const {
        return SlowdownBound;
    }

    /**
     * Sets the bound for the bounded slowdown 
     * @param bound
     */
    void Simulation::setSlowdownBound(double bound) {
        SlowdownBound = bound;
    }

    /**
     * Returns the CVS streamer for the job information
     * @return A reference to the job cvs job infor converter 
     */
    CSVJobsInfoConverter* Simulation::getJobsSimulationPerformance() const {
        return JobsSimulationPerformance;
    }

    /**
     * Sets the CVS streamer for the job information
     * @param theValue The reference to the converter
     */
    void Simulation::setJobsSimulationPerformance(CSVJobsInfoConverter* theValue) {
        JobsSimulationPerformance = theValue;
    }

    /**
     * Returns true if the simulation has to be stoped or not 
     * @return A bool indicating if the simulation has to be stoped 
     */
    bool Simulation::getStopSimulation() const {
        return StopSimulation;
    }

    /**
     * Sets if the simulation has to be stoped or not.
     * @param theValue The bool indicating if to stop the simulation
     */
    void Simulation::setStopSimulation(bool theValue) {
        StopSimulation = theValue;
    }

    /**
     * Returns true if the progress of the simulation has to be shown in the standard output
     * @return A bool indicating if the progress will be shown in the std out 
     */
    bool Simulation::getshowSimulationProgress() const {
        return showSimulationProgress;
    }

    /**
     * Sets if the simulation progress has to be shown in the standard output
     * @param theValue Indicates if the progress has to be shown 
     */
    void Simulation::setShowSimulationProgress(bool theValue) {
        std::cout << "Set simulation progress to " << theValue << std::endl;
        showSimulationProgress = theValue;
    }

    /**
     * Returns the CVS streamer for the policy information
     * @return A reference to the job cvs policy infor converter 
     */
    CSVPolicyInfoConverter* Simulation::getPolicySimulationPerformance() const {
        return PolicySimulationPerformance;
    }

    /**
     * Sets the CVS streamer for the policy information
     * @param theValue The reference to the converter
     */
    void Simulation::setPolicySimulationPerformance(CSVPolicyInfoConverter* theValue) {
        PolicySimulationPerformance = theValue;
    }

    void Simulation::initPower(PowerConsumption* powerinf) {
        //TODO: sysOrch->initPower?
        //policy->initializationPower(powerinf);
    }

    /**
     * Sets a job start event
     */
    void Simulation::simInsertJobStartEvent(Job *job) {
        SimulationEvent* startjob = new SimulationEvent(EVENT_START, job, ++this->lastEventId, job->getJobSimStartTime());
        this->events.insert(startjob);
        return;
    }

    /**
     * Sets a job finish event
     */
    void Simulation::simInsertJobFinishEvent(Job *job) {
        SimulationEvent* finishjob = new SimulationEvent(EVENT_TERMINATION, job, ++this->lastEventId, job->getJobSimFinishTime());
        this->events.insert(finishjob);
        return;
    }

    void Simulation::simModifyJobFinishEvent(Job *job) {
        // First delete old finish event
        EventQueue::iterator it = this->events.begin(); // should be optimized !
        for (; it != this->events.end(); ++it) {
            SimulationEvent* cEvent = *it;
            if ((cEvent->getJob() != NULL) && (cEvent->getJob()->getJobNumber() == job->getJobNumber()) && (cEvent->getType() == EVENT_TERMINATION || cEvent->getType() == EVENT_ABNORMAL_TERMINATION)) {
                this->events.erase(it);
                delete cEvent;
                break;
            }
        }

        // Now add the new event
        SimulationEvent* finishjob = new SimulationEvent(EVENT_TERMINATION, job, ++this->lastEventId, job->getJobSimFinishTime());
        this->events.insert(finishjob);
        return;
    }

    void Simulation::simInsertScheduleEvent() {
        SimulationEvent* scheduleevent = new SimulationEvent(EVENT_SCHEDULE, NULL, ++this->lastEventId, this->globalTime);
        this->events.insert(scheduleevent);
    }

    void Simulation::simInsertComputeBeginEvent(Job *job) {
        double _temp = job->getIOCompRatio();
        _temp *= job->getRunTime();
        _temp += job->getJobSimStartTime();
        SimulationEvent* scheduleevent = new SimulationEvent(EVENT_TRANSITION_TO_COMPUTE, job, ++this->lastEventId, int(_temp));
        //        log->debug("Inserting TRANS TO COMPUTE for job " + std::to_string(job->getJobNumber()) + " at " + std::to_string(int(_temp)), 2);
        this->events.insert(scheduleevent);

    }

    void Simulation::simInsertComputeEndEvent(Job *job) {
        double _temp = 1.0 - job->getIOCompRatio();
        _temp *= job->getRunTime();
        _temp += job->getJobSimStartTime();
        SimulationEvent* scheduleevent = new SimulationEvent(EVENT_TRANSITION_TO_OUTPUT, job, ++this->lastEventId, _temp);
        this->events.insert(scheduleevent);

    }

    SimulatorCharacteristics* Simulation::getSimInfo() {
        return this->SimInfo;
    }

    uint64_t* Simulation::getGlobalTimeRef() {
        return &globalTime;
    }

 
}
