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
#ifndef SIMULATORSIMULATION_H
#define SIMULATORSIMULATION_H

#define MAX_ALLOWED_STATSISTIC_EVENTS 1

#include <scheduling/schedulingpolicy.h>
#include <scheduling/simulatorcharacteristics.h>
#include <scheduling/simulationevent.h>
#include <scheduling/reservationtable.h>
#include <utils/log.h>
#include <utils/paravertrace.h>
#include <scheduling/policyentitycollector.h>
#include <utils/cvsjobsinfoconverter.h>
#include <utils/csvpolicyinfoconverter.h>
#include <scheduling/powerconsumption.h>
#include <scheduling/swftracefile.h>
#include <set>

using namespace std;
using std::set;


namespace Simulator {

    /**
     * @author Francesc Guim,C6-E201,93 401 16 50,
     */

    /** Struct that contains the lessthan operator that is used in the EventQueue type. The jobs are ordered by its time, and in case equal time, the events are ordered by its ide   */
    struct eventorder_t {

        bool operator()(SimulationEvent* event1, SimulationEvent* event2) const {

            /*
             * TRUE means event 1 goes closer to the head of the queue
             * FALSE means event 2 goes closer to the head of the queue
             */

            volatile double e1time = event1->getTime();
            volatile double e2time = event2->getTime();
            assert(e1time >= 0 && e2time >= 0);

            /* 
             * Simple case, if event 1 occurs before event 2.
             */
            if (e1time < e2time)
                return true;

            /* 
             * Simple case, if event 2 occurs before event 1.
             */

            if (e2time < e1time)
                return false;

            /*
             * Complex case, if both events occur at the same time.
             * We have to rank on priority according to event type, with SCHEDULE always being the loser.
             */
            if (e1time == e2time) {
                if (event1->getType() == EVENT_SCHEDULE && event2->getType() != EVENT_SCHEDULE) {
                    return false;
                } else if (event1->getType() != EVENT_SCHEDULE && event2->getType() == EVENT_SCHEDULE) {
                    return true;
                } else {
                    return event1->getid() < event2->getid();
                }
            }

            //else if( (event1->getType() == EVENT_START) && (event2->getType() == EVENT_START))
            //	return false;
            //else if( event1->getType() == EVENT_START ) {
            //		return true;
            //}

            //	return true;
            //else if( event2->getType() == EVENT_START )
            //	return true;
            //	else if(event1->getType() == EVENT_TERMINATION && event2->getType() == EVENT_ARRIVAL)
            /* in case that two events occurs at the same time, we priorize the jobs that free processors as Dan in SimEasy*/
            //	   return true;
            //	else if (event1->getType() == EVENT_ARRIVAL && event2->getType() == EVENT_START)
            /* in case that two events occurs at the same time, we priorize the event that is a event arrival due to is preferable to schedule again*/
            //	   return true;

            // If we don't know, just pick event 1.
            return true;
        }
    };


    /** Event queue that stores the pending events */
    typedef set<SimulationEvent*, eventorder_t> EventQueue;

    /**  The class that implements the event driven simulation. */
    class Simulation {
        //Forward declaration 
        class Policy;

    public:
        Simulation();
        ~Simulation();
        Simulation(SimulatorCharacteristics* SimInfo, Log* log, SystemOrchestrator *sysorch);

        virtual bool initSimulation();
        virtual bool doSimulation();
        virtual bool loadSimulation();
        virtual void collectStatistics();
        virtual bool continueSimulation(); //indicates if the continuation has to continue .. depending on batch length etc
        void setGlobaltime(double theValue);
        double getglobalTime() const;
        uint64_t* getGlobalTimeRef();

        //The following functions are intended to be used for  external components, like scheduling policies, for  delete events or force creating new events. For example these functions are used by the deepSearchScheduling polcy.
        void deleteJobEvents(Job* job); //be aware this function is quite costly if not invoqued with the second parameter that indicates that start time of when the simulation instance must start to look
        void forceJobStart(double startTime, double endTime, Job* job);
        void deleteJobFinishEvent(Job* job);
        void forceJobFinish(double endTime, Job* job);

        //Sets the the final values to the job on its completation
        void setJobPerformanceVariables(Job* job);

