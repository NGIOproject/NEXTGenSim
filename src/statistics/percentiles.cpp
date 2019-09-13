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
#include <statistics/percentiles.h>
#include <utils/utilities.h>

namespace Statistics {

/**
 * The default constructor for the class - by default the interval is 10 (in the future it may be changed)
 */
Percentiles::Percentiles()
{
  this->values = NULL;
  this->interval = 10;
}

/**
 * The default destructor for the class 
 */
Percentiles::~Percentiles()
{
}

/**
 * Function that computes the percentiles of all the values stored in the vector
 * @return Metric with the Percentiles as a string 
 */
Simulator::Metric* Percentiles::computeValue()
{
	Simulator::Metric* performance = new Simulator::Metric();
	performance->setStatisticUsed(Simulator::PERCENTILES);
	performance->setNativeType(Simulator::STRING);

	string percentiles;
	if ( values->size() != 0 )
		//then we compute all the percentiles
		for(double curPerc = interval; curPerc <= 100; curPerc+=interval)
		{
			/* first we sort all the elements of the vector */
			vector<double>::iterator nth = values->begin() + floor((values->size()-1)*curPerc/100);
			nth_element(values->begin(),
					nth,
					values->end());
			percentiles +="P("+ftos(curPerc)+")="+ftos(*nth)+";";
		}
	performance->setNativeString(percentiles);
	return performance;
}


/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* Percentiles::getvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void Percentiles::setValues(vector< double >* theValue)
{
  values = theValue;
}

}
