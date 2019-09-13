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
#include <predictors/prediction.h>
#include <assert.h>

namespace PredictionModule {

/**
 * The constructor for the prediction
 * @param prediction The prediction
 * @param job The job to whom concerns the prediction 
 */
Prediction::Prediction(double prediction,Job* job)
{
  this->prediction = prediction;
  this->job = job;
}


/**
 * The destructor for the class
 */
Prediction::~Prediction()
{
}

/**
 * Returns the job to who concers the prediction
 * @return A reference to the job 
 */
Job* Prediction::getJob() const
{
  return job;
}


/**
 * Setts the job to whom concerst the prediction
 * @param theValue A reference to the job
 */
void Prediction::setJob(Job* theValue)
{
  job = theValue;
}


/**
 * Returns the prediction
 * @return A double with the prediction 
 */
double Prediction::getprediction() const
{
  return prediction;
}


/**
 * Sets the prediction for the job 
 * @param theValue A double containing the prediction 
 */
void Prediction::setPrediction(double theValue)
{
  prediction = theValue;
}

}
