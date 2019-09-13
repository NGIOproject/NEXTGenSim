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
#ifndef DATAMININGCLASSIFIER_H
#define DATAMININGCLASSIFIER_H

#include <scheduling/metric.h>
#include <utils/log.h>
#include <datamining/discretizer.h>


#include <vector>
#include <string>

using namespace Utils;
using namespace Simulator;
using namespace std;
using std::vector;

/** Defines the different set of classifiers that are currently defined in the simulator */
enum classifier_t 
{
   C45 = 0,/** An J48 tree classifier  */
   ID3, /** An ID3 tree classifier */
   OTHER_CLASSIFIER, /** Another classifier, should not be used */
};


namespace Datamining {

/**
@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class defines the general interfaces that a generic datamining classifier should provide. */
class Classifier{
public:
 Classifier(vector<string>* attributes, vector<native_t>* attributesTypes,Log* log,discretizer_t discretizer,int numberOfBins);
 ~Classifier();

  void setClassifierType(const classifier_t& theValue);
  classifier_t getclassifierType() const;
  string getClassifierName(classifier_t classifier);
  
  /**
   * Adds a new instance to the data model. This is a knowledge of an known attribute classification.
   * @param Instances The values foreach of the attributes.
   * @return A bool indicating if the instance has been added to the current model or not.
   */
  virtual bool addInstance(vector<string>* Instances) = 0;
  
  /**
   * Given a set of response attribute values for a given instance, the model will classify it to a given class 
   * @param ResponseAttributes 
   * @return An string containing the classification 
   */
   virtual string classify(vector<string>* ResponseAttributes) = 0;
	
   /**
    * Given all the instances that have been added the data base stored in the class, the new model will be created.
    * @return A bool indicating if the model has been correctly classified.
    */
   virtual bool generateModel() = 0;
    
protected:
   classifier_t classifierType; /**< Defines the type of classifier the is implemented, see classifier_t enum definition.*/ 
   discretizer_t discretizer; /**< Defines the discretization method to be applied to the continuous values */
   int numberOfBins; /**< Defines the number of bins to be used in the discretization method*/  
      
   vector<string>* attributes; /**< Defines the attributes names  that will be used for constructring the modules */
   vector<native_t>* attributesTypes; /**< Defines the attribute types for each of the attributes provided */   
   
   Log* log; /**< A reference to the loggin engine */
};

}

#endif
