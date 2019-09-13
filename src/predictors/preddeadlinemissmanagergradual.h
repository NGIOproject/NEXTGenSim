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
#ifndef PREDICTIONMODULEPREDDEADLINEMISSMANAGERGRADUAL_H
#define PREDICTIONMODULEPREDDEADLINEMISSMANAGERGRADUAL_H


#include <predictors/preddeadlinemissmanager.h>

#include <vector>

using namespace std;
using std::vector;

namespace PredictionModule {

/**
@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>	  
*/

/**
* This a pre deadlineMiss manager that behaves as follows 
* If deadline was already missed n times, then the next prediction
* would be to add GRADUAL[n] to the current elapsed (while honoring
* the estimate postcondition.-	
*/
class PredDeadlineMissManagerGradual  : public PredDeadlineMissManager {
public:
  PredDeadlineMissManagerGradual();
  ~PredDeadlineMissManagerGradual();
  
  virtual double deadlineMiss(Job* job, double elapsed, double estimate, int    miss_count);    
  
private:
  vector<double> increment; /**< The factors that will be applied to the prediction */
 
    
};

}

#endif
