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
#include <archdatamodel/resource.h>

namespace ArchDataModel {
	
uint32_t Resource::id = 0;

Resource::Resource(resource_type_t type, std::string model, Node* node) {
	this->id = this->id++;
	this->type = type;
	this->model = model;
	this->inNode = node;
}

Resource::Resource() {
	this->type 		= UNKNOWN;
	this->model 	= "";
	this->inNode 	= NULL;
}

Resource::Resource(const Resource &r) {
	this->type 		= r.getType();
	this->model 	= r.getModel();
	this->inNode 	= r.getNode();
}

Resource::~Resource() { }

Resource *Resource::clone() const {}

void Resource::setType(resource_type_t type) {
	this->type = type;
}

resource_type_t Resource::getType() const {
	return type;
}

void Resource::setModel(std::string model) {
	this->model = model;
}

std::string Resource::getModel() const {
	return model;
}

Node *Resource::getNode() const {
	return inNode;
}

void Resource::setNode(Node * node) {
	inNode = node;
}

uint32_t Resource::getFreeCPUs() {
	return 0;
}

uint32_t Resource::getFreeDisk() {
	return 0;
}
}
