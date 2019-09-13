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
#include <statistics/interquartiledifference.h>

#include <algorithm>

namespace Statistics {

/**
 * The default constructor for the class
 */
InterquartileDifference::InterquartileDifference()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
InterquartileDifference::~InterquartileDifference()
{
}



/**
 * Function that compute the Interquartile Difference of all the values stored in the vector
 * @return Metric with the IQR
 */
Simulator::Metric* InterquartileDifference::computeValue()
{
  double dvalue;
  
  /* first we sort all the elements of the vector */
  if ( this->values->size() > 0 )
  {
    sort(this->values->begin(), this->values->end());  
    double q1 =*(this->values->begin()+int(this->values->size()*0.25));
    double q2 =*(this->values->begin()+int(this->values->size()*0.75));
    dvalue = q2-q1;
  }
  else
    dvalue = 0;
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::IQR);    
  performance->setNativeValue(dvalue);
  performance->setNativeType(Simulator::DOUBLE);
  
  return performance;
  
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* InterquartileDifference::getvalues() const
{
  return values;
}

/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void InterquartileDifference::setValues(vector< double >* theValue)
{
  values = theValue;
}


}
