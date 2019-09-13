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
#include <scheduling/jobrequirement.h>

namespace Simulator {

JobRequirement::JobRequirement()
{
  this->metric = new Metric();
  this->operatorRequired = NONE;
}


JobRequirement::~JobRequirement()
{
  delete metric;
}

/**
 * Returns the metric that models the job requirement (this includes the type of the requirement and its content) 
 * @return An metric containing the information
 */
Metric* JobRequirement::getMetric() const
{
	return metric;
}

/**
 * Sets the metric that models the job requirement 
 * @param theValue The metric used
 */
void JobRequirement::setMetric ( Metric* theValue )
{
	metric = theValue;
}

/**
 * Returns the operation specified in the requirement
 * @return An operator_t containing metric
 */
operator_t JobRequirement::getoperatorRequired() const
{
	return operatorRequired;
}

/**
 * Sets the operator specified in the requirement
 * @param theValue The metric used
 */
void JobRequirement::setOperatorRequired ( const operator_t& theValue )
{
	operatorRequired = theValue;
}

}

