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
#include <scheduling/systemorchestrator.h>
#include <scheduling/partition.h>
#include <scheduling/resourcereservationtable.h>
#include <scheduling/fcfsschedulingpolicy.h>
#include <scheduling/slurmschedulingpolicy.h>
#include <scheduling/resourceschedulingpolicy.h>
#include <scheduling/resourcereservationtable.h>
#include <archdatamodel/node.h>
#include <utils/log.h>
#include <cassert>
#include <iostream>
#include <cmath> //Needed for INFINITY macaro

namespace Simulator {

    SystemOrchestrator::SystemOrchestrator(Utils::ArchitectureConfiguration *archConf, double startTime, Utils::Log* log) {
        this->archConf = archConf;
        this->log = log;
        this->globalTime = startTime;
        //        cout << "SysOrch::SysOrch constructor calling to create a new RRT" << endl;
        this->rt = new ResourceReservationTable(log, this->globalTime);
        this->simulatePartitions = false;
        this->reserveFullNode = false;
        this->policy = NULL;
        this->generateOutputTrace = false; // Store this here for ease, can be had from archconf via simconfig too
    }

    void SystemOrchestrator::setPolicy(ResourceSchedulingPolicy *policy) {
        this->policy = policy;
        this->policyType = policy->getschedulingPolicySimulated();
        this->rspolicyType = policy->getRS_policyUsed();
        this->policy->setReservationTable((ReservationTable*) rt);
        this->policy->globalTime = globalTime;
        if (simulatePartitions) {
            for (uint32_t i = 0; i < partitions.size(); i++) {
                ResourceSchedulingPolicy *policyPart = createPolicy(policyType, rspolicyType, globalTime, log);
                (partitions[i])->setSchedulingPolicy(policyPart);
            }
        }
    }

    ResourceSchedulingPolicy *SystemOrchestrator::getPolicy() {
        return policy;
    }

    Utils::ArchitectureConfiguration *SystemOrchestrator::getArchitecture() {
        return archConf;
    }
    
    Utils::SimulationConfiguration *SystemOrchestrator::getsimuConfig() {
        return simuConfig;
    }
    
    void SystemOrchestrator::setsimuConfig(Utils::SimulationConfiguration *simuc) {
        this->simuConfig = simuc;
    }
    

    void SystemOrchestrator::setLog(Utils::Log *log) {
        this->log = log;
    }

    void SystemOrchestrator::createPartitions(std::vector<ArchDataModel::PartitionList*> partitionlists) {

        for (uint32_t pl = 0; pl < partitionlists.size(); pl++) {
            Simulator::Partition *p = new Simulator::Partition(startTime, log);
            addPartition(p);
            std::vector<ArchDataModel::Node*> nodes = partitionlists[pl]->getNodesInPartitionList();

            for (uint32_t n = 0; n < nodes.size(); n++) {

                /*
                 * This next line was in the original, but really, the better way would be to *always* simulate partitions
                 * and accept that you get an overhead if you only have 1 partition.
                 * It would simplify things. A lot.
                 */
                rt->addNode(nodes[n]); // Add the nodes to the SysOrch RRT
                p->addNode(nodes[n]); // This will internally add the nodes to the individual partition's RRT
            }
        }
        // A Testing cheat, if only 1 P, just abuse its RRT as the system one.
//        if (this->partitions.size() == 1) {
//            Simulator::Partition *temp = this->partitions[0];
//            //this->rt = temp->getReservationTable();
//        }

    }

    ResourceSchedulingPolicy *SystemOrchestrator::createPolicy(policy_type_t pType, RS_policy_type_t rspType, double startTime, Utils::Log *log) {

        switch (pType) {
            case FCFS:
            {
                log->debug("Creating a FCFS policy with no resource usage modelling.", 1);
                //TODO: Use simulateResourceUsage
                bool simulateResourceUsage = false;
                int numberOfReservations = this->policy->numberOfReservations;
                bool malexpand = this->policy->malleableExpand;
                bool malshrink = this->policy->malleableShrink;
                FCFSSchedulingPolicy *fcfs = new FCFSSchedulingPolicy(archConf, log, startTime, simulateResourceUsage, numberOfReservations, malexpand, malshrink);
                fcfs->setRS_policyUsed(rspType);
                fcfs->setGenOuputTrace(this->generateOutputTrace); // Push the paraver setting down to the RS policy which will call out at the right times for tracing.                       
                //TODO: set other parameters: emulateCPUFactor, maxAllowedRuntime, costAllocationUnit.
                return (ResourceSchedulingPolicy*) fcfs;
                break;
            }
            case SLURM:
            {
                log->debug("Creating a SLURM policy with no resource usage modelling.", 1);
                bool simulateResourceUsage = false;
                int numberOfReservations = this->policy->numberOfReservations;
                bool malexpand = this->policy->malleableExpand;
                bool malshrink = this->policy->malleableShrink;
                SLURMSchedulingPolicy *slurm = new SLURMSchedulingPolicy(archConf, log, startTime, simulateResourceUsage, numberOfReservations, malexpand, malshrink);
                slurm->setRS_policyUsed(rspType);
                slurm->setGenOuputTrace(this->generateOutputTrace); // Push the paraver setting down to the RS policy which will call out at the right times for tracing.                       
                //TODO: set other parameters: emulateCPUFactor, maxAllowedRuntime, costAllocationUnit.
                return (ResourceSchedulingPolicy*) slurm;
                break;
            }
            default:
                assert(false);
        }
        return NULL;

    }

