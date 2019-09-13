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
#ifndef HARDWARE_H
#define HARDWARE_H

#include <vector>
#include <cstdint>
#include <archdatamodel/resource.h>


namespace ArchDataModel {

/**
	* This class implements the definition of Hardware. It contains a collection of resources.
**/

class Node;

class Hardware {
public:
	Hardware();
	Hardware(const Hardware &h);
	~Hardware();
	
	void addResource(Resource* rsc);
	Resource* getResource(uint32_t id);
	std::vector<Resource*> getResourcesInHardware() const;
	
	Node *getNode() const;
	void setNode(Node *node);

private:
	std::vector<Resource*> resources;
	Node *inNode;
};

}

#endif
