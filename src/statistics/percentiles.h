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
#ifndef STATISTICSPERCENTILES_H
#define STATISTICSPERCENTILES_H

#include <statistics/statisticalestimator.h>

#include <algorithm>
#include <math.h>
#include <vector>

using namespace std;
using std::vector;

namespace Statistics  {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* Implements the statistic of Percentiles- given a set values returns the percentiles from [0..100] incrementing by interval value. For example if the interval is 20: P(20)=2;P(40)=45;...P(100) =12232
* @see the statisticalestimator class
*/
class Percentiles : public StatisticalEstimator{
public:
  Percentiles();
  ~Percentiles();
  
  virtual Simulator::Metric* computeValue();	
  void setValues(vector< double >* theValue);
  vector< double >* getvalues() const;
	
private:
  vector<double>* values; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */
  double interval; /**< Indicates the lenght of each percentile to be computed */
  
};

}


#endif
