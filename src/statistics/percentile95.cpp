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
#include <statistics/percentile95.h>

#include <algorithm>

namespace Statistics {

/**
 * The default constructor for the class
 */
Percentile95::Percentile95()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
Percentile95::~Percentile95()
{
}

/**
 * Function that computes the 95th Percentile of all the values stored in the vector
 * @return Metric with the 95Percentile 
 */
Simulator::Metric* Percentile95::computeValue()
{
	double dvalue = 0;

	Simulator::Metric* performance = new Simulator::Metric();
	performance->setStatisticUsed(Simulator::PERCENTILE95);
	performance->setNativeType(Simulator::DOUBLE);
	if ( this->values->size() > 0 )
	{
		vector<double>::iterator nth = values->begin() + floor((values->size()-1)*0.95);
		nth_element(values->begin(),
				nth,
				values->end());
		dvalue = *nth;
	}
	performance->setNativeValue(dvalue);

	return performance;
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Percentile95::getvalues() const
{
  return values;
}

/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Percentile95::setValues(vector< double >* theValue)
{
  values = theValue;
}

}
