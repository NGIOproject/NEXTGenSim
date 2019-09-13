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
#ifndef UTILSCSVPOLICYINFOCONVERTER_H
#define UTILSCSVPOLICYINFOCONVERTER_H



#include <scheduling/job.h>
#include <scheduling/schedulingpolicy.h>
#include <utils/cvsconverter.h>
#include <scheduling/policyentitycollector.h>

#include <map>

using namespace std;
using std::map;
using namespace Simulator;

/**
* This class will generate a cvs file with the information about all the states for the centers that have been simulated.
*/
namespace Utils {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class CSVPolicyInfoConverter : public CSVConverter{
public:
  CSVPolicyInfoConverter(string FilePath,Log* log);
  ~CSVPolicyInfoConverter();
    
  void createPolicyCSVInfoFile(PolicyEntityCollector* collector);

};

}

#endif
