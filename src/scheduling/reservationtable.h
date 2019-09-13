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
#ifndef SIMULATORRESERVATIONTABLE_H
#define SIMULATORRESERVATIONTABLE_H

#include <utils/architectureconfiguration.h>
#include <scheduling/job.h>
#include <utils/log.h>
#include <scheduling/resourcejoballocation.h>

#include <vector>
#include <map>

using std::vector;
using std::map;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/**
* This class contains information about how the jobs are mapped into the architecture this should be used by the scheduling policy for allocating jobs.
*/
class ReservationTable{
public:

  
  /* Constructors and destructors*/
  ReservationTable();
  ReservationTable(Utils::Log* log,uint64_t globalTime);  
  virtual ~ReservationTable();
  
  /* Auxiliar methods and main */
  //
  //
  /**
   * Allocate a job the job to the architecture to a given set of processors at a given time be aware that no collision must exist in this allocation ! this must be controlled by the scheduler.
   * @param job The job to be allocated 
   * @param allocation The allocation 
   * @return True if no error has raised, false otherwise
   */
  virtual bool allocateJob(Job* job, ResourceJobAllocation* allocation) = 0; 
  
  /**
   * deallocate a job the job to the architecture
   * @param job The job to be deallocated    
   * @return True if no error has raised, false otherwise
   */
  virtual bool deallocateJob(Job* job) = 0; //DeAllocate a job the job to the architecture based on the criteria  
  /**
   * Advises to the reservation table that a given job has been killed 
   * @param job A reference to the killed job 
   * @return True if no error has raised, false otherwise
   */
  virtual bool killJob(Job* job) = 0;
  /**
   * Returns the number of processors currently used in the reservation table at the current time 
   * @return An unsigned integer containing the number of cpus
   */
  virtual uint32_t  getNumberCPUsUsed() = 0;
  
  /* returns the previous done allocation for the given*/
  ResourceJobAllocation* getJobAllocation(Job* job);
  void setGlobalTime(double theValue);
  double getGlobalTime() const;
  void setInitialGlobalTime(double theValue);
  double getinitialGlobalTime() const;
  void setLog(Utils::Log* theValue);
  Utils::Log* getlog() const;
  
protected:

  Utils::Log* log; /**< A reference to the logging engine */
  uint64_t globalTime; /**<  contains the current global time */
  double initialGlobalTime; /**< contains the initial time that the reservation table starts */
  map<Job*,ResourceJobAllocation*> JobAllocationsMapping; /**< mapping the buckets that are associated to the job -  the hash map key corresponds to the id of the joband the content corresponds to the allocation for the job */
  
  
};

}

#endif
