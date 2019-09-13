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
#include <archdatamodel/memory.h>
#include <archdatamodel/node.h>

namespace ArchDataModel {

Memory::Memory(std::string model, uint64_t size, double bw, int persist, Node* node)
	: Resource(MEMORY, model, node) {

	this->size = size;
	this->bw = bw;
        this->persist = persist;
}

Memory::Memory() : Resource(MEMORY, "", NULL) {
	this->size	= 0;
	this->bw		= 0;
        this->persist = 0;
}

Memory::Memory(const Memory &s) : Resource(MEMORY, s.getModel(), s.getNode()) {
	this->size 	= s.getSize();
	this->bw		= s.getBW();
        this->persist = s.getPersist();
}

Resource *Memory::clone() const {
	return (Resource*)(new Memory(*this));
}

Memory::~Memory() {}

void Memory::setSize(uint64_t size) {
	this->size = size;
}

uint64_t Memory::getSize() const {
	return size;
}

void Memory::setBW(double bw) {
	this->bw = bw;
}

double Memory::getBW() const {
	return bw;
}

void Memory::setPersist(int p) {
	this->persist = p;
}

int Memory::getPersist() const {
	return persist;
}


}