    void SystemOrchestrator::setFirstStartTime(double time) {
        startTime = time;
    }

    double SystemOrchestrator::getFirstStartTime() {
        return startTime;
    }

    //This function will propagate the new globalTime

    void SystemOrchestrator::setGlobalTime(uint64_t globalTime) {
        this->globalTime = globalTime;
        if (policy != NULL)
            policy->globalTime = globalTime;
        if (simulatePartitions)
            for (uint32_t i = 0; i < partitions.size(); i++) {
                partitions[i]->setGlobalTime(globalTime);
            }
        rt->setGlobalTime(globalTime);
        //if(this->policy != NULL) policy->setGlobalTime(globalTime);
    }

    void SystemOrchestrator::addPartition(Partition *p) {
        partitions.push_back(p);

        //TODO: Is there something else to do when a partition is added? Call fillPartitions?
    }

    uint32_t SystemOrchestrator::getNumberPartitions() {
        return partitions.size();
    }

    Partition *SystemOrchestrator::getPartition(uint32_t id) {
        assert(id < partitions.size());
        return partitions[id];
    }

    Partition *SystemOrchestrator::getPartition(std::string id) {
        for (uint32_t i = 0; i < partitions.size(); i++) {
            if (partitions[i]->getName() == id) return partitions[i];
        }
        assert(false);
        return NULL;
    }

    Partition *SystemOrchestrator::selectPartition(Job *job) {
        if (job->getPartitionNumber() != -1) {
            int partNum = job->getPartitionNumber();
            return partitions[partNum - 1];
        } else {
            return partitions[0];
        }
    }

    //Scheduling related functions

    void SystemOrchestrator::arrival(Job *job) {
        if (!simulatePartitions) {
            policy->jobArrive(job);
        } else {
            Partition *selected = selectPartition(job);
            job2part[job] = selected;
            selected->arrival(job);
        }
    }

    void SystemOrchestrator::start(Job *job) {
        if (!simulatePartitions) {
            policy->jobStart(job);
        } else {
            Partition *p = job2part[job];
            p->start(job);
        }
    }

    void SystemOrchestrator::schedule() {
        if (!simulatePartitions) {
            policy->schedule();
        } else {
            for (uint32_t i = 0; i < partitions.size(); i++) {
                partitions[i]->schedule();
            }
        }
    }
    
    
    void SystemOrchestrator::backfill() {
        if (!simulatePartitions) {
            policy->backfill();
        } else {
            for (uint32_t i = 0; i < partitions.size(); i++) {
                partitions[i]->backfill();
            }
        }
    }

    void SystemOrchestrator::termination(Job *job) {
        if (!simulatePartitions) {
            policy->jobFinish(job);
        } else {
            Partition *p = job2part[job];
            if (p == NULL)
                cout << "P is NULL for job " << job->getJobNumber() << endl;
            p->termination(job);
            job2part.erase(job);
        }
    }

    void SystemOrchestrator::abnormalTermination(Job *job) {
        termination(job);
    }

    /**
     * This function is called before start the simulation , the scheduler may decide to do some init stuff.
     */
    void SystemOrchestrator::PreProcessScheduling() {

    }

    /**
     * This function is called after the simulation , the scheduler may decide to do some init stuff.
     */
    void SystemOrchestrator::PostProcessScheduling() {
        std::cout << "Waiting queue sizes: " << std::endl;
        if (simulatePartitions)
            for (uint32_t i = 0; i < partitions.size(); i++) {
                FCFSSchedulingPolicy *s = (FCFSSchedulingPolicy*) partitions[i]->getSchedulingPolicy();
                std::cout << "Partition: " << partitions[i]->getName() << std::endl;
                std::cout << "Size: " << s->getWQSize() << std::endl << std::endl;
            }

    }

    void SystemOrchestrator::setSim(Simulation* simulator) {
        policy->setSim(simulator);
        this->sim = simulator;
        if (simulatePartitions)
            for (uint32_t i = 0; i < partitions.size(); i++) {
                partitions[i]->setSim(simulator);
            }
    }

    void SystemOrchestrator::setReserveFullNode(bool rfn) {
        reserveFullNode = rfn;
    }

    bool SystemOrchestrator::getReserveFullNode() {
        return this->reserveFullNode;
    }

    void SystemOrchestrator::setSimulatePartitions(bool simp) {
        simulatePartitions = simp;
    }

    void SystemOrchestrator::setOutputTrace(bool theValue) {
        this->generateOutputTrace = theValue;
    }

    bool SystemOrchestrator::getOutputTrace() {
        return this->generateOutputTrace;
    }
    
    void SystemOrchestrator::transitiontoCompute(Job* job) {
        if (!simulatePartitions) {
            policy->jobTransitionToCompute(job);
        } else {
            Partition *p = job2part[job];
            if (p == NULL) {
                cout << "P is NULL for job " << job->getJobNumber() << endl;
            }
            p->transitionToCompute(job);
        }
    }
    
    void SystemOrchestrator::transitiontoOutput(Job* job) {
        if (!simulatePartitions) {
            policy->jobTransitionToOutput(job);
        } else {
            Partition *p = job2part[job];
            if (p == NULL) {
                cout << "P is NULL for job " << job->getJobNumber() << endl;
            }
            p->transitionToOutput(job);
        }        
    }


}
