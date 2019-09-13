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
#ifndef PREDICTIONMODULEPREDDEADLINEMISSMANAGEREXP_H
#define PREDICTIONMODULEPREDDEADLINEMISSMANAGEREXP_H

#include <predictors/preddeadlinemissmanager.h>

namespace PredictionModule {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** A preddeadlinemissmanager that increments the prediction in a exponential factor*/
class PredDeadlineMissManagerExp : public PredDeadlineMissManager{
public:
  PredDeadlineMissManagerExp();
  PredDeadlineMissManagerExp(double factor);
  ~PredDeadlineMissManagerExp();
  
  virtual double deadlineMiss(Job* job, double elapsed, double estimate, int    miss_count);    

private:
  double factor; /**< the exponent factor (>1)*/

};

}

#endif
