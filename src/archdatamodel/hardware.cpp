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
#include <archdatamodel/hardware.h>

namespace ArchDataModel {
	
Hardware::Hardware() {
	resources.clear();
	inNode = NULL;
}

Hardware::Hardware(const Hardware &s) {
	std::vector<Resource*> _res = s.getResourcesInHardware();
	for(unsigned int i = 0; i < _res.size(); i++)
		resources.push_back(_res[i]->clone());
	
	inNode = s.getNode();
}

Hardware::~Hardware() { 
	for(unsigned int i = 0; i < resources.size(); i++)
		delete resources[i];
}

void Hardware::addResource(Resource* rsc) {
	resources.push_back(rsc);
}

Resource* Hardware::getResource(uint32_t id) {
	return resources[id];
}

std::vector<Resource*> Hardware::getResourcesInHardware() const {
	return resources;
}

Node *Hardware::getNode() const {
	return inNode;
}

void Hardware::setNode(Node* node) {
	inNode = node;
}

}