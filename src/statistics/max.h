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
#ifndef STATISTICSMAX_H
#define STATISTICSMAX_H

#include <statistics/statisticalestimator.h>

#include <vector>

using namespace std;
using std::vector;


namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
* Implements the statistic of max- given a set of values returns the maximmum of all the values. Can be applied to vectors of integers or doubles 
* @see the statisticalestimator class
*/
class Max : public StatisticalEstimator {
public:
  Max();
  ~Max();
  
  virtual Simulator::Metric* computeValue();	
  void setValues(vector< double >* theValue);
  void setValues(vector< int >* theValue);
  vector< double >* getvalues() const;
  void setDvalues(vector< double >* theValue);
  vector< double >* getdvalues() const;
  void setIvalues(vector< int >* theValue);
  vector< int >* getivalues() const;
	
	

private:
 vector<double>* dvalues;  /**<  The reference to the vector of doubles that hols all the values to whom the stimator will be computed  */
 vector<int>* ivalues;  /**<  The reference to the vector of integers that hols all the values to whom the stimator will be computed  */

};

}

#endif
