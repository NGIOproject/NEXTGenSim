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
#include <predictors/userruntimeestimatepred.h>
#include <predictors/preddeadlinemissmanagergradual.h>
#include <assert.h>

namespace PredictionModule {

/**
 * The default constructor, the class creates ad Gradual deadline post manager and a Gradual deadline pre manager 
 */
UserRuntimeEstimatePred::UserRuntimeEstimatePred(Log* log) : Predictor(log)
{
  this->type = USER_RUNTIME_ESTIMATE;
  //by default it has a exp pred deadline exp manager 
  this->preManager = new PredDeadlineMissManagerGradual();
  this->postManager = new PredDeadlineMissManagerGradual();

}

/**
 * The destructor for the class
 */
UserRuntimeEstimatePred::~UserRuntimeEstimatePred()
{
	delete preManager;
	delete postManager;
}

  /**
   * Invoked immediately upon the job's arrival, when it is placed in the waitqueue. `pred_v' will contain at least one runtime prediction for the given job.
   * @param job The job that has arrived
   * @return The prediction for the job runtime 
   */
Prediction* UserRuntimeEstimatePred::jobArrived(Job* job)
{
  //we simply return the requested runtime of the job 
  return this->estimateJobPerformance(job);
}

   /**
   * This function can be used for estimate the performance variable, however the job is not being queued necesarily.
   * @param job The job that has to predicted
   * @return The prediction for the job runtime 
   */
Prediction* UserRuntimeEstimatePred::estimateJobPerformance(Job* job) 
{
  //we simply return the requested runtime of the job 
  Prediction* pred = new Prediction(job->getOriginalRequestedTime(),job);
  return pred;
}

  /**
   * Invoked when a job is scheduled to run, i.e. when it is moved from thewaitqueue to the runqueue.
   * @param job The job that has started
   * @param time The startime for the job 
   */
void UserRuntimeEstimatePred::jobStarted(Job* job, double time)
{
  //nothing to do 
}
  /**
   * The given RUNNING job has just exceeded its prediction (i.e. the previous
   * prediction was too short).
   * Produce a new prediction to this job (and possibly to other jobs which
   * are effected) using the appropriate missed-deadlines manager ("pre" if
   * the current missed prediction is smaller than the user estimate; "post"
   * otherwise).  
   * Need to generate a new (longer) estimate at least for this job, and
   * possibly to others. This means that after this function is finished,
   * pred_v will contain at least one Pred_t associated with job_id.
   * 
   * @param job The job that has reached the deadline 
   * @param time The time when the deadline has been reached 
   * @return 
   */
Prediction* UserRuntimeEstimatePred::jobDeadlineMissed(Job* job, double     time)
{
  //first we increment the deadline misses for this job     
  double elapsed = time-job->getJobSimStartTime();
  double uest = job->getOriginalRequestedTime();
  int cnt = this->updateMissedCountJob(job);
  
  double new_prdct = postManager->deadlineMiss(job,elapsed,uest,cnt);
  
  return new Prediction(new_prdct,job);
  
}

  /**
   * 
   * The given RUNNING job has just terminated. This information may (or may
   * not) help in producing more accurate predictions for currently waiting or
   * running jobs. The returned prev_v may be empty (which means that this
   * Predictor_t has decided not to change any previous predictions).
   *   
   * @param job The job that has terminated 
   * @param time The time 
   * @param completed If it has completed or not 
   * @return if not null means that no prediction has been changed 
   * TODO: Return object
   */
Prediction* UserRuntimeEstimatePred::jobTerminated(Job* job, double     time, bool       completed)
{
  Prediction* TODOReturn = 0;
  return TODOReturn;
  //nothing to do
}

}
