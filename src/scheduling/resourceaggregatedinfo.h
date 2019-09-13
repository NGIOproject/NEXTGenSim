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
#ifndef SIMULATORRESOURCEAGGREGATEDINFO_H
#define SIMULATORRESOURCEAGGREGATEDINFO_H

#include <list>
#include <vector>
#include <map>
#include <string>

#include <scheduling/metric.h>

using namespace std;
using std::list;
using std::vector;
using std::map;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/**
*  This class contains the aggregate info specifically about the centers that the borkers will be sending around the meta-brokering system for interchange information about their status and scheduling metrics.
*/

class ResourceAggregatedInfo{
public:
  ResourceAggregatedInfo();
  ~ResourceAggregatedInfo();

  void setResourceType ( const string& theValue );
  string getResourceType() const;
  void setResourceName ( const string& theValue );
  string getResourceName() const;
  void setAttributes ( vector< Metric * >* theValue );
  vector< Metric * >* getAttributes() const;
	

private:
 string ResourceName; /**< A string containing the resource name */
 string ResourceType; /**< A string containing the resource type */
 
 vector<Metric*>* Attributes; /**< A vector of metric  of the resource, each metric should be a string with the value containing the value for the attribute*/

};

}

#endif
