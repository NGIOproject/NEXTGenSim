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
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <cstdint>


namespace ArchDataModel {

 /** This enum contains all the different resource types */
enum resource_type_t {
    COMPUTE,
	STORAGE,
	MEMORY,
	UNKNOWN,
	NUM_TYPES
};

/**
	* @author Daniel Rivas, daniel.rivas@bsc.es
**/

/**
	* This class implements the definition of Resource. 
	* It is an abstract class that each specific resource will have to inherit.
**/

class Node;

class Resource {
public:
	Resource(resource_type_t type, std::string model, Node* node);
	Resource();
	Resource(const Resource &r);
	virtual ~Resource();
	
	virtual Resource *clone() const;
	
	void setType(resource_type_t type);
	resource_type_t getType() const;
	void setModel(std::string model);
	std::string getModel() const;
	
	Node *getNode() const;
	void setNode(Node* node);
	
	//Check resource availability
	virtual uint32_t getFreeCPUs();
	virtual uint32_t getFreeDisk();
	
protected:
	static uint32_t id; /* Number that uniquely identifies the resource. */
	resource_type_t type; /* Type of resource. Ex: Compute, Storage, Fabric, Accelerator, etc. */
	std::string model; /* Model of the resource. Can be void. Ex: Intel Xeon, NVMe, etc. */
	Node *inNode;
};

}

#endif
