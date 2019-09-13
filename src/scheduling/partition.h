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
#ifndef PARTITION_H
#define PARTITION_H

#include <set>
#include <string>
#include <archdatamodel/node.h>
#include <scheduling/job.h> //Job class + RS_policy_type_t
#include <utils/configurationfile.h> //Needed for policy_type_t

namespace Utils {
	class Log;
}

namespace Simulator {

//Forward declarations
class ResourceSchedulingPolicy;
class ResourceReservationTable;
class SchedulingPolicy;
class Simulation;


struct nodeid_lt_t {
	bool operator() (ArchDataModel::Node* n1, ArchDataModel::Node* n2) const {
		return n1->getID() < n2->getID();
	}
};

class Partition {
public:
	Partition();
	Partition(double startTime, Utils::Log *log);
	~Partition();

	ResourceReservationTable *getReservationTable();
	void setReservationTable(ResourceReservationTable *rt);
	Utils::policy_type_t getPolicyType();
	RS_policy_type_t getRSPolicyType();
	ResourceSchedulingPolicy *getSchedulingPolicy();
	void setSchedulingPolicy(ResourceSchedulingPolicy *policy);

	void addNode(ArchDataModel::Node *node);
	ArchDataModel::Node *removeNode(uint32_t nodeid);
	
	//Get information about the partition and its nodes.
	std::string getName();
	uint32_t getNumNodes();
	uint32_t getTotalCPUs();
	uint32_t getCPUsPerNode();
	uint64_t getTotalMemory();
	uint64_t getMemoryPerNode();
	uint64_t getTotalDisk();
	uint64_t getDiskPerNode();
	
	//Scheduling related functions
	void arrival(Job *job);
	void start(Job *job);
	void termination(Job *job);
	void schedule();
        void backfill();
        void transitionToCompute(Job* job);
        void transitionToOutput(Job* job);
	
	//Global time: set and propagation.
	void setGlobalTime(uint64_t globalTime);

	//Sim related functions
	virtual void setSim(Simulation* theValue);
	
private:
	Utils::policy_type_t getPolicyType(std::string spolicy);
	RS_policy_type_t getRSPolicyType(std::string rspolicy);
	
	std::string name; /**< Name of the Partition to be displayed.*/
	uint32_t numNodes; /**< Number of nodes in the partition.*/
	//Characteristics of the nodes
	uint32_t cpus; /**< Number of cores per node of the partition.*/
	uint64_t mem; /**< Memory (in MB) per node of the partition.*/
	uint64_t disk; /**< Size (in MB) of the disk per node of the partition.*/
	Utils::policy_type_t policyType;
	RS_policy_type_t rspolicyType;
	
	ResourceReservationTable *rt; /**< Each partition has its own reservation table 
																	in order to abstract how the real hardware is reserved.*/
	ResourceSchedulingPolicy *policy; /**< Scheduling Policy used to schedule jobs in this partition.*/

	std::set<ArchDataModel::Node*, nodeid_lt_t> nodes; /**< List of nodes in the partition.*/
	
	uint64_t globalTime;
	
	Utils::Log *log; /**< Log of the simulation.*/
	
	Simulation *sim;
};

}

#endif
