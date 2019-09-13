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
#ifndef RESOURCEBUCKET_H
#define RESOURCEBUCKET_H

#include <scheduling/job.h>
#include <archdatamodel/node.h>
#include <map>

namespace Simulator {

/* This struct represents the list of resources reserved by a job within a Resource. */
struct ReservationList {
	uint32_t cpus;
	uint64_t disk;
	uint64_t mem;
	double memBW;
	double diskBW;
	double fabBW; 
};

/* This struct represents the list of resource requirements of a job in order to find a suitable resource allocation. */
struct ResourceRequirements {
	uint32_t cpus;
	uint32_t cpusPerNode;
	uint64_t mem;
	uint32_t memPerNode;
	uint64_t disk;
	uint32_t diskPerNode;
	double diskBW;
	double memBW;
	double fabBW;
	
	bool operator<(ResourceRequirements& rr) {
		if(this->cpus < rr.cpus or
		this->mem < rr.mem or
		this->disk < rr.disk or
		this->memBW < rr.memBW or
		this->diskBW < rr.diskBW or
		this->fabBW < rr.fabBW)
			return true;
		return false;
	}
};


/**
	* @author Daniel Rivas, daniel.rivas@bsc.es
**/

/**
* This class implements a resource bucket. This is a class that represents 
* the reservation state of a resource in a given interval of time. 
*/


class ResourceBucket {
public:
	ResourceBucket();
	ResourceBucket(ArchDataModel::Node *node, double startTime, double endTime);
	ResourceBucket(double startTime, double endTime);
	ResourceBucket(const ResourceBucket &rb);
	ResourceBucket(const ReservationList &rl, double startTime, double endTime); //In this case the RB is used to indicate resources to be reserved.
	~ResourceBucket();
	
	bool operator==(const ResourceBucket& rb);
	bool operator<(const ResourceBucket& rb); //Returns true if this has at least one free resource with a lower value.
	bool operator>(const ResourceBucket& rb); //Returns true if this has at least one free resource with a higher value.
	ResourceBucket& operator+=(const ResourceBucket& rb);
	ResourceBucket& operator/=(const ResourceBucket& rb); //returns the common minimum of free resources
	
	void setID(uint32_t id);
	uint32_t getID() const;
	
	void setStartTime(double startTime);
  double getStartTime() const;
  void setEndTime(double endTime);
  double getEndTime() const;
	void setNode(ArchDataModel::Node *node);
	ArchDataModel::Node *getNode() const;
	
	//Get resource's info
	uint32_t getFreeCPUs() const;
	uint64_t getFreeMemory() const;
	uint64_t getFreeDisk() const;
	double getFreeMemoryBW() const;
	double getFreeDiskBW() const;
	double getFreeFabricBW() const;

  uint32_t getJobUsedCPUs(Job *job);

	std::map<Job*, ReservationList> getJobReservations() const;
	ReservationList getResourceList(); //Builds a ReservationList from the free resources of the bucket.
	bool deallocateJob(Job *job);
	void allocateJob(Job *job, ReservationList rl);
	bool shrinkJob(Job* job, int &scpus, int &remcpus);
	bool expandJob(Job *job, ReservationList rl);	
	void setTimeNextRelease(double time);
	double getTimeNextRelease() const;
	
private:
	ArchDataModel::Node *node; /**< Pointer to the Node whose state is represented by the Resource Bucket. */
	uint32_t id;
	//Resources
	uint32_t freeCPUs;
	uint64_t freeMemory;
	double freeMemoryBW;
	uint64_t freeDisk;
	double freeDiskBW;
	double freeFabricBW;
	
	std::map<Job*, ReservationList> jobReservation; /* Map to know what was exactly reserved by a given job. */
	
	double timeNextRelease; /* Start time of the next release of resources. Used when trying to find a bucket to allocate. */

	double startTime;
	double endTime;
};

}

#endif
