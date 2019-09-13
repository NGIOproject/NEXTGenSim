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
#include <predictors/predictor.h>

namespace PredictionModule {

Predictor::Predictor(Log* log)
{
 maxEstimate = -1; 
 this->log = log;
}


Predictor::~Predictor()
{
}

double Predictor::getmaxEstimate() const
{
  return maxEstimate;
}


void Predictor::setMaxEstimate(double theValue)
{
  maxEstimate = theValue;
}


PredDeadlineMissManager* Predictor::getpostManager() const
{
  return postManager;
}

 
void Predictor::setPostManager(PredDeadlineMissManager* theValue)
{
  postManager = theValue;
}


PredDeadlineMissManager* Predictor::getpreManager() const
{
  return preManager;
}


void Predictor::setPreManager(PredDeadlineMissManager* theValue)
{
  preManager = theValue;
}

Predictor_type_t Predictor::getType() const
{
  return type;
}


int Predictor::updateMissedCountJob(Job* job)
{
  map<Job*,int>::iterator jobIt = this->missedDeadlineCount.find(job);

  if(jobIt ==  this->missedDeadlineCount.end())
  {
    this->missedDeadlineCount.insert(pairJobInt(job,1));
    return 0;
  }
  else  
  { 
    assert(missedDeadlineCount[job] > 0);
    int miss_counts = missedDeadlineCount[job];
    this->missedDeadlineCount[job] = miss_counts+1;
    return miss_counts;
  }
}

/**
 * Returns the number of jobs that have to finish for rebuilt the prediction model 
 * @return A integer conatining the amount of jobs to bee finished 
 */
int Predictor::getModelIntervalGeneration() const
{
  return ModelIntervalGeneration;
}


/**
 * Sets the amount of jobs that have to finish for update the prediction model 
 * @param theValue A integer containing the amount of jobs to be finished 
 */
void Predictor::setModelIntervalGeneration(const int& theValue)
{
  ModelIntervalGeneration = theValue;
}



}
