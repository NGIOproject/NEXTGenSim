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
#ifndef STATISTICSCOUNTLITERALS_H
#define STATISTICSCOUNTLITERALS_H

#include <statistics/statisticalestimator.h>
#include <utils/utilities.h>

#include <vector>

using namespace std;
using std::vector;

namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* Implements the statistic of CountLiterals- given a set of values returns the count that each value is repeated. To be applied to string vectors. 
* @see the statisticalestimator class
*/
class CountLiterals : public StatisticalEstimator{
public:
  CountLiterals();
  ~CountLiterals();


 virtual Simulator::Metric* computeValue();	
 void setSValues(vector< string >* theValue);
 vector< string >* getSvalues() const;
	
private:
 vector<string>* values; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */

};

}

#endif
