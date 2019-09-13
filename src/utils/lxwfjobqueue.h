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
#ifndef UTILSLXWFJOBQUEUE_H
#define UTILSLXWFJOBQUEUE_H

#include <assert.h>
#include <set>


#define FCFS_LXWF_Qt 3

#include <utils/jobqueue.h>

using namespace std;
using std::set;

using namespace Simulator;

namespace Utils {


/**< Comparation operation of two jobs based on it's submit time - surely it has to be tested in more detail. Mainly due to the order of jobs changes during the time .. so it should be recomputed each tim */

struct LXWF_lt_t {
    bool operator() (Job* job1, Job* job2) const {
	
	double x = job1->getJobSimFinishTime();
	double y = job2->getJobSimFinishTime();
	assert( x >= 0 && y >= 0 );

	if( x != y )
	    return x < y;
	else
	   return job1->getJobNumber() < job2->getJobNumber();
    }
};


typedef set<Job*, LXWF_lt_t> LXWFQueue;

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**   This class implements a job queue where the order of the queue is based on the LXWF priority .*/
class LXWFJobQueue : public JobQueue{
public:
  LXWFJobQueue();
  ~LXWFJobQueue();

  /* Auxiliar functions and main functions */
  virtual void insert(Job* job);
  virtual void erase(Job* job);
  virtual Job* headJob();    
  virtual Job* next(); 
  virtual Job* begin();
  virtual void deleteCurrent();   
  virtual bool contains(Job* job);
  
private:
  LXWFQueue queue; /**< Contains the queue of jobs where they are sorted in the order defined in the comparaison operator */
  LXWFQueue::iterator currentIterator;/**< The iterator that points to the current position that is being queried to the queue */
};

}

#endif
