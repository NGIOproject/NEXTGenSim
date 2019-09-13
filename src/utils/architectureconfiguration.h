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
#ifndef ARCHITECTURECONF_H
#define ARCHITECTURECONF_H

#include <string>
#include <vector>
#include <map>
#include <archdatamodel/partitionlist.h>
#include <archdatamodel/resource.h>
#include <archdatamodel/compute.h>
#include <archdatamodel/node.h>
#include <archdatamodel/storage.h>
#include <archdatamodel/memory.h>
#include <archdatamodel/hardware.h>
#include <utils/configurationfile.h>
#include <utils/log.h>
#include <boost/property_tree/ptree.hpp>
#include <cstdint>

//Forward declarations
namespace Simulator {
	class SystemOrchestrator;
}

namespace Utils {

/**
* @author Daniel Rivas, daniel.rivas@bsc.es
**/


/**
	* This class implements the definition of the System Architecture used in this simulation.
	* It will contain all the physical/conceptual elements such as Partitions, Nodes, Hardware, Resource etc
	* plus some general info.
**/

class ArchitectureConfiguration {
public:
	//Constructors
	ArchitectureConfiguration(std::string configFile, Utils::Log* log, double startime, architecture_type_t type);
	ArchitectureConfiguration();
	~ArchitectureConfiguration();
	
	bool loadConfigurationFromFile();
	
	std::string getConfigFile();
	void setConfigFile(std::string configFile);
	
	ArchDataModel::PartitionList* getPartitionList(uint32_t id);

	Simulator::SystemOrchestrator *getSystemOrchestrator();
	std::vector<ArchDataModel::PartitionList*> getPartitionListsInSystem();
	std::string getName();
	std::string getVendor();
	std::string getOSName();
	
	//TODO
	std::vector<double> getFrequencies();
	std::vector<double> getVoltages();
	double getActivity();
	double getPortion();
	
	//Resources' information
	uint64_t getNumberCPUs();
	uint64_t getTotalMemory();
	uint64_t getTotalDisk();
	double getClockSpeed();

	architecture_type_t getArchitectureType();
	void setArchitectureType(architecture_type_t type);
	
private:
	bool readSystemArchitecture(boost::property_tree::ptree pt);
	ArchDataModel::Compute* readComputeAttributes(boost::property_tree::ptree::value_type comp);
	ArchDataModel::Storage* readStorageAttributes(boost::property_tree::ptree::value_type stor);
	ArchDataModel::Memory* readMemoryAttributes(boost::property_tree::ptree::value_type mem);

	//std::map<std::string, Resource*> readResources(boost::property_tree::ptree pt);

	std::string name; 	/* Name of the machine */
	std::string vendor; /* Vendor of the machine */
	std::string osname; /* The O.S. of system running on the host */
	Utils::Log* log;		/* Log for recording debug information */
	
	std::vector<ArchDataModel::PartitionList *> partitionLists;
	
	std::string configFile; /* Path of the config file that contains the system
													architecture's definition */
													
	uint64_t totalCPUs; //Total number of CPUs in the system.
	uint64_t totalMemory;
	uint64_t totalDisk;
	double clockSpeed;
	
	Simulator::SystemOrchestrator *sysorch; /* Pointer to the System Orchestrator in charge of Node Management. */
	architecture_type_t architectureType;
	double startime;
};

}

#endif
