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
#ifndef STATISTICSACCUMULATELITERALS_H
#define STATISTICSACCUMULATELITERALS_H


#include <statistics/statisticalestimator.h>
#include <map>
#include <vector>

using namespace Simulator;
using namespace std;
using std::vector;
using std::map;

namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>*/

/**
* Implements the statistic of Accumulate- given a set of pairs -string,double- returns the adding.group by the string 
* @see the statisticalestimator class
*/
class AccumulateLiterals : public StatisticalEstimator {
public:
  AccumulateLiterals();
  ~AccumulateLiterals();

 virtual Simulator::Metric* computeValue();	
 void setSPValues(map<string,vector<double>*>* theValue);
 map<string,vector<double>*>* getSPvalues() const;
	
private:
 map<string,vector<double>*>* valuesSP; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */
 statistic_t accumulateToCompute; /**<  The type of statisticalestimator that will be applied per group */
 
};

}

#endif
