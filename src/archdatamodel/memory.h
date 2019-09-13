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
#ifndef MEMORYRESOURCE_H
#define MEMORYRESOURCE_H

#include <string>
#include <cstdint>
#include <archdatamodel/resource.h>

namespace ArchDataModel {

/**
	* @author Daniel Rivas, daniel.rivas@bsc.es
**/

/**
	* This class implements the definition of the resource Memory.
**/

class Node;

class Memory : public Resource {
public:
	Memory(std::string model, uint64_t size, double bw, int persist, Node *node);
	Memory();
	Memory(const Memory &s);
	~Memory();
	Resource *clone() const;

	//sets and gets
	void setSize(uint64_t size);
	void setBW(double bw);
        void setPersist(int persist);

	uint64_t getSize() const;
	double getBW() const;
        int getPersist() const;

private:
	uint64_t size; /* Size in MB */
	double bw; /* Bandwidth in MB/s */
        int persist; /* 0 = no DCPMM; else = DCPMM */

};

}

#endif
