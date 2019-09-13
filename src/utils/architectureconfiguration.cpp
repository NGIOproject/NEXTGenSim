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
#include <utils/architectureconfiguration.h>
#include <scheduling/systemorchestrator.h>
#include <scheduling/partition.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <exception>


using namespace ArchDataModel;

namespace Utils {

ArchitectureConfiguration::ArchitectureConfiguration(std::string configFile, Utils::Log* log, double startime, architecture_type_t type) {
	this->configFile 	= configFile;
	this->log					= log;
	this->totalCPUs 	= 0;
	this->totalMemory = 0;
	this->architectureType = type;
	this->startime		= startime;
}

ArchitectureConfiguration::ArchitectureConfiguration() {}

ArchitectureConfiguration::~ArchitectureConfiguration() {}

bool ArchitectureConfiguration::loadConfigurationFromFile() {
	//TODO
	log->debug("Loading the architecture file: " + configFile);
	
	using boost::property_tree::ptree;
	ptree pt;
	
	try {
		read_xml(configFile.c_str(), pt);
	}
	catch(std::exception const& ex)
	{
		std::cout << "Exception! " << ex.what() << std::endl;
		return false;
	}
	
	return readSystemArchitecture(pt);
}

bool ArchitectureConfiguration::readSystemArchitecture(boost::property_tree::ptree pt) {
	name 	= pt.get<std::string>("System.SystemInfo.Name", "-");
	vendor 	= pt.get<std::string>("System.SystemInfo.Vendor", "");
	osname	= pt.get<std::string>("System.SystemInfo.OSName", "");
		
	/* First build a map with all the different resources. Each one has a unique 
		identifier that'll be referenced later from the system definition. */
	std::map<std::string, Resource*> resources;
	
	using boost::property_tree::ptree;
	BOOST_FOREACH(ptree::value_type &res, pt.get_child("System.Resources")) {
		if(std::string(res.first.data()) == "Compute") {
			Compute *c = readComputeAttributes(res);
			resources[c->getModel()] = c;
		}
		else if(std::string(res.first.data()) == "Storage") {
			Storage *s = readStorageAttributes(res);
			resources[s->getModel()] = s;
		}
		else if(std::string(res.first.data()) == "Memory") {
			Memory *m = readMemoryAttributes(res);
			resources[m->getModel()] = m;
		}
		else {
			std::string r = res.first.data();
			log->error("Unknown Resource type specified: " + r + ".");
			return false;
		}
	}
	
	uint32_t nodeid = 0;
	
	try {
		BOOST_FOREACH(ptree::value_type &arch, pt.get_child("System.Architecture")) {			
		
			PartitionList *pl = new PartitionList();

			uint32_t rep_part = 1;
			BOOST_FOREACH(const ptree::value_type &part, arch.second.get_child("")) {
				//Check if this child contains the attributes.
				if(std::string(part.first.data()) == "<xmlattr>") {
					rep_part = part.second.get<uint32_t>("rep", 1);
					continue;
				}
				else if(std::string(part.first.data()) != "Node") {
					log->error("A component other than a Node specified under Partition "
											+ std::string(part.first.data()) + ".");
				}
				
				Node *n = new Node(nodeid++);
				uint32_t rep_node = 0;
				BOOST_FOREACH(const ptree::value_type &node, part.second.get_child("")) {
					if(std::string(node.first.data()) == "<xmlattr>") {
						rep_node = node.second.get<uint32_t>("rep", 1);
						continue;
					}
					else if(std::string(node.first.data()) != "Resource") {
						log->error("A component other than a Hardware specified under Node: "
												+ std::string(node.first.data()) + ".");
					}
					
					uint32_t rep_resource = node.second.get_child("<xmlattr>").get<uint32_t>("rep", 1);
					std::string resource = node.second.get_child("<xmlattr>").get<std::string>("res", "");
					
					if(resource == "")
						log->error("Resource id/model must be specified resource's definition.");
					
					for(uint32_t i = 0; i < rep_resource; i++) {
						Resource *r_cloned = resources[resource]->clone();
						n->addResource(r_cloned);
					}
				}
				for(uint32_t i = 0; i < rep_node; i++) {
					pl->addNode(new Node(*n));
				}
				delete n;
			}
			//Add partition + multiply in case rep_partitionlist > 1.
			for(uint32_t i = 0; i < rep_part; i++)
				partitionLists.push_back(new PartitionList(*pl));
			delete pl;
		}
	}
	catch(std::exception const& e) {
		log->error("Exception caught: " + std::string(e.what()) + ".");
	}
	
	totalCPUs = 0;
	totalMemory = 0;
	//Put everything in place, i.e. tell each component where inside where it is.
	for(uint32_t i = 0; i < partitionLists.size(); i++) {
		PartitionList *pl = partitionLists[i];
		std::vector<Node*> nodes = pl->getNodesInPartitionList();
		for(uint32_t j = 0; j < nodes.size(); j++) {
			Node *n = nodes[j];
			n->setPartitionList(pl);
			std::vector<Resource*> res = n->getResourcesInNode();
			for(uint32_t r = 0; r < res.size(); r++) {
				res[r]->setNode(n);
				switch(res[r]->getType()) {
					case ArchDataModel::COMPUTE:
						totalCPUs += ((Compute*)res[r])->getTotalCPUs();
						totalMemory += ((Compute*)res[r])->getLocalMemory();
						break;
					case ArchDataModel::STORAGE:
						totalDisk += ((Storage*)res[r])->getSize();
						break;
					case ArchDataModel::MEMORY:
						totalMemory += ((Memory*)res[r])->getSize();
						break;
					default:
						assert(false);
				}
			}
		}
	}
	
	log->debug("Total number of CPUs in the system: " +to_string(totalCPUs), 1);
	log->debug("Total Memory in the system: " + ftos(totalMemory/1024/1024), 1);
	sysorch = new Simulator::SystemOrchestrator(this, startime, log);
        
//        // Test, default is false, expect this to fail.
//        if (sysorch->getOutputTrace() == true){
//            std::cout << "ARCH CONF SYSORCH TRUE" << endl;
//        }
//        else {
//            std::cout << "ARCH CONF SYSORCH FALSE" << endl;    
//        }
	sysorch->createPartitions(partitionLists);

#if 0
	try {
		BOOST_FOREACH(ptree::value_type &part, pt.get_child("System.PartitionList")) {			
	
			//iterate over partitions
			//BOOST_FOREACH(const ptree::value_type &part, arch.second.get_child("")) {
				//Check if this child contains the attributes.
				std::string name, policy, rspolicy;
				uint32_t numNodes, numCores;
				uint64_t mem, disk;
				
				try {
					const ptree & attr = part.second.get_child("");
	
					name 			= attr.get<std::string>("name", "");
					numNodes	= attr.get<uint32_t>("nodes", 0);
					numCores	= attr.get<uint32_t>("cores", 0);
					mem				= attr.get<uint64_t>("mem", 0);
					disk			= attr.get<uint64_t>("storage", 0);
					policy		= attr.get<std::string>("policy", "");
					rspolicy	= attr.get<std::string>("rspolicy", "");
				}
				catch(std::exception const& e) {
					log->error("Exception caught: " + std::string(e.what()) + ".");
				}

				std::cout << "Name: " << name << std::endl;
				std::cout << "Nodes: " << numNodes << std::endl;
				std::cout << "Cores: " << numCores << std::endl;
				std::cout << "Memory: " << mem << std::endl;
				std::cout << "Disk: " << disk << std::endl;
				std::cout << "Policy: " << policy << std::endl;
				std::cout << "RS Policy: " << rspolicy << std::endl;

				mem = mem*1024*1024;
				
				Simulator::Partition *p = new Simulator::Partition(name, numNodes, numCores, 
															mem, disk, policy,rspolicy, startime, log);
				sysorch->addPartition(p);
			//}
		}
	}
	catch(std::exception const& e) {
		log->error("Exception caught: " + std::string(e.what()) + ".");
	}	
#endif

	return true;
}

Compute *ArchitectureConfiguration::readComputeAttributes(boost::property_tree::ptree::value_type comp) {
	std::string model;
	uint32_t sockets = 0, cps = 0, tpc = 0, vpu = 0, membw = 0;
	uint64_t mem = 0;
	double freq = 0.0, voltage = 0.0, tdp = 0.0;
	
	using boost::property_tree::ptree;
	model 	= comp.second.get_child("<xmlattr>").get<std::string>("id", "");
		
	try {
		const ptree & attr = comp.second.get_child("");
	
		sockets = attr.get<uint32_t>("sockets", 0);
		cps 		= attr.get<uint32_t>("cps", 0);
		mem			= attr.get<uint64_t>("mem", 0);
		membw		= attr.get<uint32_t>("membw", 0);
		tpc			= attr.get<uint32_t>("tpc", 0);
		vpu			= attr.get<uint32_t>("vpu", 0);
		freq		= attr.get<double>("freq", 0.0);
		voltage	= attr.get<double>("voltage", 0.0);
		tdp			= attr.get<double>("tdp", 0.0);
	}
	catch(std::exception const& e) {
		log->error("Exception caught: " + std::string(e.what()) + ".");
	}

	//Mem: MB -> B as in Job description
	mem = mem*1024*1024;	

	if(sockets == 0)
		log->error("Compute \"" + model + "\" has no number of sockets specified or is 0.");
	if(cps == 0)
		log->error("Compute \"" + model + "\" has no number of cores per socket specified or is 0.");
	if(tpc == 0)
		log->error("Compute \"" + model + "\" has no number of threads per core specified or is 0.");
	
	if(model == "") {
		string scores = std::to_string(cps);
		string ssockets = std::to_string(sockets);
		model = "Compute " + ssockets + "x" + scores + " cores"; 
	}

	Compute *c = new Compute(model, sockets, cps, freq, voltage, tdp, tpc, vpu, mem, membw, NULL);
	return c;
}

Storage *ArchitectureConfiguration::readStorageAttributes(boost::property_tree::ptree::value_type stor) {
	std::string model;
	uint64_t size = 0;
	double bw = 0.0;
	
	using boost::property_tree::ptree;
	model 	= stor.second.get_child("<xmlattr>").get<std::string>("id", "");
	
	try {
		const ptree & attr = stor.second.get_child("");
		size 	= attr.get<uint64_t>("size", 0);
		bw		= attr.get<double>("bw", 0.0);
	}
	catch(std::exception const& e) {
				log->error("Exception caught: " + std::string(e.what()) + ".");
	}
	
	size = size*1024*1024;
	if(size == 0)
		log->error("Storage \"" + model + "\" has no size specified or is 0.");
	if(bw == 0.0)
		log->error("Storage \"" + model + "\" has no bw specified or is 0.");
	
	if(model == "") {
		string ssize = std::to_string(size);
		model = "Storage " + ssize + "MB";
	}

	Storage *s = new Storage(model, size, bw, NULL);
	return s;
}

Memory *ArchitectureConfiguration::readMemoryAttributes(boost::property_tree::ptree::value_type mem) {
	std::string model;
	uint64_t size = 0;
	double bw = 0.0;
        int persist = 0;
	
	using boost::property_tree::ptree;
	model 	= mem.second.get_child("<xmlattr>").get<std::string>("id", "");
	
	try {
		const ptree & attr = mem.second.get_child("");
		size 	= attr.get<uint64_t>("size", 0);
		bw		= attr.get<double>("bw", 0.0);
                persist = attr.get<int>("persist", 0);
	}
	catch(std::exception const& e) {
				log->error("Exception caught: " + std::string(e.what()) + ".");
	}

	size = size * 1024 * 1024; //GB -> KB as in Job description
	
	if(size == 0)
		log->error("Memory \"" + model + "\" has no size specified or is 0.");
	if(bw == 0.0)
		log->error("Memory \"" + model + "\" has no bw specified or is 0.");
        if(persist == 0)
            log->error("Memory \"" + model + "\" has no persistence specified or is 0 (None)");
	
	if(model == "") {
		string ssize = std::to_string(size);
		model = "Memory " + ssize + "MB"; 
	}

	Memory *s = new Memory(model, size, bw, persist, NULL);
	return s;
}

Simulator::SystemOrchestrator *ArchitectureConfiguration::getSystemOrchestrator() {
	return sysorch;
}

std::vector<PartitionList*> ArchitectureConfiguration::getPartitionListsInSystem() {
	return partitionLists;
}

void ArchitectureConfiguration::setConfigFile(std::string configFile) {
	this->configFile = configFile;
}

std::string ArchitectureConfiguration::getName() {
	return name;
}

std::string ArchitectureConfiguration::getVendor() {
	return vendor;
}

std::string ArchitectureConfiguration::getOSName() {
	return osname;
}

//TODO
std::vector<double> ArchitectureConfiguration::getFrequencies() {
	assert(false);
}

std::vector<double> ArchitectureConfiguration::getVoltages() {
	assert(false);
}

double ArchitectureConfiguration::getActivity() {
	assert(false);
}

double ArchitectureConfiguration::getPortion() {
	assert(false);
}

uint64_t ArchitectureConfiguration::getNumberCPUs() {
	return totalCPUs;
}

uint64_t ArchitectureConfiguration::getTotalMemory() {
	return totalMemory;
}

uint64_t ArchitectureConfiguration::getTotalDisk() {
	return totalDisk;
}

double ArchitectureConfiguration::getClockSpeed() {
	return clockSpeed;
}

architecture_type_t ArchitectureConfiguration::getArchitectureType() {
	return architectureType;
}

void ArchitectureConfiguration::setArchitectureType(architecture_type_t type) {
	architectureType = type;
}

}
