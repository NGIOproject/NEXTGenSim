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
#include <scheduling/simulatorcharacteristics.h>


namespace Simulator {

/**
 * The default constructor for the class
 */
SimulatorCharacteristics::SimulatorCharacteristics()
{
  this->archConfiguration = NULL;
  this->simStatistics = NULL;
  this->workload = NULL;
}

/**
 * The default destructor for the class 
 */
SimulatorCharacteristics::~SimulatorCharacteristics()
{
}


/**
 * Returns the configuration for the architecture that is being simulated 
 * @return A reference to the architecture configuration
 */
ArchitectureConfiguration* SimulatorCharacteristics::getarchConfiguration() 
{
  return archConfiguration;
}


/**
 * Sets the architecture configuration that will be used in the simulation 
 * @param theValue A reference to the architecture configuration
 */
void SimulatorCharacteristics::setArchConfiguration(ArchitectureConfiguration* theValue)
{
  archConfiguration = theValue;
}


/**
 * Returns the simStatistics that will be used in the simulation 
 * @return A reference to the simStatistics 
 */
SimStatistics* SimulatorCharacteristics::getsimStatistics() 
{
  return simStatistics;
}


/**
 * Sets the simStatistics that will be used in the simulation 
 * @param theValue A reference to the simStatistics
 */
void SimulatorCharacteristics::setSimStatistics(SimStatistics* theValue)
{
  simStatistics = theValue;
}


/**
 * Returns the workload that will be used as in input for the simulation 
 * @return A reference to the workload 
 */
TraceFile* SimulatorCharacteristics::getWorkload() 
{
  return workload;
}


/**
 * Sets the workload that will be used in the simulation 
 * @param theValue A reference to the workload 
 */
void SimulatorCharacteristics::setWorkload(TraceFile* theValue)
{
  workload = theValue;
}

}
