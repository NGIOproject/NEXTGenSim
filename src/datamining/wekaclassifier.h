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
#ifndef DATAMININGWEKACLASSIFIER_H
#define DATAMININGWEKACLASSIFIER_H

#include <datamining/classifier.h>
#include <datamining/wekaobject.h>


namespace Datamining {

/**
@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements a datamining classifier based on the interfaces defined in the Classifier type */
class WekaClassifier: public Classifier,WekaObject{
public:
  WekaClassifier(vector<string>* attributes, vector<native_t>* attributesTypes,string ModelFilePath,bool NewFilePerModel,classifier_t type,Log* log,discretizer_t discretizer,int numberOfBins);
  virtual ~WekaClassifier();
  
  
  virtual bool addInstance(vector<string>* Instances);
  virtual string classify(vector<string>* ResponseAttributes);
  virtual bool generateModel();
  
private:
  void discretizeInputFile(string NoDiscretized,string CurrentArff,vector<string>* attributesToDiscretize);
  
protected:

  string ModelFilePath; /**< This string indicates in which directory the models will be output */
  bool NewFilePerModel; /**< This bool indicates if each time that a model is generated it has to be stored in a new file */
  int LastModelId ; /**< Indicates the last id for the last model created, mainly used for the file name generation */
  string ModelNamePrefix ; /**< Indicates the prefix for the model files names */
  string InputFileName; /**< Indicates the prefix for the input file for weka that will be used for model the files */ 
 
  string CurrentModel; /**< Indicates the current model file */
  string CurrentArff; /**< Indicates the curren arff used in the last model created */ 

  InstancesSet* theSetOfInstances; /**< Contains the set of instances that will be used for create the model  */
  
  int numberOfInstances; /**< Indicates the number of instances that have been added to the current model */

  string WekaModelGenerationClass; /**< Indicates the java class that implements the classifier generation model - will be setted up by the subclasses */
  string WekaModelGenerationParameters; /** Indicates the parameters that have to be provided to the given classifier  that generates the model - will be setted up by the subclasses */
  
  string WekaClassificationClass ; /**< Indicates the java weka class that implements the classification method */
  string WekaClassificationParameters; /**< Indicates the parametrs that have to be provided for predicting the instance */
  
  vector<string> AttributesInterval; /**< This vector contains the string with all the intervals about the attribute, so in this way we will be able to construct the approapiate interval when building up de vector with the input file for the prediction. Thereby, given an real input we will be able to return the approapiate interval */

  string LastPredictedClasse; /**< In some situations the weka stuff is not able to provide a good prediction, in these situation we will provide the previous prediction */
  int countLastPredictedClassUsed; /**< Indicates the amount of times that the last prediction techniques was used */
};

}

#endif
