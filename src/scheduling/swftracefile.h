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
#ifndef SWFTRACEFILE_H
#define SWFTRACEFILE_H

#include <scheduling/tracefile.h>
#include <utils/log.h>
#include <iostream>
#include <fstream>

using namespace Utils;

namespace Simulator {

//Forward declaration 

/**
@author Francesc Guim,C6-E201,93 401 16 50,
*/

/** Class for loading SWF traces files */
class SWFTraceFile: public TraceFile {
public:
  SWFTraceFile();
  ~SWFTraceFile();
  SWFTraceFile(string FilePath,Log* log, bool is_output = false);
	
	bool openTrace();
	void logJob(Job *job);
    
  virtual bool loadTrace();
  virtual void processHeaderEntry(string headerLine);

protected:
  bool isGrid; /**< This variable inidicates if the workload is a grid workload, this will be considered in the job object instantiation to decide with kind of job has to be created */
  bool isExtended;  /**< Similar to the previous value indicates when the current workload is an extended workload, this may be used by the the loader to decide which job has to be instantiated */


	bool is_output;
	std::ofstream outtrace;
};

}
#endif
