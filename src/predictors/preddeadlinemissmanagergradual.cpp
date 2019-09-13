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
#include <predictors/preddeadlinemissmanagergradual.h>

#include <algorithm>
#include <math.h>
#include <assert.h>

namespace PredictionModule {


/**
 * The default constructor for the pre missmanager 
 */
PredDeadlineMissManagerGradual::PredDeadlineMissManagerGradual()
{

  static const int MIN  = 60;
  static const int HOUR = 60*60;

  increment.push_back(1*MIN);
  increment.push_back(5*MIN);
  increment.push_back(15 * MIN);
  increment.push_back(30 * MIN);
  increment.push_back(1 * HOUR);
  increment.push_back(2 * HOUR);
  increment.push_back(5 * HOUR);
  increment.push_back(10 * HOUR);
  increment.push_back(20 * HOUR);
  increment.push_back(50 * HOUR);
  increment.push_back(100 * HOUR);
  increment.push_back(200 * HOUR);
  increment.push_back(500 * HOUR);
  increment.push_back(1000 * HOUR);
};


/**
 * The destructor for the premanager 
 */
PredDeadlineMissManagerGradual::~PredDeadlineMissManagerGradual()
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
double PredDeadlineMissManagerGradual::deadlineMiss(Job* job, double elapsed, double estimate, int  miss_count)
{     
  preCond (job, elapsed, estimate, miss_count);
  
  int    idx  = min((int)increment.size()-1, miss_count);
  double pred = elapsed + increment[idx];
  double ret  = elapsed < estimate ? min(pred,estimate) : pred;
		
  postCond(job, elapsed, estimate, miss_count, ret);
   
  return ret;
}

}
