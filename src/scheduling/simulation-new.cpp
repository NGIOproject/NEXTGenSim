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

//#include <scheduling/gridjob.h>

namespace Simulator {

/**
 * The default constructor for the class
 */

Simulation::Simulation() 
{
}

/**
 * The constructor that should be used.  
 * @param SimInfo A reference to the simulation configuration 
 * @param log A reference to the logging engince
 * @param sysOrch A reference to the system orchestrator of the simulation 
 */
Simulation::Simulation(SimulatorCharacteristics* SimInfo,Log* log,SystemOrchestrator *sysOrch) 
{
	this->sysOrch = sysOrch;
	this->lastEventId = 0;
	this->globalTime = 0;
	this->SimInfo = SimInfo;
	this->log = log;
	this->ArrivalFactor = 1;
	this->pendigStatsCollection = 0;
	//we copy a reference of the job list in to the scheduling policy that will be used by some scheduler policies
	this->collectStatisticsInterval = 10*60;  //by default each 10 minutes the policy statistics will be collected
	this->policyCollector = new PolicyEntityCollector((SchedulingPolicy*)sysOrch->getPolicy(),log);
	this->JobsSimulationPerformance  = NULL;
	this->PolicySimulationPerformance = NULL;
	this->StopSimulation = false;
	//regarding the progress stuff
	this->firstShow = true;
	this->showSimulationProgress = true;
	this->totalItems = 100;
	this->lastEventType = EVENT_OTHER;
	this->lastProcessedJob = 0;
	this->usefull = 0;
	this->numberOfFinishedJobs = 0;
}

/**
 * The default destructor for the class 
 */

Simulation::~Simulation()
{
}

/**
 * Function that initialize the simulation. It mainly create all the arrival events for all the jobs that have to be simulated. 
 * @return A boolean indicating if the initialization has been done with  no errors.
 */
bool Simulation::initSimulation()
{
	/******************************
	 * initialize global data structures.
	 * this includes loading all jobs into the event queue according to their
	 * arrival times.
	 */
	TraceFile* workload =  this->SimInfo->getWorkload();
	/* we create one event arrival for eahc of the jobs */
	map<int,Job*>::iterator iter = workload->JobList.begin();
	Job* firstArrival = iter->second;
	for(;iter != workload->JobList.end();++iter) {
		Job* job = (Job*) iter->second;
		/*at this time we do not modelize what happends with jobs with runtime 0 */
		if(job->getRunTime() == 0) {
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"The job "+to_string(job->getJobNumber())+" has runtime zero, we skip it",2);
			continue;
		}
		/* we compute the submitt time according the arrival factor */
		double submittime = ceil(job->getJobSimSubmitTime()*this->ArrivalFactor);
		job->setJobSimSubmitTime(submittime);
		/*it's time to create an event arrival for each job*/
		if((this->SimInfo->getarchConfiguration() == 0 ||
				job->getNumberProcessors() <= this->SimInfo->getarchConfiguration()->getNumberCPUs()) &&
				job->getNumberProcessors() != -1) {
			SimulationEvent* arrivalEvent = new SimulationEvent(EVENT_ARRIVAL,job,++this->lastEventId,submittime);
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_ARRIVAL event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(submittime));
			this->events.insert(arrivalEvent);
		}
		else {
			job->setJobSimStatus(KILLED_NOT_ENOUGH_RESOURCES);
			log->error("The job "+to_string(job->getJobNumber())+" uses "+to_string(job->getNumberProcessors())+" and the architecture has less processors.");
		}
	}
	//we set the simulation start time to the initial startTime
	this->globalTime = firstArrival->getJobSimSubmitTime();
	//we create the event for collecting the statistics
	SimulationEvent* nextCollection = new SimulationEvent(EVENT_COLLECT_STATISTICS,0,++this->lastEventId,globalTime+this->collectStatisticsInterval);
	this->events.insert(nextCollection);
	this->pendigStatsCollection++;
	return true; //TODO: Return object
}

/**
 * This is the main loop simulation function. Here all the events are being treated by the simulator. 
 * @return A bool indicating if the simulation has been run successfully.
 */
