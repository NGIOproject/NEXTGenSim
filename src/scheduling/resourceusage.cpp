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
#include <scheduling/resourceusage.h>

namespace Simulator {

ResourceUsage::ResourceUsage()
{
  NumberCpusUsed = 0;
  nodeSizeUsed = 0;
  NumberNodesUsed = 0;
  diskSizeUsed = 0;
  memoryUsed = 0;
}


ResourceUsage::~ResourceUsage()
{
}

/**
 * Returns the architecture
 * @return A architecture the architecture used in the simulation
 */
ArchitectureConfiguration* ResourceUsage::getArchitecture() const
{
	return architecture;
}

/**
 * Sets the EthernetBWLoad to the ResourceUsage
 * @param theValue The EthernetBWLoad
 */
void ResourceUsage::setArchitecture ( ArchitectureConfiguration* theValue )
{
	architecture = theValue;
}

/**
 * Returns the diskSizeUsed
 * @return A double containing the diskSizeUsed used 
 */
double ResourceUsage::getDiskSizeUsed() const
{
	return diskSizeUsed;
}

/**
 * Sets the diskSizeUsed to the ResourceUsage
 * @param theValue The diskSizeUsed
 */
void ResourceUsage::setDiskSizeUsed ( double theValue )
{
	diskSizeUsed = theValue;
}

/**
 * Returns the memoryUsed
 * @return A double containing the memoryUsed used 
 */
double ResourceUsage::getMemoryUsed() const
{
	return memoryUsed;
}

/**
 * Sets the memoryUsed to the ResourceUsage
 * @param theValue The memoryUsed
 */
void ResourceUsage::setMemoryUsed ( double theValue )
{
	memoryUsed = theValue;
}

/**
 * Returns the nodeSizeUsed
 * @return A double containing the nodeSizeUsed used 
 */
int ResourceUsage::getNodeSizeUsed() const
{
	return nodeSizeUsed;
}

/**
 * Sets the nodeSizeUsed to the ResourceUsage
 * @param theValue The nodeSizeUsed
 */
void ResourceUsage::setNodeSizeUsed ( int theValue )
{
	nodeSizeUsed = theValue;
}

/**
 * Returns the NumberCpusUsed
 * @return A double containing the NumberCpusUsed used 
 */
int  ResourceUsage::getNumberCPUsUsed() const
{
	return NumberCpusUsed;
}

/**
 * Sets the NumberCpusUsed to the ResourceUsage
 * @param theValue The NumberCpusUsed
 */
void  ResourceUsage::setNumberCpusUsed ( int theValue )
{
	NumberCpusUsed = theValue;
}


int  ResourceUsage::getNumberNodesUsed() const
{
	return NumberNodesUsed;
}

/**
 * Sets the NumberNodesUsed to the ResourceUsage
 * @param theValue The NumberNodesUsed
 */
void  ResourceUsage::setNumberNodesUsed ( int theValue )
{
	NumberNodesUsed = theValue;
}

}
