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
#ifndef COMPUTERESOURCE_H
#define COMPUTERESOURCE_H

#include <string>
#include <cstdint>
#include <archdatamodel/resource.h>

namespace ArchDataModel {

/**
	* @author Daniel Rivas, daniel.rivas@bsc.es
**/

/**
	* This class implements the definition of the resource Compute. 
**/

class Node;

class Compute : public Resource {
public:
	Compute(std::string model, uint32_t sockets, uint32_t cps, uint64_t mem, Node* node);
	Compute(std::string model, uint32_t sockets, uint32_t cps, double freq, double voltage, 
		double tdp, uint32_t tpc, uint32_t vpu, uint64_t mem, uint32_t membw, Node *node);
	Compute(const Compute &c);
	Compute();
	~Compute();
	Resource *clone() const;
	
	//sets and gets
	void setSockets(uint32_t sockets);
	void setCoresPerSocket(uint32_t cps);
	void setFrequency(double freq);
	void setVoltage(double voltage);
	void setTDP(double tdp);
	void setMultithreading(uint32_t tpc);
	void setVPUSize(uint32_t vpu);
	void setLocalMemory(uint64_t mem);
	
	uint32_t getSockets() const;
	uint32_t getCoresPerSocket() const;
	uint32_t getTotalCPUs() const;
	double getFrequency() const;
	double getVoltage() const;
	double getTDP() const;
	uint32_t getMultithreading() const;
	uint32_t getVPUSize() const;
	uint64_t getLocalMemory() const;
	uint32_t getLocalMemoryBW() const;
	
private:

	// Basic characteristics for scheduling.
	uint32_t sockets; /* Number of sockets. */
	uint32_t cps; /* Number of CPUs per socket. */
	uint32_t totalCPUs; /* Total number of CPUs. (= sockets*cps but stored for convenience) */
	uint64_t localMem; /* Local Memory RAM in socket. In MB */
	uint32_t localMemBW; /* BW of the local memory RAM in socket. In MB/s. */
	
	// Additional characteristics (optional)
	double freq; /* Frequency of the CPUs */
	double voltage; /* Working voltage of the CPUs. */
	double tdp; /* Thermal Design Power of the processor. */
	uint32_t tpc; /* Number of hardware threads/core. 0 means no SMT support. */
	uint32_t vpu; /* Size of the Vector Processing Unit. 0 means no VPU. */
	
};

}

#endif
