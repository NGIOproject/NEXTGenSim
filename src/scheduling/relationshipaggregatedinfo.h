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
#ifndef SIMULATORRELATIONSHIPAGGREGATEDINFO_H
#define SIMULATORRELATIONSHIPAGGREGATEDINFO_H

#include <string>

using namespace std;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
*  This class contains the aggregate info specifically about the relationship that the borkers will be sending around the meta-brokering system for interchange information about their status and scheduling metrics.
*/

class RelationshipAggregatedInfo{
public:
  RelationshipAggregatedInfo();
  ~RelationshipAggregatedInfo();
  
  void setTargetType ( const string& theValue );
  string getTargetType() const;
  void setTargetName ( const string& theValue );
  string getTargetName() const;	
  void setSourceType ( const string& theValue );
  string getSourceType() const;
  void setSourceName ( const string& theValue );
  string getSourceName() const;
	

private:
  string SourceType; /**< String containing the source type for the relation ship   */
  string SourceName ; /**<  String containing the source name for the relation ship  */
  string TargetType; /**< String containing the Target type for the relation ship   */
  string TargetName ; /**<  String containing the Target name for the relation ship  */

};

}

#endif
