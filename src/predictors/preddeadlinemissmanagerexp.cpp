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
#include <predictors/preddeadlinemissmanagerexp.h>

#include <algorithm>
#include <math.h>
#include <assert.h>

using namespace std;

namespace PredictionModule {

/**
 * The default constructor for the pre missmanager 
 */
PredDeadlineMissManagerExp::PredDeadlineMissManagerExp()
{
}

/**
 * The constructor of the deadlineMiss pre manager 
 * @param factor The factor that will be applied to the past estimation of the jobs for return the new one 
 */
PredDeadlineMissManagerExp::PredDeadlineMissManagerExp(double factor)
{
  this->factor = factor;
  assert(factor>1);
}

/**
 * The destructor for the premanager 
 */
PredDeadlineMissManagerExp::~PredDeadlineMissManagerExp()
{
}

/**
 * 
 * @param job The job about who concerns the estimation
 * @param elapsed The elapsed time for the job
 * @param estimate The estimation for the job 
 * @param miss_count The times that the estimation for the job has been wrong
 * @return The new prediction for the job runtime 
 */
double PredDeadlineMissManagerExp::deadlineMiss(Job* job, double elapsed, double estimate, int  miss_count)
{     
  preCond (job, elapsed, estimate, miss_count);
 
  double pred;
	
  if( elapsed > 1.0)
    pred = pow( elapsed, this->factor );
  else if(elapsed == 1.0)
    pred = 2.0;
  else 
  {
    assert(elapsed<1.0);
    pred = 1.0;
  }
	    
  double ret  = elapsed<estimate ? min(pred,estimate) : pred;

  assert( ret > elapsed);
	
  postCond(job, elapsed, estimate, miss_count, ret);
   
  return ret;
}


}
