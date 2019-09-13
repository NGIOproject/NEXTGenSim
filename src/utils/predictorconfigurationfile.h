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
#ifndef UTILSPREDICTORCONFIGURATIONFILE_H
#define UTILSPREDICTORCONFIGURATIONFILE_H

#include <string>

#include <predictors/predictor.h>
#include <datamining/classifier.h>
#include <datamining/discretizer.h>

/* libxml2 stuff */
#include <libxml/tree.h>
#include <libxml/parser.h>


using namespace PredictionModule;
using namespace Datamining;
using namespace std;

namespace Utils {

/**
*@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/


/** This class loads a definition of a predictor from a configuration file */
class PredictorConfigurationFile{
public:
  PredictorConfigurationFile(string predictorConfigurationFile);
  ~PredictorConfigurationFile();
  
  void init(); //initialize all the variables of the class 

  string ConfigFile; /**< The file path of the predictor definition */

  /*Common fileds for all the predictors */
  Predictor_type_t predictorType; /**< Indicates the predictor type defined in the configuration file  */
  DeadlineMissManager_t preDeadlineManager; /**< Indicates the type of pre deadline mananager for the predictor */
  DeadlineMissManager_t postDeadlineManager; /**< Indicates the type of the post deadline manager for the predictor */
  
  bool fileLoaded; /**< Indicates if the configuration file has been loaded correctly */
  
  
  xmlDocPtr doc; /**< XML containing the definition for the architecture */ 
 
  
  /* STUFF REGARDING THE HISTORICAL CLASSIFIER PREDICTOR STUFF */
  
  classifier_t classifierType; /**< Defines the type of classifier the is implemented, see classifier_t enum definition.*/ 
  discretizer_t discretizer; /**< Defines the discretization method to be applied to the continuous values */
  int numberOfBins; /**< Defines the number of bins to be used in the discretization method*/  
  
  
  vector<string>*  ResponseJobAttributes; /** The vector containing the jobs variables that will be used for construct the prediction model */
  bool NewFilePerModel; /**< If true, all the models generated for carrying out the prediction will be dump in a separate filed  */
  int ModelIntervalGeneration; /**< A double indicating the amount of job finished between two different model generations  */
  string PredictionVariable; /** The variable to be predicted, it should be the runtime in almost all the cases, however we leave it free just in case the developed would like to use the predictor for any other goal */
  string ModelDirectoryFile; /**< The dir where all the predictor weka files will be dumped  */
  
 
  
private:
  void extractHistoricalClassifierFields(xmlNodePtr classifier);
  classifier_t getClassifierType(string strClassifier);
  discretizer_t getDiscretizerType(string strDiscretizer);
  DeadlineMissManager_t getDeadLineManager(string strManager);

};

}

#endif
