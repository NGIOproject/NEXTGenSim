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
#ifndef PARTITIONLIST_H
#define PARTITIONLIST_H

#include <vector>
#include <cstdint>
#include <archdatamodel/node.h>

namespace ArchDataModel {

/**
	* This class implements the definition of PartitionList. It contains nodes.
**/

class PartitionList {
public:
	//Constructors
	PartitionList();
	PartitionList(const PartitionList &p);
	~PartitionList();
	
	Node* addNode();
	void addNodes(uint32_t numNodes);
	void addNode(Node* node);
	
	Node* getNode(uint32_t id);
	std::vector<Node*> getNodesInPartitionList() const;
	int getNumNodes();

private:
	std::vector<Node*> nodes;
	
};

}

#endif
