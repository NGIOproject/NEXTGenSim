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
#include <scheduling/reservationtable.h>

using namespace Utils;

namespace Simulator {

/***************************
  Constructor and destructors
***************************/

/**
 * @param log A reference to the logging engine 
 * @param globalTime The globalTime of the simulation
 */
ReservationTable::ReservationTable(Log* log, uint64_t globalTime)
{
  this->log = log;
  this->globalTime = globalTime;
  this->initialGlobalTime = globalTime;
}

/**
 * The default constructor for the class
 */
ReservationTable::ReservationTable()
{

}

/**
 * The default destructor for the class 
 */
ReservationTable::~ReservationTable()
{
}

/***************************
  Sets and gets
***************************/

/**
 * Returns the allocation of a given job 
 * @param job The job to whom the allocation is required
 * @return The allocation of the job
 */
ResourceJobAllocation* ReservationTable::getJobAllocation(Job* job)
{
  map<Job*,ResourceJobAllocation*>::iterator iter = this->JobAllocationsMapping.find(job);
  
  ResourceJobAllocation* allocation = iter->second;
  
  return allocation;
}
 

/**
 * Returns the globalTime
 * @return A double containing the globalTime
 */
double ReservationTable::getGlobalTime() const
{
  return globalTime;
}


/**
 * Sets the globalTime to the ReservationTable
 * @param theValue The globalTime
 */
void ReservationTable::setGlobalTime(double theValue)
{
  globalTime = theValue;
}

/**
 * Returns the initialGlobalTime
 * @return A double containing the initialGlobalTime
 */
double ReservationTable::getinitialGlobalTime() const
{
  return initialGlobalTime;
}


/**
 * Sets the initialGlobalTime to the ReservationTable
 * @param theValue The initialGlobalTime
 */
void ReservationTable::setInitialGlobalTime(double theValue)
{
  initialGlobalTime = theValue;
}

/**
 * Returns the log
 * @return A reference to the logging engine
 */
Log* ReservationTable::getlog() const
{
  return log;
}

/**
 * Sets the Log to the ReservationTable
 * @param theValue The reference to the logging engine
 */
void ReservationTable::setLog(Log* theValue)
{
  log = theValue;
}

}
