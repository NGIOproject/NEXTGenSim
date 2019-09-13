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
#include <scheduling/resourcebucket.h>
#include <cassert>
#include <iostream>

namespace Simulator
{

    ResourceBucket::ResourceBucket()
    {
        node = NULL;
        freeCPUs = 0;
        freeMemory = 0;
        freeMemoryBW = 0;
        freeDisk = 0;
        freeDiskBW = 0;
        freeFabricBW = 0;
        timeNextRelease = -1;
    }

    ResourceBucket::ResourceBucket(ArchDataModel::Node *node, double startTime, double endTime)
    {
        this->node = node;
        this->freeCPUs = node->getNumberCPUs();
        this->freeMemory = node->getMemorySize();
        this->freeMemoryBW = node->getMemoryBW();
        this->freeDisk = node->getDiskSize();
        this->freeDiskBW = node->getDiskBW();
        this->freeFabricBW = node->getFabricBW();
        this->startTime = startTime;
        this->endTime = endTime;
        this->timeNextRelease = -1;
    }

    ResourceBucket::ResourceBucket(const ResourceBucket& rb)
    {
        this->node = rb.getNode();
        this->freeCPUs = rb.getFreeCPUs();
        this->freeMemory = rb.getFreeMemory();
        this->freeMemoryBW = rb.getFreeMemoryBW();
        this->freeDisk = rb.getFreeDisk();
        this->freeDiskBW = rb.getFreeDiskBW();
        this->freeFabricBW = rb.getFreeFabricBW();
        this->jobReservation = rb.getJobReservations();
        this->startTime = rb.getStartTime();
        this->endTime = rb.getEndTime();
        this->timeNextRelease = rb.getTimeNextRelease();
        this->id = rb.getID();
    }

    ResourceBucket::ResourceBucket(const ReservationList &rl, double start, double end)
    {
        node = NULL;
        freeCPUs = rl.cpus;
        freeMemory = rl.mem;
        freeDisk = rl.disk;
        freeMemoryBW = rl.memBW;
        freeDiskBW = rl.diskBW;
        freeFabricBW = rl.fabBW;
        startTime = start;
        endTime = end;
        this->timeNextRelease = -1;
    }

    ResourceBucket::ResourceBucket(double start, double end)
    {
        node = NULL;
        startTime = start;
        endTime = end;
        this->timeNextRelease = -1;
    }

    ResourceBucket::~ResourceBucket()
    {
        //if(jobReservation.size() > 0) {
        //				std::cout << "Deleting bucket: " << (void*)this << std::endl;
        //for(std::map<Job*, ReservationList>::iterator it = jobReservation.begin(); it != jobReservation.end(); it++)
        //	std::cout << "Job: " << (void*)(*it).first << std::endl;
        //assert(false);
        //}
    }

    bool ResourceBucket::operator==(const ResourceBucket& rb)
    {
        if (this->freeCPUs != rb.getFreeCPUs() or
            this->freeMemory != rb.getFreeMemory() or
            this->freeMemoryBW != rb.getFreeMemoryBW() or
            this->freeDisk != rb.getFreeDisk() or
            this->freeDiskBW != rb.getFreeDiskBW() or
            this->freeFabricBW != rb.getFreeFabricBW())
            return false;

        //if they have same attributes, check that jobs allocated are the same
        std::map<Job*, ReservationList> rblist = rb.getJobReservations();
        if (this->jobReservation.size() != rblist.size())
            return false;

        std::map<Job*, ReservationList>::iterator it = this->jobReservation.begin();
        for (; it != this->jobReservation.end(); it++) {
            if (rblist.find(it->first) == rblist.end()) //job not found on rb's list.
                return false;
        }
        return true;
    }

    bool ResourceBucket::operator<(const ResourceBucket& rb)
    {
        if (this->freeCPUs < rb.getFreeCPUs() or
            this->freeMemory < rb.getFreeMemory() or
            this->freeMemoryBW < rb.getFreeMemoryBW() or
            this->freeDisk < rb.getFreeDisk() or
            this->freeDiskBW < rb.getFreeDiskBW() or
            this->freeFabricBW < rb.getFreeFabricBW())
            return true;
        else
            return false;
    }

