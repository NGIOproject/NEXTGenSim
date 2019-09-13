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
#ifndef PREDICTIONMODULEHISTORICALIFIER_H
#define PREDICTIONMODULEHISTORICALIFIER_H

#include <datamining/wekaclassifier.h> 
#include <predictors/predictor.h>
#include <utils/log.h>
#include <assert.h>

using namespace Datamining;

namespace PredictionModule {

/**
*	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements a prediction techniques that is based on classification techniques and discretization techniques */
class HistoricalClassifier : public Predictor{
public:
  HistoricalClassifier(classifier_t classifierType,discretizer_t discretizer,int numberOfBins,Log* log,vector<string>* ResponseJobAtts,vector<native_t>* ResponseJobAttsType,string ModelDirectoryFile,bool NewFilePerModel,int ModelIntervalGeneration, string PredictionVariable, native_t PredictionVariableType);
  ~HistoricalClassifier();
  
  virtual Prediction* jobArrived(Job* job) ;
  virtual void jobStarted(Job* job, double time);
  virtual Prediction* jobDeadlineMissed(Job* job, double     time);
  virtual Prediction* jobTerminated(Job* job, double     time, bool       completed);
  virtual Prediction* estimateJobPerformance(Job* job);
  
  void setNoInformationPrediction(double theValue);
  double getNoInformationPrediction() const;	
  
  void setWekaEnv(string JavaBinary,string classpath,string wekaPath,string TemporaryDir);    
	
  
private:
  classifier_t classifierType; /**< Defines the type of classifier the is implemented, see classifier_t enum definition.*/ 
  discretizer_t discretizer; /**< Defines the discretization method to be applied to the continuous values */
  int numberOfBins; /**< Defines the number of bins to be used in the discretization method*/  

  WekaClassifier* classifier; /** The classifier that is used for the prediction */
  vector<string>*  ResponseJobAttributes; /** The vector containing the jobs variables that will be used for construct the prediction model */
  vector<native_t>* ResponseJobAttributesType; /** The string containing the types for each of the attributes of the ResponseJobAttributes */
  bool NewFilePerModel; /**< If true, all the models generated for carrying out the prediction will be dump in a separate filed  */
  
  native_t PredictionVariableType; /**< Indicates the type for the prediction variable */
  string PredictionVariable; /**< The string containing the prediction  */
  
  string ModelDirectoryFile; /**< The dir where all the predictor weka files will be dumped  */
  
  int NumberOfInstances; /**< Indicates the number of instances that are currently inserted in the model */
  double ModelInitialized; /**< Indicates if the model has been initialized or not */
  double NoInformationPrediction; /**< Contains the basic prediction, this will be returned in case that the model has not been created */
  
  double NumberOfLastInstances; /** Indicates the number of instances added since the last model generation */
  
};

}

#endif
