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
#ifndef DATAMININGDISCRETIZER_H
#define DATAMININGDISCRETIZER_H

#include <scheduling/metric.h>
#include <utils/log.h>


#include <vector>
#include <string>

using namespace std;
using namespace Simulator;
using namespace Utils;
using std::vector;

enum discretizer_t 
{
   SAME_INTERVAL_DISCRETIZER = 0,/** Discretize a given set of real attribues to nominal attributes in intervals of same lenght  */
   SAME_INSTANCES_PER_INTERVAL_DISCRETIZER, /** Discretize a given set of real attribues to nominal attributes in intervals with the same number of instances */
   OTHER_DISCRETIZER, /** Another discretizer, should not be used */
};

namespace Datamining {

/**
@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/


/** This class defines the general interfaces that a generic datamining discretizer should provide. */
class Discretizer{
public:
  Discretizer(Log* log);
  ~Discretizer();
  
  void setDiscretizerType(const discretizer_t& theValue);
  discretizer_t getdiscretizerType() const;
  string getDiscretizerName(discretizer_t classifier);
   
  
  /**
   * This is the minimum operation that a given discretizer must implement. That is, given a set of values of a given attribute the discretizer should return a set of nominal values containing the discretization values for the 
   * @param input The vector with the real values to discretize 
   * @return A vector containing the nominal values with the discretization
   */
  virtual vector<string>* discretize(vector<string>* input) = 0;
    
protected:
   discretizer_t discretizerType; /**< Defines the type of classifier the is implemented, see classifier_t enum definition.*/    
   Log* log; /**< A reference to the loggin engine */    
    

};

}

#endif
