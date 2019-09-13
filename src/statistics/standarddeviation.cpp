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
#include <statistics/standarddeviation.h>

#include <statistics/average.h>

#include <algorithm>
#include <math.h>

namespace Statistics {


/**
 * The default constructor for the class
 */
StandardDeviation::StandardDeviation()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
StandardDeviation::~StandardDeviation()
{
}

/**
 * Function that computes the standard deviation of all the values stored in the vector
 * @return Metric with the standard deviation
 */
Simulator::Metric* StandardDeviation::computeValue()
{
	double dvalue = 0.0;
	Simulator::Metric* performance = new Simulator::Metric();
	performance->setStatisticUsed(Simulator::STDEV);
	performance->setNativeType(Simulator::DOUBLE);
	if(this->values->size() > 1)
	{
		/* first we compute the mean */
		Average avg;
		avg.setValues(this->values);
		Simulator::Metric* tmpMetric = avg.computeValue();
		double mean = tmpMetric->getnativeDouble();
		delete tmpMetric;
		double sum_of_square_differences = 0;
		for ( vector<double>::iterator iter = this->values->begin();
				iter != this->values->end();
				++iter)
		{
			const double this_square_difference = *iter - mean;
			sum_of_square_differences += this_square_difference * this_square_difference;
		}
		dvalue = sqrt( sum_of_square_differences / ( this->values->size() - 1 ));
	}
	performance->setNativeValue(dvalue);
	return performance;
}

/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector 
 */
vector< double >* StandardDeviation::getvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void StandardDeviation::setValues(vector< double >* theValue)
{
  values = theValue;
}


}
