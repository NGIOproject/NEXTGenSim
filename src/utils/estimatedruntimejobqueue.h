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
#ifndef UTILSESTIMATEDRUNTIMEJOBQUEUE_H
#define UTILSESTIMATEDRUNTIMEJOBQUEUE_H

#include <scheduling/job.h>
#include <utils/jobqueue.h>

#include <assert.h>
#include <set>

#define ESTIMATED_RT_JOB_Qt 1

using namespace std;
using std::set;

using namespace Simulator;

namespace Utils {

/**< Comparation operation of two jobs based on it's submit time */
struct estimatedRT_lt_t {
    bool operator() (Job* job1, Job* job2) const {
	
	double x = job1->getRequestedTime();
	double y = job2->getRequestedTime();
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return job1->getJobNumber() < job2->getJobNumber();
    }
};


/**
*  @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Set based on the requested time  */
typedef set<Job*, estimatedRT_lt_t> EstRTQueue;

/** 
*  This class implements a job queue where the order of the queue is based on the required runtime for the job.
*/
class EstimatedRunTimeJobQueue : public JobQueue{
public:
  EstimatedRunTimeJobQueue();
  ~EstimatedRunTimeJobQueue();  
  
  /* Auxiliar functions and main functions */
  virtual void insert(Job* job);
  virtual void erase(Job* job);
  virtual Job* headJob(); 
  virtual Job* next(); //Function used for getting the next job in the current position, if null means that there are no more jobs, the jobs will be returned following the FCFS criteria
  virtual Job* begin();
  virtual void deleteCurrent();
  virtual bool contains(Job* job);

  
  EstRTQueue* getQueue();  

private:
  EstRTQueue queue; /**< Contains the queue of jobs where they are sorted in the order defined in the comparaison operator */
  EstRTQueue::iterator currentIterator; /**< The iterator that points to the current position that is being queried to the queue */

};

}

#endif