bool Simulation::doSimulation()
{
	bool getStartTimeFirstJob = false;
	double startTimeFirstJob;
	double total_usage = 0;
	//we tell to the policy that the scheduling will start now
	this->sysOrch->PreProcessScheduling();
	int ConsecutiveStatisticsCollection = 0;
	double terminatedJobs = 0;
	double totalJobs = this->SimInfo->getWorkload()->getLoadedJobs();
	/* As we are working with an FCFSF scheduling class we convert the pointer for commodity*/
	while(this->events.size() > 0 && continueSimulation()) {
		/*event treatment */
		EventQueue::iterator iter = this->events.begin();
		SimulationEvent* event = *iter;
		this->currentEvent = event;
		/* update the global to the current time for all the instances that use this time */
		assert(globalTime <= event->getTime());
		this->globalTime = event->getTime();
		this->sysOrch->setGlobalTime(event->getTime());
		Job* job = event->getJob();
		this->currentJob = job;
		if(log->getLevel() > 8)
			this->dumpEvents();
		//Create vars for the switch case
		double rtime,usage,terminationTime;
		int cpus;
		/*we update the global time to the current event time .. */
		switch(event->getType()) {
		case EVENT_TERMINATION:
			//TODO: sysOrch->computePower()?
			//if(policy->computePower())
			//	policy->updateEnergyConsumedInPower(globalTime, job);
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"EVENT_TERMINATION for the job "+ to_string(job->getJobNumber()),1);
			this->sysOrch->termination(job);
			/* we set the variables related to the job */
			setJobPerformanceVariables(job);
			//double wtime = job->getJobSimWaitTime(); //UNUSED
			rtime = job->getRunTime();
			cpus = job->getNumberProcessors();
			//double response = wtime + rtime; //UNUSED
			usage = cpus*rtime;
			total_usage += usage;
			/* for progress indication */
			terminatedJobs++;
			numberOfFinishedJobs++;
			break;
		case EVENT_ABNORMAL_TERMINATION:
			//TODO: sysOrch->computePower()?
			// if(policy->computePower()) {
				// policy->updateEnergyConsumedInPower(globalTime, job);
				/*
						if (policy->getPowerAware() && job->getRunsAtReducedFrequency())
						policy->decreaseNumberOfReducedModeCpus(job);
				 */
			// }
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"EVENT_ABNORMAL_TERMINATION for the job "+ to_string(job->getJobNumber()),1);
			/* right now the unique stuff to do is to terminate the job */
			sysOrch->abnormalTermination(job);
			/* we set the variables related to the job */
			setJobPerformanceVariables(job);
			/* for progress indication */
			terminatedJobs++;
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
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"GlobalTime["+to_string(globalTime)+"]"+"EVENT_START for the job "+ to_string(job->getJobNumber()),1);
			sysOrch->start(job);
			/* we also will show the progress information at this point */
			if(this->showSimulationProgress)
				showProgress(terminatedJobs,totalJobs);
			break;
		case EVENT_ARRIVAL:
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"EVENT_ARRIVAL for the job "+ to_string(job->getJobNumber()),1);
			/* first put the job to the wait queue and allocate it to the reservationTable */
			sysOrch->arrival(job);
			/*create  event for the job start - in case the start time and finish time has been assigned */
			if(job->getJobSimStartTime() != -1 && job->getJobSimFinishTime() != -1) {
				startjob = new SimulationEvent(EVENT_START,job,++this->lastEventId,job->getJobSimStartTime());
				log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_START event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(job->getJobSimStartTime()),1);
				/*We create a job termination for the job .. if it fails, or must be killed
				 * Previously the getJobSimFinishTime was used to create this event. However that is not true for those policies that do not allow extending job runtime. Two situations may happen here:
				 * a) In job scheduling, if the estimated runtime is provided by the user and the system does not support extending the runtime. The job will be killed. Thus, what we has to be done is to create the termination job according to the runtime provided in the trace and kill the job otherwise.
				 * b) Some job policies like the using job predictors may allow extending the runtime. For those situations the runtime needs to be extended
				 * For a) we are going to use the provided runtime. For be) We will use the estimatedJobFinishTime
				 * Careful! If the user runtime estimation was accurate (aka: requested runtime < real runtime). We will use the getJobSimFinishTime) instead
				 * This also applies to forceJobFinish function.
				 * (Reference note EVENT_TERMINATION_NOTE1)
				 */
				terminationTime = -1;
				//TODO: Use getAllowUserEstimatedRuntimeViolation
				// if(policy->getAllowUserEstimatedRuntimeViolation() ||
						// (job->getJobSimStartTime()+job->getOriginalRequestedTime()) > job->getJobSimFinishTime() //good user estimation :)
				// )
					// terminationTime = job->getJobSimFinishTime();
				// else
					// terminationTime = job->getJobSimStartTime()+job->getOriginalRequestedTime();
					terminationTime = job->getJobSimFinishTime();
				if(job->getStatus() == COMPLETED) {
					finishjob = new SimulationEvent(EVENT_TERMINATION,job,++this->lastEventId,terminationTime);
					log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_TERMINATION event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(job->getJobSimFinishTime()),1);
				}
				else {
					finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION,job,++this->lastEventId,terminationTime);
					log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_ABNORMAL_TERMINATION event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(job->getJobSimFinishTime()),1);
				}
				this->events.insert(startjob);
				this->events.insert(finishjob);
			}
			break;
		case EVENT_COLLECT_STATISTICS:
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"Processing a EVENT_COLLECT_STATISTICS envent",2);
			this->pendigStatsCollection--;
			//we generate and event for the next collection only if there is more than one event , coz in case there is one element in the event queue
			//will mean that this event is the collection event
			if(this->events.size() > 1 && pendigStatsCollection <= MAX_ALLOWED_STATSISTIC_EVENTS) {
				SimulationEvent* nextCollection = new SimulationEvent(EVENT_COLLECT_STATISTICS,0,++this->lastEventId,globalTime+this->collectStatisticsInterval);
				this->events.insert(nextCollection);
				this->pendigStatsCollection++;
			}
			if(lastEventType == EVENT_COLLECT_STATISTICS)
				ConsecutiveStatisticsCollection++;
			else
				ConsecutiveStatisticsCollection = 0;
			//we collect the current statistics
			if(ConsecutiveStatisticsCollection < 5)
				this->collectStatistics();
			break;
		default:
			assert(false);
			break;
		}
		this->events.erase(iter);
		lastEventType = event->getType();
		delete event;
		if(job != NULL)
			/* this is only for debug stuff for check the last processed job in case of failure */
			lastProcessedJob = job;
	}
	log->debug("GlobalTime["+to_string(globalTime)+"]"+"Simulation has finished, we generate all the statistics and information for the analysis.",2);
	/*
	double wtime = job->getJobSimWaitTime();
	double rtime = job->getRunTime();
	int cpus = job->getNumberProcessors();
	double response = wtime + rtime;

	double usage = cpus*rtime;
	 */
	 
	//TODO: IMPORTANT - redefine utilization
	double utilization = total_usage*100/((globalTime-sysOrch->getFirstStartTime())*sysOrch->getArchitecture()->getNumberCPUs());
	std::cout << std::endl << std::endl;
	std::cout <<"Total usage: " << total_usage*100 << std::endl;
	std::cout << "globalTime: " << globalTime << std::endl;
	std::cout << "SysOrch startTime: " << sysOrch->getFirstStartTime() << std::endl;
	std::cout << "Number CPUs (sysOrch): " << sysOrch->getArchitecture()->getNumberCPUs() << std::endl;
	
	cout<<endl<<endl<<"Total usage in cpus*s="<<ftos(total_usage)<<" and util percentage is:"<<utilization<<"%"<<endl<<endl;
	this->collectFinalStatistics();
	this->sysOrch->PostProcessScheduling();
	
	//TODO: Use computePower.
	//if (policy->computePower())
		/* print some information about power consumed during workload */
		//policy->dumpGlobalPowerInformation(globalTime, usefull);
	//Delete events objects
	while (!events.empty())
	{
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
bool Simulation::loadSimulation()
{
	return true; //TODO: Return object
}

/**
 * Shows that current statistics concerning the jobs that have been simulated and to the policy. The statistics are shown in the STDOUT.
 */
void Simulation::showCurrentStatistics()
{

	/* first we generate the statistics */
	SimStatistics* stats = this->SimInfo->getsimStatistics();
	TraceFile* workload =  this->SimInfo->getWorkload();
	stats->computeJobMetrics(&workload->JobList);
	stats->computePolicyMetrics(this->policyCollector);
	stats->ShowCurrentStatisticsStd();
	std::cout << "Current global time:"+ ftos(this->globalTime)  << endl;
	if(lastProcessedJob != NULL)
		std::cout << "Last Job treated: "+to_string(lastProcessedJob->getJobNumber())  << endl;
	std::cout << "Current Event:"+getStringEvent(lastEventType) << endl;
	this->firstShow = true;
}

/**
 * Returns the human readable description of an event type.
 * @param type The type for the event.
 * @return The string with the human readable 
 */
string Simulation::getStringEvent(event_t type)
{
	switch(type) {
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
void Simulation::collectFinalStatistics()
{

	if(numberOfFinishedJobs < 1) {
		log->debug("GlobalTime["+to_string(globalTime)+"]"+"No finished jobs. We do not compute the statistics.");
		return;
	}
	SimStatistics* stats = this->SimInfo->getsimStatistics();
	TraceFile* workload =  this->SimInfo->getWorkload();
	/*in case of required we dump the performance for the jobs */
	if(this->JobsSimulationPerformance != NULL)
		this->JobsSimulationPerformance->createJobsCSVInfoFile(&workload->JobList);
	if(this->PolicySimulationPerformance != NULL)
		this->PolicySimulationPerformance->createPolicyCSVInfoFile(this->policyCollector);
	/*  we generate the statistics */
	stats->computeJobMetrics(&workload->JobList);
	stats->computePolicyMetrics(this->policyCollector);
	stats->saveGlobalStatistics();
}

/**
 * Indicates if the simulation should be finished 
 * @return A bool indicating if the simulation should be stopped 
 */
bool Simulation::continueSimulation()
{
	bool continueSimulation = !this->getStopSimulation();
	return continueSimulation;
}


/**
 * Sets the paraver trace for the current simulation
 * @param trace A reference to the paraver trace 
 */
void Simulation::setOutputTrace(ParaverTrace* trace)
{
	//TODO: SysOrch Set Paraver Trace
	//this->policy->setPrvTrace(trace);
}

/**
 * Returns the current globalTime of the simulation 
 * @return An integer with the global time 
 */
double Simulation::getglobalTime() const
{
	return globalTime;
}


/**
 * Sets the globalTime for the simulation 
 * @param theValue The integer containing the global time for the simulation 
 */
void Simulation::setGlobaltime(double theValue)
{
	globalTime = theValue;
}

/**
 * This function is mainly used for debugging or for state that the simulation is performing as expected. It will dump all the pending events to the debug engine.
 */
void Simulation::dumpEvents() 
{
	EventQueue::iterator it = this->events.begin(); // should be optimized !
	for(;it != this->events.end();++it) {
		SimulationEvent* cEvent = *it;
		/* debug stuff */
		if(this->log->getLevel() > 8 && (cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION))
			this->log->debug("GlobalTime["+ftos(globalTime)+"] Event type "+to_string(cEvent->getType())+" with the id "+to_string(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+to_string(cEvent->getJob()->getJobNumber()),2);
	}
}


/**
 * Delete all the events that concerns the provided job. This is used in situations when the job has been rescheduled and the events that was previously generated have lost its validity.
 * @param job The job to whom events have to be deleted 
 */
void Simulation::deleteJobEvents(Job* job) 
{
	vector<SimulationEvent*> toDelete;
	EventQueue::iterator it = this->events.begin(); // should be optimized !

	for(;it != this->events.end();++it){
		SimulationEvent* cEvent = *it;
		/* debug stuff */
		if(this->log->getLevel() > 8 && (cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION))
			this->log->debug("GlobalTime["+to_string(globalTime)+"]"+"Event type "+to_string(cEvent->getType())+" with the id "+to_string(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+to_string(cEvent->getJob()->getJobNumber()),2);
		if(cEvent->getType() != EVENT_COLLECT_STATISTICS  && cEvent->getJob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid()) {
			this->log->debug("GlobalTime["+to_string(globalTime)+"]"+"Deleting the event type "+to_string(cEvent->getType())+" with the id "+to_string(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+to_string(cEvent->getJob()->getJobNumber()),2);
			toDelete.push_back(cEvent);
			continue;
		}
		/* debug stuff */
		if(this->log->getLevel() > 8 && (cEvent->getType() == EVENT_START || cEvent->getType() == EVENT_TERMINATION))
			this->log->debug("GlobalTime["+to_string(globalTime)+"]"+"Event type "+to_string(cEvent->getType())+" with the id "+to_string(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+to_string(cEvent->getJob()->getJobNumber()),2);
	}
	for(vector<SimulationEvent*>::iterator itdel = toDelete.begin(); itdel != toDelete.end();++itdel) {
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
void Simulation::deleteJobFinishEvent(Job* job) 
{
	if(job == this->currentJob && this->currentEvent->getType() == EVENT_ARRIVAL)
		return;
	EventQueue::iterator it = this->events.begin(); // should be optimized !
	for(;it != this->events.end();++it) {
		SimulationEvent* cEvent = *it;
		if( cEvent->getType() != EVENT_COLLECT_STATISTICS  && cEvent->getJob()->getJobNumber() == job->getJobNumber() && currentEvent->getid() != cEvent->getid()
				&& (cEvent->getType() == EVENT_TERMINATION || cEvent->getType() == EVENT_ABNORMAL_TERMINATION)
		) {
			this->log->debug("GlobalTime["+to_string(globalTime)+"]"+"Deleting the event type "+to_string(cEvent->getType())+" with the id "+to_string(cEvent->getid()) +" that is scheduled by "+ftos(cEvent->getTime())+" for the job "+to_string(cEvent->getJob()->getJobNumber()),2);
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
void Simulation::forceJobFinish(double endTime,Job* job)
{
	//debugging stuff
	if(this->log->getLevel() > 8)
		this->dumpEvents();
	/* If the job is the one that is currently processed and it is a start time we do not have to create a finish event
	 * it will be created in function.
	 * Carefull! If the user runtime estimation was accurate (aka: requested runtime < real runtime). We will use the getJobSimFinishTime) instead
	 * Search for note EVENT_TERMINATION_NOTE1 in this same document.
	 */
	if(!(job == this->currentJob && this->currentEvent->getType() == EVENT_START)) {
		log->debug("GlobalTime["+to_string(globalTime)+"]"+"Reescheduling the job "+ to_string(job->getJobNumber())+ ". Adding an EVENT_TERMINATE "+ftos(endTime),2);
		/*we create a job termination for the job .. if it fails, or must be killed*/
		SimulationEvent* finishjob  = NULL;
		double terminationTime = -1;
		//TODO: Use getAllowUserEstimatedRuntimeViolation
		//if(policy->getAllowUserEstimatedRuntimeViolation() ||
		//		(job->getJobSimStartTime()+job->getOriginalRequestedTime()) > endTime //good user estimation :)
		//)
		//	terminationTime = endTime;
		//else
			terminationTime = job->getJobSimStartTime()+job->getOriginalRequestedTime();
		if(job->getStatus() == COMPLETED) {
			finishjob = new SimulationEvent(EVENT_TERMINATION,job,++this->lastEventId,terminationTime);
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_TERMINATION event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
		}
		else {
			finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION,job,++this->lastEventId,terminationTime);
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_ABNORMAL_TERMINATION event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
		}
		this->events.insert(finishjob);
	}
	//debugging stuff
	if(this->log->getLevel() > 8)
		this->dumpEvents();
}

/**
 * Function that generate all the required events for force a job start in the provided time. 
 * @param startTime The start time for the new start event 
 * @param endTime The end time for the new  finialization event 
 * @param job The job to whom finish event has to be created 
 */
void Simulation::forceJobStart(double startTime,double endTime,Job* job)
{  
	//debugging stuff
	if(this->log->getLevel() > 8)
		this->dumpEvents();
	//In case that this job is the once that is being scheduled and the current event is EVENT_ARRIVAL we do not
	//create any EVENT TERMINATION due to this events will be created in the main doSimulation method
	if(!(job == this->currentJob && this->currentEvent->getType() == EVENT_ARRIVAL))
	{
		/*create  event for the job start*/
		SimulationEvent* startjob = new SimulationEvent(EVENT_START,job,++this->lastEventId,startTime);
		/*we create a job termination for the job .. if it fails, or must be killed*/
		SimulationEvent* finishjob  = NULL;
		log->debug("GlobalTime["+to_string(globalTime)+"]"+"Rescheduling the job "+ to_string(job->getJobNumber())+ ". Adding an EVENT_START  the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(startTime),2);
		if(job->getStatus() == COMPLETED) {
			finishjob = new SimulationEvent(EVENT_TERMINATION,job,++this->lastEventId,endTime);
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_TERMINATION event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
		}
		else {
			finishjob = new SimulationEvent(EVENT_ABNORMAL_TERMINATION,job,++this->lastEventId,endTime);
			log->debug("GlobalTime["+to_string(globalTime)+"]"+"Adding an EVENT_ABNORMAL_TERMINATION event for the job "+ to_string(job->getJobNumber())+ " the event id is "+to_string(this->lastEventId)+ " the time for the event is "+ftos(endTime),2);
		}
		this->events.insert(startjob);
		this->events.insert(finishjob);
	}
	//debugging stuff
	if(this->log->getLevel() > 8)
		this->dumpEvents();
}

/**
 * This function updates the performance variables for the job 
 * @param job A reference to the job that has finished 
 */
void Simulation::setJobPerformanceVariables(Job* job)
{
	job->setJobSimWaitTime(job->getJobSimStartTime()-job->getJobSimSubmitTime());
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
void Simulation::showProgress (double current,double total)
{
	double progress = ceil((current/total)*100);
	double items = ceil(totalItems*(current/total));
	string prg_str = ftos(progress);
	if(prg_str.size() == 1)
		prg_str = "  "+prg_str;
	else if(prg_str.size() == 2)
		prg_str = " "+prg_str;
	if(this->firstShow) {
		this->firstShow = false;
		std::cout << "Progress simulation:";
	}
	else {
		std::cout << ("\b\b\b\b\b\b");
		for(int i = 0; i < this->totalItems;i++)
			std::cout << ("\b");
	}
	std::cout.flush();
	std::cout <<  prg_str+" %";
	for(int i = 0;i < this->totalItems; i++) {
		if(i < items)
			std::cout << "*" ;
		else
			std::cout << " " ;
	}
	std::cout << "|";
	std::cout.flush();
}

/**
 * Returns the amount of time between two statistics collection 
 * @return  A double containing the invertal between two statistics collection 
 */
double Simulation::getcollectStatisticsInterval() const
{
	return collectStatisticsInterval;
}


/**
 * Sets the interval of statistics collection 
 * @param theValue The interval of statiscs collection
 */
void Simulation::setCollectStatisticsInterval(double theValue)
{
	collectStatisticsInterval = theValue;
}

/**
 * Invoqued when the statistics event is raised. 
 */
void Simulation::collectStatistics()
{
	this->policyCollector->pullValues(this->globalTime);
}

double Simulation::getArrivalFactor() const
{
	return ArrivalFactor;
}


/**
 * Sets the arrival factor for the jobs 
 * @param theValue The arrival factor
 */
void Simulation::setArrivalFactor(double theValue)
{
	ArrivalFactor = theValue;
}

/**
 * Returns the CVS streamer for the job information
 * @return A reference to the job cvs job infor converter 
 */
CSVJobsInfoConverter* Simulation::getJobsSimulationPerformance() const
{
	return JobsSimulationPerformance;
}


/**
 * Sets the CVS streamer for the job information
 * @param theValue The reference to the converter
 */
void Simulation::setJobsSimulationPerformance(CSVJobsInfoConverter* theValue)
{
	JobsSimulationPerformance = theValue;
}


/**
 * Returns true if the simulation has to be stoped or not 
 * @return A bool indicating if the simulation has to be stoped 
 */
bool Simulation::getStopSimulation() const
{
	return StopSimulation;
}


/**
 * Sets if the simulation has to be stoped or not.
 * @param theValue The bool indicating if to stop the simulation
 */
void Simulation::setStopSimulation(bool theValue)
{
	StopSimulation = theValue;
}

/**
 * Returns true if the progress of the simulation has to be shown in the standard output
 * @return A bool indicating if the progress will be shown in the std out 
 */
bool Simulation::getshowSimulationProgress() const
{
	return showSimulationProgress;
}


/**
 * Sets if the simulation progress has to be shown in the standard output
 * @param theValue Indicates if the progress has to be shown 
 */
void Simulation::setShowSimulationProgress(bool theValue)
{
	showSimulationProgress = theValue;
}

/**
 * Returns the CVS streamer for the policy information
 * @return A reference to the job cvs policy infor converter 
 */
CSVPolicyInfoConverter* Simulation::getPolicySimulationPerformance() const
{
	return PolicySimulationPerformance;
}

/**
 * Sets the CVS streamer for the policy information
 * @param theValue The reference to the converter
 */
void Simulation::setPolicySimulationPerformance(CSVPolicyInfoConverter* theValue)
{
	PolicySimulationPerformance = theValue;
}

void Simulation::initPower(PowerConsumption* powerinf)
{
	//TODO: sysOrch->initPower?
	//policy->initializationPower(powerinf);
}
}
