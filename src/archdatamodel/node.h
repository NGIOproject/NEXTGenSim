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
#ifndef NODE_H
#define NODE_H

#include <cstdint>
#include <vector>

namespace ArchDataModel {

/**
	* @author Daniel Rivas, daniel.rivas@bsc.es
**/

/**
	* This class implements the definition of Node.
**/

class Resource;
class PartitionList;

class Node {
public:
	Node(uint32_t nodeid);
	Node(const Node &n);
	~Node();
	
	void addResource(Resource* r);
	Resource* getResource(uint32_t id);
	std::vector<Resource*> getResourcesInNode() const;
	void setPartitionList(PartitionList* pl);
	PartitionList *getPartitionList() const;	
	
	uint64_t getMemorySize();
	uint32_t getMemoryBW();
	uint32_t getFabricBW();
	uint64_t getDiskSize();
	uint32_t getDiskBW();
	uint32_t getID();
	uint32_t getNumberCPUs();
	
protected:
	/*Characteristics of the node*/
	uint64_t memorySize; /**< Size of the main memory in MB*/
	uint32_t memoryBW; /**< MB/s Size of the Bus Memory*/
	uint32_t fabricBW; /**< MBs/second of the Size of the fibre that convey the messages to the main network*/
	uint64_t diskSize; /**< Size of the disk attached in MB*/
	uint32_t diskBW; /**< MB/s Size of the bus to disk*/
	uint32_t numberCPUs; /**< Indicates the number of Cpus available on the node */
	
	uint32_t id; /**< ID of the Node. */
	
	PartitionList *inPartitionList; /** < The partition in which this node lies */
  
	//Physical Resources assigned to the node.
	std::vector<Resource*> resources;
};


}

#endif
