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
#ifndef SIMULATORRESOURCEUSAGE_H
#define SIMULATORRESOURCEUSAGE_H

#include <utils/architectureconfiguration.h>

using namespace Utils;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
/**
*   This class is a conveyor about the status of a given resource/host in a given timestamp. 
*/
class ResourceUsage{
public:
  ResourceUsage();
  ~ResourceUsage();
  void setNumberNodesUsed ( int theValue );
  int getNumberNodesUsed() const;
  void setNumberCpusUsed ( int theValue );
  int getNumberCPUsUsed() const;
  void setNodeSizeUsed ( int theValue );
  int getNodeSizeUsed() const;
  void setMemoryUsed ( double theValue );
  double getMemoryUsed() const;
  void setDiskSizeUsed ( double theValue );
  double getDiskSizeUsed() const;
  void setArchitecture ( ArchitectureConfiguration* theValue );
  ArchitectureConfiguration* getArchitecture() const;	

private: 
   ArchitectureConfiguration* architecture;
   int NumberCpusUsed; /**< The total number of processors of the architecture used */
   int nodeSizeUsed; /**< The size of the nodes -- not always valid (see heterogeneous resources) used */
   int NumberNodesUsed; /**< The total number of nodes in the architecture used  */   
   
   double diskSizeUsed; /**< The amount of terabytes of capacity of the host used  */
   double memoryUsed; /**< The global amount of Memory - this field is an agregated of all the information, in some situations may be it make no sense. used  */

};

}

#endif
