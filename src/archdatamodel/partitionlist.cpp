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
#include <archdatamodel/partitionlist.h>

namespace ArchDataModel {

PartitionList::PartitionList() { 
	nodes.clear();
}

PartitionList::PartitionList(const PartitionList &r) {
	std::vector<Node*> _nodes = r.getNodesInPartitionList();
	
	for(unsigned int i = 0; i < _nodes.size(); i++)
		nodes.push_back(new Node(*_nodes[i]));
}

void PartitionList::addNode(Node* node) {
	nodes.push_back(node);
}

Node* PartitionList::getNode(uint32_t id) {
	return nodes.at(id);
}

std::vector<Node*> PartitionList::getNodesInPartitionList() const {
	return nodes;
}

PartitionList::~PartitionList() {
	for(unsigned int i = 0; i < nodes.size(); i++)
		delete nodes[i];
}

int PartitionList::getNumNodes(){
	return nodes.size();
}

}