        //sets and gets
        void setCollectStatisticsInterval(double theValue);
        double getcollectStatisticsInterval() const;
        void setArrivalFactor(double theValue);
        double getArrivalFactor() const;
        void setJobsSimulationPerformance(CSVJobsInfoConverter* theValue);
        CSVJobsInfoConverter* getJobsSimulationPerformance() const;

        void setSlowdownBound(double bound);
        double getSlowdownBound() const;

        //for debuggin stuff 
        void dumpEvents();
        void setStopSimulation(bool theValue);
        bool getStopSimulation() const;

        //progress stuff
        void setShowSimulationProgress(bool theValue);
        bool getshowSimulationProgress() const;
        void showCurrentStatistics(); //displays the current statistcs

        void setPolicySimulationPerformance(CSVPolicyInfoConverter* theValue);
        CSVPolicyInfoConverter* getPolicySimulationPerformance() const;

        void initPower(PowerConsumption* powerinf);

        void simInsertJobStartEvent(Job *job);
        void simInsertJobFinishEvent(Job *job);
        void simModifyJobStartEvent(Job *job);
        void simModifyJobFinishEvent(Job *job);
        void simInsertScheduleEvent();


        void simInsertComputeBeginEvent(Job *job);
        void simInsertComputeEndEvent(Job *job);

        SimulatorCharacteristics* getSimInfo();


        int backfillcounter;
    protected:
        //functions oriented to check all the events are released
        //int increaseJobEvents(Job* job); 
        //int decreaseJobEvents(Job* job); 
        //map<Job*,int> JobEvents;


        SystemOrchestrator* sysOrch; /**< A reference to the schedulingpolicy that is simulated*/
        SimulatorCharacteristics* SimInfo; /**< The characarcteristics of the current simulation  */
        EventQueue events; /**< The queue of events that have to be treated */
        uint64_t globalTime; /**< The current time of the simulation  */
        Log* log; /** The logging engine */
        int lastEventId; /**< The last id assigned to an event , the ids for events must be unique */
        double ArrivalFactor; /**< indicates for how much the submitt time has to be multiplied  */

        double SlowdownBound; /**< indicated the bound used for the Bounded Slowdown. */

        SimulationEvent* currentEvent; /**< this is stored for consistency, coz in case of dioing the main loop simulation and calling the method deleteJobEvents something mai go worng, coz we could be try to deleting the same event in both places*/
        Job* currentJob; //same as before

        /*STATISTICS STUFF */
        double pendigStatsCollection; /**< Indicates how many statistics events are pending in the simulation  */

        // statistical collector 
        double collectStatisticsInterval; /**< indicates when the simulator will have to collect the punctual statistics that are not associated whit any event (by default it is 10 minutes), */
        PolicyEntityCollector* policyCollector;

        //this function collects all the statistics that are associated to the current execution 
        void collectFinalStatistics();
        CSVJobsInfoConverter* JobsSimulationPerformance; /**< This CVS converter, in case of created, will store all the information concerning the jobs once the simulation is finished  */
        CSVPolicyInfoConverter* PolicySimulationPerformance; /**< This CSV converer, in case of created, will store the information coserning the policy once the simulation is finished */

        bool StopSimulation; /**< indicates if the simulation should finish or not */

        //indicate progress - if true indicates the progress simulation 
        bool showSimulationProgress; /**< Inidicates if the similation progress (percentage of finished jobs) has to be shown in the STDOUT*/
        bool firstShow; /**< Indicates if it is the first time that the progress is shown, this is for delete the appropiate number of chars in the desplay */
        double totalItems; /**< total bullets to shown in the progress bar */
        void showProgress(double current, double total); //shows to the STDIO the progress respect the total and the current processed stuff
        string getStringEvent(event_t type);

        //last event and job treated for dumping current status 
        event_t lastEventType; /**< The last event treated  */
        Job* lastProcessedJob; /**< The last processed job  */
        double startTimeFirstJob;
        int usefull; /* save how many processors per runtime will be used in simulation */

        int numberOfFinishedJobs;

        SimulationEvent* startjob;
        SimulationEvent* finishjob;

        trace* output_trace; // Generic class for all output traces.
    };

}

#endif
