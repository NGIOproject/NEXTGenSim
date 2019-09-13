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
#include <statistics/max.h>

#include <math.h>

namespace Statistics {

/**
 * The default constructor for the class
 */
Max::Max()
{
  this->dvalues = NULL;
  this->ivalues = NULL;
}


/**
 * The default destructor for the class 
 */
Max::~Max()
{
}


/**
 * Returns a reference to the vector of doubles (if set) that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector of integers 
 */
vector< double >* Max::getdvalues() const
{
  return dvalues;
}


/**
 * Sets the reference to the vector of doubles that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector of doubles 
 */
void Max::setDvalues(vector< double >* theValue)
{
  dvalues = theValue;
}


/**
 * Returns a reference to the vector of integers (if set) that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< int >* Max::getivalues() const
{
  return ivalues;
}


/**
 * Sets the reference to the vector of integersthat holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector of integers 
 */
void Max::setIvalues(vector< int >* theValue)
{
  ivalues = theValue;
}

/**
 * Function that compute the maximum of all the values stored in the vector
 * @return Metric with the maximum
 */
Simulator::Metric* Max::computeValue()
{
  double maximum = 0;

  if(this->ivalues != NULL)
    for(vector<int>::iterator it = this->ivalues->begin();it != this->ivalues->end();++it)
    {
      maximum = max((double)*it,maximum);      
    }
  else
    for(vector<double>::iterator it = this->dvalues->begin();it != this->dvalues->end();++it)
    {
      maximum = max((double)*it,maximum);          
    }
       
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::MAX);    
  performance->setNativeType(Simulator::DOUBLE);
  performance->setNativeValue(maximum);
 
  return performance;
}

}
