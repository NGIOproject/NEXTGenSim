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
#include <scheduling/partition.h>
#include <scheduling/resourceschedulingpolicy.h>
#include <scheduling/resourcereservationtable.h>

namespace Simulator {

    Partition::Partition(double startTime, Utils::Log *log) {
        this->name = "";
        this->numNodes = 0;
        this->cpus = 0;
        this->mem = 0;
        this->disk = 0;
        this->policyType = FCFS;
        this->rspolicyType = FF_AND_FCF;
        this->globalTime = 0;
        this->log = log;
        this->rt = new ResourceReservationTable(log, globalTime);
        this->policy = NULL;
    }

    void Partition::setGlobalTime(uint64_t globalTime) {
        this->globalTime = globalTime;
        this->rt->setGlobalTime(globalTime);
        if (policy != NULL)
            this->policy->setGlobalTime(globalTime);
    }

    ResourceReservationTable *Partition::getReservationTable() {
        return rt;
    }

    void Partition::setReservationTable(ResourceReservationTable *rt) {
        this->log->debug("Partition::setRT");
        cout << "Partition::setRT (cout)" << endl;
        this->rt = rt;
    }

    policy_type_t Partition::getPolicyType(std::string spolicy) {
        assert(spolicy != "");
        policy_type_t ret = OTHER_POLICY;

        if (spolicy == "FCFS"){
            ret = FCFS;
        } else if (spolicy == "SLURM"){
            ret = SLURM;
        } else {
            ret = OTHER_POLICY;
        }
        
        return ret;       
    }

    policy_type_t Partition::getPolicyType() {
        return policyType;
    }

    RS_policy_type_t Partition::getRSPolicyType(std::string rspolicy) {
        assert(rspolicy != "");

        if (rspolicy == "FF_AND_FCF")
            return FF_AND_FCF;
        else {
            std::cout << "Unknown RS policy used: " << rspolicy << std::endl;
            assert(false);
        }
        //add other supported resource selection policies.
    }

    RS_policy_type_t Partition::getRSPolicyType() {
        return rspolicyType;
    }

    ResourceSchedulingPolicy *Partition::getSchedulingPolicy() {
        return policy;
    }

    void Partition::setSchedulingPolicy(ResourceSchedulingPolicy *policy) {
        this->policy = policy;
        this->policyType = policy->getschedulingPolicySimulated();
        this->rspolicyType = policy->getRS_policyUsed();
        this->policy->setReservationTable(rt);
    }

    void Partition::addNode(ArchDataModel::Node *node) {
        //    cout << __PRETTY_FUNCTION__ << endl;
        nodes.insert(node);
        rt->addNode(node);
        numNodes++;
        cpus = node->getNumberCPUs();
        mem = node->getMemorySize();
        disk = node->getDiskSize();
    }

    ArchDataModel::Node *Partition::removeNode(uint32_t nodeid) {
        assert(false);
        /*std::set<ArchDataModel::Node*>::iterator n = nodes.find(nodeid);
        if(n != nodes.end())
                nodes.erase(n);
        else
                return NULL;*/
    }

    std::string Partition::getName() {
        return name;
    }

    uint32_t Partition::getNumNodes() {
        return numNodes;
    }

    uint32_t Partition::getTotalCPUs() {
        return cpus*numNodes;
    }

    uint32_t Partition::getCPUsPerNode() {
        return cpus;
    }

    uint64_t Partition::getTotalMemory() {
        return mem*numNodes;
    }

    uint64_t Partition::getMemoryPerNode() {
        return mem;
    }

    uint64_t Partition::getTotalDisk() {
        return disk*numNodes;
    }

    uint64_t Partition::getDiskPerNode() {
        return disk;
    }

    //Scheduling related functions

    void Partition::arrival(Job *job) {
        policy->jobArrive(job);
    }

    void Partition::start(Job *job) {
        policy->jobStart(job);
    }

    void Partition::termination(Job *job) {
        policy->jobFinish(job);
    }

    void Partition::schedule() {
        policy->schedule();
    }

    void Partition::backfill() {
        policy->backfill();
    }

    void Partition::transitionToCompute(Job* job){
        policy->jobTransitionToCompute(job);
    }
    
    void Partition::transitionToOutput(Job* job){
        policy->jobTransitionToOutput(job);
    }

    void Partition::setSim(Simulation* simulator) {
        sim = simulator;
        policy->setSim(simulator);
    }

}
