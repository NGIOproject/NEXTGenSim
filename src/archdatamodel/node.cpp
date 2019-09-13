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
#include <archdatamodel/node.h>
#include <archdatamodel/resource.h>
#include <archdatamodel/partitionlist.h>
#include <archdatamodel/compute.h>
#include <archdatamodel/storage.h>
#include <archdatamodel/memory.h>

namespace ArchDataModel {

Node::Node(uint32_t nodeid) {
	id				= nodeid;
	numberCPUs 		= 0;
	memoryBW		= 0;
	memorySize		= 0;
	diskBW			= 0;
	diskSize		= 0;
	fabricBW		= 0;
}

Node::Node(const Node &n) {
	id				= n.id;
	numberCPUs		= n.numberCPUs;
	memoryBW		= n.memoryBW;
	memorySize		= n.memorySize;
	diskBW			= n.diskBW;
	diskSize		= n.diskSize;
	fabricBW		= n.fabricBW;
	std::vector<Resource*> _resources = n.getResourcesInNode();
	for(int i = 0; i < _resources.size(); i++)
		resources.push_back(_resources[i]->clone());
	inPartitionList = n.getPartitionList();
}

Node::~Node() {}

void Node::addResource(Resource *r) {
	resources.push_back(r);
	if(r->getType() == COMPUTE) {
		Compute *c = (Compute*)r;
		numberCPUs += c->getTotalCPUs();
		memorySize += c->getLocalMemory();
		memoryBW += c->getLocalMemoryBW();
	}
	else if(r->getType() == STORAGE) {
		Storage *s = (Storage*)s;
		diskSize += s->getSize();
		diskBW += s->getBW();
	}
	else if(r->getType() == MEMORY) {
		Memory *m = (Memory*)r;
		memorySize += m->getSize();
		memoryBW += m->getBW();
	}
}

uint32_t Node::getNumberCPUs() {
  return numberCPUs;
}

uint64_t Node::getMemorySize() {
  return memorySize;
}

uint32_t Node::getMemoryBW() {
  return memoryBW;
}

uint64_t Node::getDiskSize() {
	return diskSize;
}

uint32_t Node::getDiskBW() {
	return diskBW;
}

uint32_t Node::getFabricBW() { 
  return fabricBW;
}

uint32_t Node::getID() {
  return id;
}

Resource* Node::getResource(uint32_t id) {
	return resources[id];
}

std::vector<Resource*> Node::getResourcesInNode() const {
	return resources;
}

PartitionList *Node::getPartitionList() const {
	return inPartitionList;
}

void Node::setPartitionList(PartitionList* partitionList) {
	inPartitionList = partitionList;
}


}