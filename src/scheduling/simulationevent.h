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
#ifndef SIMULATORSIMULATIONEVENT_H
#define SIMULATORSIMULATIONEVENT_H

#include <scheduling/job.h>



namespace Simulator {

    /** Enum that contains all the event types that can be triggered in the simulation */
    enum event_t {
        EVENT_ILLEGAL = 0,
        EVENT_TERMINATION,
        EVENT_ADJUST_PREDICTION, // prediction too short (event may be turned off/on)
        EVENT_ARRIVAL,
        EVENT_START,
        EVENT_ABNORMAL_TERMINATION,
        EVENT_NOSPECIFIC, //using for finding specific events
        EVENT_COLLECT_STATISTICS,
        EVENT_SCHEDULE,
        EVENT_BACKFILL,
        EVENT_TRANSITION_TO_COMPUTE,
        EVENT_TRANSITION_TO_OUTPUT,
        EVENT_OTHER,
    };

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/**
* This class implements a simulation event.
* @see The class simulation
*/
class SimulationEvent{
public:
  SimulationEvent(event_t type,Job* job,int id,double time);
  ~SimulationEvent();
  void setId(const int& theValue);
  int getid() const;
  void setTime(const double& theValue);
  double getTime() const;
  void setType(const event_t& theValue);
  event_t getType() const;
  void setJob(Job* theValue);
  Job* getJob() const;


private:

  Job* job;  /**< to which job the event is associated.. */
  double time; /**< the time associated to event .. when it must be triggered..*/
  int id; /** the id for the event */
  event_t type; /**< which kind of event .*/

  vector<Job*> job_vector; /** vector of jobs in this event */

};

}

#endif
