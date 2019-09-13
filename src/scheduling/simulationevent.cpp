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
#include <scheduling/simulationevent.h>

namespace Simulator {

/**
 * The default constructor for the job event 
 * @param type The event type 
 * @param job The job associated to the event 
 * @param id The id for event 
 * @param time The time associated to the event 
 */
SimulationEvent::SimulationEvent(event_t type,Job* job,int id,double time)
{
  this->type = type;
  this->job = job;
  this->id = id;
  this->time = time;
}

/**
 * The default destructor for the class 
 */
SimulationEvent::~SimulationEvent()
{
}

/**
 * Returns the id for the simulationevent
 * @return A integer containing the id
 */
int SimulationEvent::getid() const
{
  return id;
}


void SimulationEvent::setId(const int& theValue)
{
  id = theValue;
}

void SimulationEvent::setTime(const double& theValue)
{
  this->time = theValue;
}

/**
 * Returns the time for the event 
 * @return A double containing the Time for the event 
 */

double SimulationEvent::getTime() const
{
  return this->time;
}


/**
 * Returns the event type 
 * @return A event_t containing the type of the event 
 */
event_t SimulationEvent::getType() const
{
  return type;
}


/**
 * Sets the type of he current event 
 * @param theValue The type of the event 
 */
void SimulationEvent::setType(const event_t& theValue)
{
  type = theValue;
}

/**
 * Returns the job related to the event 
 * @return A reference to the job 
 */
Job* SimulationEvent::getJob() const
{
  return job;
}


/**
 * Sets the job related to the event 
 * @param theValue A reference to the event 
 */
void SimulationEvent::setJob(Job* theValue)
{
  job = theValue;
}

}