    bool ResourceBucket::operator>(const ResourceBucket& rb)
    {
        if (this->freeCPUs > rb.getFreeCPUs() or
            this->freeMemory > rb.getFreeMemory() or
            this->freeMemoryBW > rb.getFreeMemoryBW() or
            this->freeDisk > rb.getFreeDisk() or
            this->freeDiskBW > rb.getFreeDiskBW() or
            this->freeFabricBW > rb.getFreeFabricBW())
            return true;
        else
            return false;
    }

    ResourceBucket& ResourceBucket::operator+=(const ResourceBucket& rb)
    {
        assert(node == rb.getNode());
        this->freeCPUs += rb.getFreeCPUs();
        this->freeMemory += rb.getFreeMemory();
        this->freeMemoryBW += rb.getFreeMemoryBW();
        this->freeDisk += rb.getFreeDisk();
        this->freeDiskBW += rb.getFreeDiskBW();
        this->freeFabricBW += rb.getFreeFabricBW();

        return *this;
    }

    ResourceBucket& ResourceBucket::operator/=(const ResourceBucket& rb)
    {
        assert(node == rb.getNode());
        this->freeCPUs = (this->freeCPUs <= rb.getFreeCPUs()) ? this->freeCPUs : rb.getFreeCPUs();
        this->freeMemory = (this->freeMemory <= rb.getFreeMemory()) ? this->freeMemory : rb.getFreeMemory();
        this->freeMemoryBW = (this->freeMemoryBW <= rb.getFreeMemoryBW()) ? this->freeMemoryBW : rb.getFreeMemoryBW();
        this->freeDisk = (this->freeDisk <= rb.getFreeDisk()) ? this->freeDisk : rb.getFreeDisk();
        this->freeDiskBW = (this->freeDiskBW <= rb.getFreeDiskBW()) ? this->freeDiskBW : rb.getFreeDiskBW();
        this->freeFabricBW = (this->freeFabricBW <= rb.getFreeFabricBW()) ? this->freeFabricBW : rb.getFreeFabricBW();

        return *this;
    }

    ReservationList ResourceBucket::getResourceList()
    {
        ReservationList rl = {.cpus = freeCPUs, .disk = freeDisk,
            .mem = freeMemory, .memBW = freeMemoryBW,
            .diskBW = freeDiskBW, .fabBW = freeFabricBW};
        return rl;
    }

    void ResourceBucket::setID(uint32_t id)
    {
        this->id = id;
    }

    uint32_t ResourceBucket::getID() const
    {
        return id;
    }

    void ResourceBucket::setNode(ArchDataModel::Node *node)
    {
        this->node = node;
    }

    ArchDataModel::Node *ResourceBucket::getNode() const
    {
        return node;
    }

    void ResourceBucket::setStartTime(double startTime)
    {
        this->startTime = startTime;
    }

    void ResourceBucket::setEndTime(double endTime)
    {
        this->endTime = endTime;
    }

    double ResourceBucket::getStartTime() const
    {
        return startTime;
    }

    double ResourceBucket::getEndTime() const
    {
        return endTime;
    }

    uint32_t ResourceBucket::getFreeCPUs() const
    {
        return freeCPUs;
    }

    uint64_t ResourceBucket::getFreeMemory() const
    {
        return freeMemory;
    }

    uint64_t ResourceBucket::getFreeDisk() const
    {
        return freeDisk;
    }

    double ResourceBucket::getFreeMemoryBW() const
    {
        return freeMemoryBW;
    }

    double ResourceBucket::getFreeDiskBW() const
    {
        return freeDiskBW;
    }

    double ResourceBucket::getFreeFabricBW() const
    {
        return freeFabricBW;
    }

    std::map<Job*, ReservationList> ResourceBucket::getJobReservations() const
    {
        return jobReservation;
    }

    bool ResourceBucket::deallocateJob(Job* job)
    {
        if (jobReservation.find(job) == jobReservation.end())
            return false;

        ReservationList rl = jobReservation[job];
        freeCPUs += rl.cpus;
        freeMemory += rl.mem;
        freeDisk += rl.disk;
        freeMemoryBW += rl.memBW;
        freeDiskBW += rl.diskBW;
        freeFabricBW += rl.fabBW;

        std::map<Job*, ReservationList>::iterator it = jobReservation.find(job);
        jobReservation.erase(it);
        /* Not using this for the moment. Inherited from AnalogicalBucket.
  this->usePenalizedTime = false;
  this->penalizedTime = 0;
  this->penalizedEndTime = -1;
         */

        return true;
    }

