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
#ifndef SYSTEMORCHESTRATOR_H
#define SYSTEMORCHESTRATOR_H

#include <utils/configurationfile.h> //Needed for policy_type_t
#include <scheduling/job.h> //RS_policy_type_t
#include <scheduling/resourcebucket.h>

#include <vector>
#include <map>


//Forward declarations
namespace Simulator {
	class ResourceSchedulingPolicy;
	class ResourceReservationTable;
	class SchedulingPolicy;
	class Partition;
}

namespace Utils {
	class ArchitectureConfiguration;
	class Log;
	class ConfigurationFile;
        class SimulationConfiguration;
}

namespace ArchDataModel {
	class Node;
}

namespace Simulator {

class Simulation;

/**
* @author Daniel Rivas, daniel.rivas@bsc.es
**/


/**
	* This class implements the definition of the System Orchestrator used in this simulation.
	* It contains all the information about nodes, partitions and scheduling.
	* It also constructs all of it, including reservation tables.
**/

class ArchitectureConfiguration;

class SystemOrchestrator {
public:
	SystemOrchestrator(Utils::ArchitectureConfiguration *archConf, double startTime, Utils::Log* log=NULL);
	~SystemOrchestrator();
	
	void setPolicy(ResourceSchedulingPolicy *policy);
	ResourceSchedulingPolicy *getPolicy();
	void setLog(Utils::Log *log);
	void setGlobalTime(uint64_t globalTime);
	void setFirstStartTime(double time);
	double getFirstStartTime();
	Utils::ArchitectureConfiguration *getArchitecture();
	
	void createPartitions(std::vector<ArchDataModel::PartitionList*> partitionlists);
	std::vector<ArchDataModel::Node*> getNodes();
	
	//Partitions related functions
	uint32_t getNumberPartitions();
	void addPartition(Partition *p);
	Partition *getPartition(uint32_t id);
	Partition *getPartition(std::string id);
	
	//Scheduling related functions
	void arrival(Job* job);
	void start(Job* job);
	void termination(Job* job);
	void abnormalTermination(Job* job);
	void schedule();
        void backfill();
        void transitiontoCompute(Job* job);
        void transitiontoOutput(Job* job);
	
	void PreProcessScheduling();
	void PostProcessScheduling();
	
	//Sim related functions
	virtual void setSim(Simulation* theValue);
	void setSimulatePartitions(bool simp);
	void setReserveFullNode(bool rfn);
        bool getReserveFullNode();
        
        // Tracing Options
        void setOutputTrace(bool theValue);
        bool getOutputTrace();
        
        Utils::SimulationConfiguration* getsimuConfig();
        void setsimuConfig(Utils::SimulationConfiguration *simuc);

private:
	ResourceSchedulingPolicy *createPolicy(Utils::policy_type_t pType, RS_policy_type_t rspType, double startTime, Utils::Log *log);
	Partition *selectPartition(Job *job);

	std::vector<ArchDataModel::Node*> nodes;
	
	bool simulatePartitions;
	bool reserveFullNode;
        bool generateOutputTrace;

	std::vector<Partition*> partitions; /**< Vector with all the partitions defined.*/
	std::map<Job*,Partition*> job2part; /**< A Map to know to which partition a job was sent.*/
	ResourceSchedulingPolicy *policy; /**< Policy used to schedule jobs */
	
	RS_policy_type_t rspolicyType;
	Utils::policy_type_t policyType;
	ResourceReservationTable *rt; /**< Reservationtable */
	
	Utils::ArchitectureConfiguration *archConf; /**< Architecture Configuration of the simulation.*/
	Utils::Log *log; /**< Log of the simulation. */
	uint64_t globalTime; /**< globalTime of the simulation.*/
	double startTime; /**< Start time of the first job submitted to be scheduled.*/
	
	Simulation* sim;
        Utils::SimulationConfiguration* simuConfig;
	
	std::map< std::pair<uint64_t, uint64_t>, Partition* > partSel; /**< map to speed up partition selection. Like a LUT of previous selections.*/
};

}

#endif
