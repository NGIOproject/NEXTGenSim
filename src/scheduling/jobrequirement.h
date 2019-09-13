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
#ifndef SIMULATORJOBREQUIREMENT_H
#define SIMULATORJOBREQUIREMENT_H

#include <scheduling/metric.h>

/** This enums indicates the available operators that can be associated to a job requirement */
enum operator_t {
    LESS_THAN=0,
    EQUAL,	    	 
    HIGHER_THAN,
    LESS_EQ_THAN,
    HIGHER_EQ_THAN,
    NONE
};


namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
This class conatins information about the job requirements. Note that a job requirement is basically grid oriented. It allows to specify the requirement of a given resource and the operator.
*/


class JobRequirement{
public:
  JobRequirement();
  ~JobRequirement();

  void setOperatorRequired ( const operator_t& theValue );
  operator_t getoperatorRequired() const;
  void setMetric ( Metric* theValue );
  Metric* getMetric() const;
	

private:
  operator_t operatorRequired; /**< The operator used for specify the requirement */
  Metric* metric; /**< The metric that contains the value related to the requiment  */

};

}

#endif
