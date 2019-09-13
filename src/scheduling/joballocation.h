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
#ifndef SIMULATORJOBALLOCATION_H
#define SIMULATORJOBALLOCATION_H

#include <vector>
#include <deque>
#include <climits>
#include "resourcebucket.h"

using namespace std; 
using std::vector;

namespace Simulator {

/** This enum contains all the problems that can be generated when trying to find an allocation */
enum allocation_problem_t {
    NOT_ENOUGH_RESOURCES = 0,
    NOT_ENOUGH_CPUS,
    JOBS_COLLISION,
    NOPROBLEM,
};

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements an job allocation for the job. */
class JobAllocation{
public:
  JobAllocation();
  JobAllocation(double startTime, double endTime);
  virtual ~JobAllocation();
  void setEndTime(double endTime);
  double getEndTime() const;
  virtual void setRealAllocation(bool theValue);
  bool getrealAllocation() const;
  void setStartTime(double startTime);
  double getStartTime() const;
  void setAllocationReason(const allocation_problem_t& theValue);
  allocation_problem_t getallocationReason() const;	
  void setAllocationProblem(bool theValue);
  bool getAllocationProblem() const;	
  void setAllocationLack(ResourceRequirements rrl);
  ResourceRequirements getAllocationLack();

protected:
  /* the start and end time for the allocation 
     in case that the buckets have been already cut 
     their startTime and endTime will be same as the following ones..
     if not may differ. Once the job is permanently allocated this 
     values should be the same
  */
  double startTime; /**< The startime for the job allocation */
  double endTime; /**< The endTime for the job allocation*/
  /* if the allocation is really done. If not this can be a offer from the 
     reservation to the schdeuling algorithm .. so no definitive
  */
  bool realAllocation; /**< Indicates if the allocation is a real allocation. A real allocation is an allocation that is currently done in the reservation table. In this situation the memory management for the allocation is done by the reservation table */

  /*the following two variables indicates if the allocation has been carried out without problems
    or not, in case there have been problems, for example there is no allocation that satisfies the 
    required criteria the second variable indicates the reason
   */
  bool allocationProblem; /**< If true, the allocation variables are invalid, since it has occer some problem when the allocation has been generated. In this case the realAllocation will be false */
  allocation_problem_t allocationReason; /**< If allocationProblem its true, this variable will indicate the problem that has been raised in the allocation search*/
  
  ResourceRequirements lack;

};

}

#endif
