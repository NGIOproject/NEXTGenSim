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
#include <scheduling/joballocation.h>

namespace Simulator {


/**
 * The default constructor for the class
 */
 
JobAllocation::JobAllocation()
{
  this->startTime = -1;
  this->endTime = -1;
  this->allocationProblem = false;
  this->allocationReason = NOPROBLEM;
  this->realAllocation = false;
}

/**
 * The constructor for the class that will be more commontly used.
 * @param startTime The startime for the allocation 
 * @param endTime The endTime for the allocation
 */
JobAllocation::JobAllocation(double startTime, double endTime)
{
  this->startTime = startTime;
  this->endTime = endTime;
  this->allocationProblem = false;
  this->allocationReason = NOPROBLEM;
  this->realAllocation = false;
}

/**
 * The default destructor for the class 
 */
JobAllocation::~JobAllocation()
{
}

/**
 * Returns the endTime
 * @return A double containing the endTime
 */
double JobAllocation::getEndTime() const
{
  return endTime;
}


/**
 * Sets the endTime for the allocation 
 * @param theValue A double endTime for the job 
 */
void JobAllocation::setEndTime(double end)
{
	endTime = end;
}


/**
 * Returns if the allocation is done in the reservation table 
 * @return Bool that indicates if its real 
 */
bool JobAllocation::getrealAllocation() const
{
  return realAllocation; 
}


/**
 * Sets if the current allocation is real 
 * @param theValue If the allocation is real 
 */
void JobAllocation::setRealAllocation(bool theValue)
{
  realAllocation = theValue;  
}


/**
 * Returns the startTime 
 * @return A double containing the startTime
 */
double JobAllocation::getStartTime() const
{
  return startTime;
}


/**
 * Sets the start time for the allocation 
 * @param theValue The startime for the allocation 
 */
void JobAllocation::setStartTime(double start)
{
  startTime = start;
}


/**
 * Returns the allocation problem reason (if not problem in will be NOPROBLEM)
 * @return Returns a allocation_problem_t with the allocation problem
 */
allocation_problem_t JobAllocation::getallocationReason() const
{
  return allocationReason;
}


/**
 * Sets the allocation problem reason 
 * @param theValue The allocation problem
 */
void JobAllocation::setAllocationReason(const allocation_problem_t& theValue)
{
  allocationReason = theValue;
}

/**
 * Returns if there have been occurred any problem during the allocation 
 * @return A bool indicating if the allocation has any problem
 */
bool JobAllocation::getAllocationProblem() const
{
  return allocationProblem;
}

/**
 * Sets if there has been any allocation problem 
 * @param theValue If there has been occured any problem 
 */
void JobAllocation::setAllocationProblem(bool theValue)
{
  allocationProblem = theValue;
}

void JobAllocation::setAllocationLack(ResourceRequirements rrl)
{
	this->lack = rrl;
}

ResourceRequirements JobAllocation::getAllocationLack()
{
	return lack;
}

}
