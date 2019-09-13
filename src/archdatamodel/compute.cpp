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
#include <archdatamodel/compute.h>

namespace ArchDataModel {

Compute::Compute(std::string model, uint32_t sockets, uint32_t cps, uint64_t mem, Node *node)
	: Resource(COMPUTE, model, node) {
	
	this->sockets 	= sockets;
	this->cps 			= cps;
	this->localMem	= mem;
	this->tpc 				= 1;
	this->tdp 				= 0;
	this->freq 				= 0;
	this->voltage 		= 0;
	this->vpu					= 0;
	
	this->totalCPUs = this->sockets*this->cps;
}

Compute::Compute(std::string model, uint32_t sockets, uint32_t cps, double freq, 
	double voltage, double tdp, uint32_t tpc, uint32_t vpu, uint64_t mem, uint32_t membw, Node* node)
	: Resource(COMPUTE, model, node) {
	
	this->sockets 		= sockets;
	this->cps 				= cps;
	this->freq 				= freq;
	this->voltage 		= voltage;
	this->tdp 				= tdp;
	this->tpc 				= tpc;
	this->vpu					= vpu;
	this->localMem		= mem;
	this->localMemBW	= membw;
	
	this->totalCPUs = this->sockets*this->cps;
}

Compute::Compute() : Resource(COMPUTE, "", NULL) {
	this->sockets 		= 0;
	this->cps 				= 0;
	this->freq 				= 0;
	this->voltage 		= 0;
	this->tdp 				= 0;
	this->tpc 				= 0;
	this->vpu					= 0;
	this->localMem		= 0;
	this->localMemBW 	= 0;
	
	this->totalCPUs = this->sockets*this->cps;
}

Compute::Compute(const Compute &c) : Resource(COMPUTE, c.getModel(), c.getNode()) {
	sockets 	= c.getSockets();
	cps 			= c.getCoresPerSocket();
	freq 			= c.getFrequency();
	voltage 	= c.getVoltage();
	tdp 			= c.getTDP();
	tpc 			= c.getMultithreading();
	vpu				= c.getVPUSize();
	localMem	= c.getLocalMemory();
	localMemBW = c.getLocalMemoryBW();
	
	this->totalCPUs = this->sockets*this->cps;
}

Resource *Compute::clone() const {
	return (Resource*)(new Compute(*this));
}

Compute::~Compute() {}

void Compute::setSockets(uint32_t sockets) {
	this->sockets = sockets;
}

uint32_t Compute::getSockets() const {
	return sockets;
}

void Compute::setCoresPerSocket(uint32_t cps) {
	this->cps = cps;
}

uint32_t Compute::getCoresPerSocket() const {
	return cps;
}

uint32_t Compute::getTotalCPUs() const {
	return totalCPUs;
}

void Compute::setFrequency(double freq) {
	this->freq = freq;
}

double Compute::getFrequency() const {
	return freq;
}

void Compute::setVoltage(double voltage) {
	this->voltage = voltage;
}

double Compute::getVoltage() const {
	return voltage;
}

void Compute::setTDP(double TDP) {
	this->tdp = tdp;
}

double Compute::getTDP() const {
	return tdp;
}

void Compute::setMultithreading(uint32_t tpc) {
	this->tpc = tpc;
}

uint32_t Compute::getMultithreading() const {
	return tpc;
}

void Compute::setVPUSize(uint32_t vpu) {
	this->vpu = vpu;
}

uint32_t Compute::getVPUSize() const {
	return vpu;
}

uint64_t Compute::getLocalMemory() const {
	return localMem;
}

uint32_t Compute::getLocalMemoryBW() const {
	return localMemBW;
}

}
