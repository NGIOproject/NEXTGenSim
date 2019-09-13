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
#ifndef SIMULATORSIMULATORCHARACTERISTICS_H
#define SIMULATORSIMULATORCHARACTERISTICS_H

#include <utils/architectureconfiguration.h>
#include <statistics/statistics.h>
#include <scheduling/tracefile.h>

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/

/** This class contains a the caracteritzation of the simulation */
class SimulatorCharacteristics{
public:
  /* Constructors and desctructors*/

  SimulatorCharacteristics();
  ~SimulatorCharacteristics();
  
  /* Get and sets methods*/
  
  void setArchConfiguration(ArchitectureConfiguration* theValue);	
  ArchitectureConfiguration* getarchConfiguration() ;
  void setSimStatistics(SimStatistics* theValue);
  SimStatistics* getsimStatistics() ;
  void setWorkload(TraceFile* theValue);
  TraceFile* getWorkload();
	
    
private:
   /*The following files provide information about the characteristics of the simulator*/
   ArchitectureConfiguration* archConfiguration; /**< Contains the characteritzation of the architecture used in the current simulator */
   TraceFile* workload; /**< Contains the workload trace file that will be used in the simulation */
   SimStatistics* simStatistics; /**< Contains the statistics configuration that will be computed for the simulation. In this class the user can specify which statistical information wants to retrieve and for wich metric.*/
};

}

#endif