    void ResourceBucket::allocateJob(Job *job, ReservationList rl)
    {

        assert(endTime != -1);
        assert(rl.cpus <= freeCPUs);
        assert(rl.mem <= freeMemory);
        assert(rl.disk <= freeDisk);
        assert(rl.memBW <= freeMemoryBW);
        assert(rl.diskBW <= freeDiskBW);
        assert(rl.fabBW <= freeFabricBW);

        if (jobReservation.find(job) == jobReservation.end()) {
            freeCPUs -= rl.cpus;
            freeMemory -= rl.mem;
            freeDisk -= rl.disk;
            freeMemoryBW -= rl.memBW;
            freeDiskBW -= rl.diskBW;
            freeFabricBW -= rl.fabBW;

            jobReservation[job] = rl;
        }
        else {
            ReservationList orl = jobReservation[job];

            //cout <<"orl.cpus = " << orl.cpus << endl;
            orl.cpus += rl.cpus;
            orl.mem += rl.mem;
            orl.disk = rl.disk;
            orl.memBW = rl.memBW;
            orl.diskBW = rl.diskBW;
            orl.fabBW = rl.fabBW;

            jobReservation[job].cpus = orl.cpus;

            freeCPUs -= rl.cpus;
            freeMemory -= rl.mem;
            freeDisk -= rl.disk;
            freeMemoryBW -= rl.memBW;
            freeDiskBW -= rl.diskBW;
            freeFabricBW -= rl.fabBW;
        }
    }

    bool ResourceBucket::expandJob(Job *job, ReservationList rl)
    {

        if (jobReservation.find(job) == jobReservation.end())
            return false;

        assert(endTime != -1);
        assert(rl.cpus <= freeCPUs);
        assert(rl.mem <= freeMemory);
        assert(rl.disk <= freeDisk);
        assert(rl.memBW <= freeMemoryBW);
        assert(rl.diskBW <= freeDiskBW);
        assert(rl.fabBW <= freeFabricBW);

        if (jobReservation.find(job) == jobReservation.end())
            return false;

        ReservationList orl = jobReservation[job];

        //cout <<"orl.cpus = " << orl.cpus << endl;
        orl.cpus += rl.cpus;
        orl.mem += rl.mem;
        orl.disk = rl.disk;
        orl.memBW = rl.memBW;
        orl.diskBW = rl.diskBW;
        orl.fabBW = rl.fabBW;

        jobReservation[job].cpus = orl.cpus;

        freeCPUs -= rl.cpus;
        freeMemory -= rl.mem;
        freeDisk -= rl.disk;
        freeMemoryBW -= rl.memBW;
        freeDiskBW -= rl.diskBW;
        freeFabricBW -= rl.fabBW;

        return true;
    }

    bool ResourceBucket::shrinkJob(Job* job, int &scpus, int &remcpus)
    {
        int tscpus;

        if (jobReservation.find(job) == jobReservation.end())
            return false;

        if (jobReservation[job].cpus > (unsigned int)scpus) {
            tscpus = scpus;
        }
        else {
            tscpus = jobReservation[job].cpus;
        }

        scpus -= tscpus;
        freeCPUs += tscpus;
        jobReservation[job].cpus -= tscpus;
        remcpus = jobReservation[job].cpus;

        if (jobReservation[job].cpus == 0) {
            ReservationList rl = jobReservation[job];
            freeCPUs += rl.cpus;
            freeMemory += rl.mem;
            freeDisk += rl.disk;
            freeMemoryBW += rl.memBW;
            freeDiskBW += rl.diskBW;
            freeFabricBW += rl.fabBW;
            std::map<Job*, ReservationList>::iterator it = jobReservation.find(job);
            jobReservation.erase(it);
        }

        return true;
    }

    uint32_t ResourceBucket::getJobUsedCPUs(Job *job)
    {
        if (jobReservation.find(job) == jobReservation.end())
            return 0;
        ReservationList rl = jobReservation[job];
        return rl.cpus;
    }

    void ResourceBucket::setTimeNextRelease(double time)
    {
        timeNextRelease = time;
    }

    double ResourceBucket::getTimeNextRelease() const
    {
        return timeNextRelease;
    }
}
