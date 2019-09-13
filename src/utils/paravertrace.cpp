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
#include <utils/paravertrace.h>
#include <assert.h>
#include <utils/utilities.h>
#include <time.h>
#include <fstream>
#include <archdatamodel/node.h>
#include <archdatamodel/partitionlist.h>
#include <utils/architectureconfiguration.h>

#include <math.h>

#include "scheduling/systemorchestrator.h"
#include "scheduling/partition.h"
#include "scheduling/resourcereservationtable.h"
#include "scheduling/schedulingpolicy.h"
#include "scheduling/resourceschedulingpolicy.h"

namespace Utils
{

    /**
     * The constructor for the class that should be used
     * @param trace_name The filename for the trace files
     * @param dir The directory where the files have to be stored
     * @param total_jobs The total jobs that will be included in the trace
     * @param architecture The architecture that has been simulated
     * @param archType The architecture type used
     */
    ParaverTrace::ParaverTrace(string trace_name, string dir, int total_jobs, ArchitectureConfiguration* architecture)
    {
        this->dir = expandFileName(dir);

        //Need also to replace
        if (dir.length() == 0 && trace_name.length() == 0)
            //ok no folder provided, we assume dir as a cfg
            this->filepath = "paraver_simulation.prv";
        else
            this->filepath = dir + "/" + trace_name + ".prv";

        cout << "Creating a paraver generator " << this->filepath << endl;

        this->trace_name = trace_name;
        this->lastid = 0;
        //by default there is no log
        this->log = 0;

        this->total_jobs = 0;
        this->architecture = architecture;

        this->startJobTrace = -1;
        this->endJobTrace = -1;

        //Creating the paraver header.
        //The ftime is the total time of the trace, in our case is computed with the last arrival in the simulation;
        //obviously we do not know how much will time will take the overall simulation time, however we approximate it, by adding to the
        //provided time the running time for the last job (this amount have to be included in the parameter)


        if (date == "") {
            time_t tim = time(NULL);
            struct tm *tmsp = localtime(&tim);

            this->date = to_string(tmsp->tm_mday) + "/" + to_string(tmsp->tm_mon) + "/" + to_string(tmsp->tm_year + 1900) + " at " + to_string(tmsp->tm_hour) + ":" + to_string(tmsp->tm_min);
        }

        this->upperJob = 1;
        cout << this->date << endl;
        cout << "Total jobs = " << total_jobs << endl;
        // This is hacky, probably missing something here, like working in a window region rather than the whole trace...
        this->total_jobs = total_jobs;
    }

    /**
     * The default destructor for the class
     */
    ParaverTrace::~ParaverTrace()
    {

    }

/**
     * Generates the traces files with the jobs that have been simulated.
     */
    void ParaverTrace::close()
    {
        generateNamesConfigFile();
        generateArchitectureDefinition();

        
        this->paraverHeader = "#Paraver(" + this->date + "):" + ftos(this->last_arrival + 30) + ":" + architectureDefintion + ":" + to_string(this->total_jobs) + this->paraverHeader;
        cout << "ARCH DEFN:" << endl << architectureDefintion << endl;

        for (int i = 0; i <= this->upperJob; i++) {
            map<int, string>::iterator it = this->paraverHeaderEntries.find(i);

            if (it == this->paraverHeaderEntries.end())
                continue;

            this->paraverHeader += it->second;
        }


        cout << "Creating paraver file " << this->filepath << endl;

        std::ofstream fout(this->filepath.c_str(), ios::trunc);

        if (!fout) {
            cout << " Error when creating the file " << this->filepath.c_str() << endl;
            exit(1);
        }

        fout << this->paraverHeader << endl;
        log->debug("The PARAVER IS: " + this->paraverHeader, 2);
        for (ParaverEntriesSet::iterator it = this->paraverBody.begin(); it != this->paraverBody.end(); ++it) {
            ParaverEntry* en = *it;
            fout << en->entry << endl;
            log->debug(en->entry, 2);
        }

        fout.close();

    }

/**
     * Indicates to the class that a job has started in the provided allocation.  The function generates the events concerning the job.
     * @param job_id The job id
     * @param allocations The set of buckets containing where the processes for the job have been allocated
     *
     * We do not need the end of the job as this should contain it all.
     */
    void ParaverTrace::ParaverJobRuns(int job_id, deque<ResourceBucket*>& allocations)
    {

        //        cout << "PARAVER JOB "<< to_string(job_id) << " RUNS " << allocations.size() << endl;
        for (deque<ResourceBucket*>::iterator it = allocations.begin(); it != allocations.end(); ++it) {
            cout << "Job " << to_string(job_id) << " running on " << to_string((*it)->getID()) << "start: " << to_string((*it)->getStartTime()) << " ends at " << to_string((*it)->getEndTime()) << endl;
            cout << "1:" + to_string((*it)->getID()) + ":1:1:" +  to_string((*it)->getStartTime()) + ":" + to_string((*it)->getEndTime()) + ":1" << endl;
//            log->debug("Job " + to_string(job_id) + " running on " + to_string((*it)->getID()) + "start: " + to_string((*it)->getStartTime()) + " ends at " + to_string((*it)->getEndTime()), 2);


        }

//        if (this->log == 0) {
//            cout << "JobRun: Log 0" << endl;
//        }
        log->debug("ParaVerJobRuns " + to_string(job_id) + "done.", 2);
//        this->paraverBody.insert(new ParaverEntry("1:" + to_string(*it->getID()) + ":1:1" +  to_string((*it)->getStartTime()) + ":" + to_string((*it)->getEndTime()) + ":1"      ));



        //We add the job if it belongs to the desired interval
//        if ((job_id < this->startJobTrace && this->startJobTrace != -1) || (job_id > this->endJobTrace && this->endJobTrace != -1))
//            return;


//        // store application id for this job
//        this->idJob2Applid[job_id] = this->upperJob;
//      cout << "Tot jobs ups" << endl;
//        this->total_jobs++;
//
//        //We add the events for running job.
//        //At this time the given job acquires the number of cpus that it requires.
//        //There should be no conflicts due to the the scheduler has to maintain one cpu per process
//        int task = 1;
//
        int jobsize = allocations.size();

        string paraverHeaderEntry = ":" + to_string(jobsize) + "(";

        bool first = true;

        for (deque<ResourceBucket*>::iterator it = allocations.begin(); it != allocations.end(); ++it) {
            ResourceBucket* bucket = *it;
//
//            // event traces
//
//            this->paraverBody.insert(new ParaverEntry("2:0:" + to_string(this->upperJob) + ":" + to_string(task) +
//                                                      ":1:" + ftos(bucket->getJob()->getJobSimSubmitTime()) + ":" +
//                                                      to_string(JOB_STATE_EVENT_TYPE) + ":" + to_string(JOB_STATE_SUBMIT_EVENT),
//                                                      bucket->getJob()->getJobSimSubmitTime(), ++this->lastid));
//
//
//            this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu() + 1) + ":" +
//                                                      to_string(this->upperJob) + ":" + to_string(task) + ":1:" + ftos(bucket->getStartTime()) + ":" +
//                                                      to_string(JOB_STATE_EVENT_TYPE) + ":" + to_string(JOB_STATE_START_EVENT),
//                                                      bucket->getStartTime(), ++this->lastid));
//
////            if (bucket->getJob()->getBWMemoryUsed() != -1) {
////                this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu() + 1) + ":" +
////                                                          to_string(this->upperJob) + ":" + to_string(task) + ":1:" +
////                                                          ftos(bucket->getStartTime()) + ":" + to_string(JOB_BWMEM_EVENT_TYPE) + ":" +
////                                                          ftos(bucket->getJob()->getBWMemoryUsed()), bucket->getStartTime(), ++this->lastid));
////            }
//
//            // record trace
            /*
             * recall, Paraver doesn't like 0 as a node number, so add 1 to getID calls
             */
            this->paraverBody.insert(new ParaverEntry("1:" + to_string(bucket->getID()+1) + ":" + to_string(job_id) + ":1:1" + ":" + ftos(bucket->getStartTime()) + ":" + ftos(bucket->getEndTime()) + ":" + to_string(JOB_RUNNING_STATE), bucket->getStartTime(), ++this->lastid));
//
//
            if (!first) {
                paraverHeaderEntry += ",";
            }
            else
                first = false;

            /*
             * recall, Paraver doesn't like 0 as a node number, so add 1 to getID calls
             */
            paraverHeaderEntry += "1:" + to_string(bucket->getID()+1);
//            task++;
            cout << this->last_arrival << "     " << bucket->getEndTime() << endl;
            if (this->last_arrival < bucket->getEndTime()){
                this->last_arrival = bucket->getEndTime();
                
            }
        }
//
        paraverHeaderEntry += ")";
//
        this->paraverHeaderEntries.insert(pairJobHeaderStr(this->upperJob, paraverHeaderEntry));
//
        this->upperJob++;

    }

    /**
     * Indicates to the class that a job has quit and leave her provided allocation.
     * @param job_id The job id
     * @param allocations The set of buckets containing where the processes for the job had been allocated
     */
    void ParaverTrace::ParaverJobExit(int job_id, deque<ResourceBucket*>& allocations)
    {
        
        Simulator::SystemOrchestrator *so = this->architecture->getSystemOrchestrator();
        Partition *p = so->getPartition(0);
        Simulator::ResourceSchedulingPolicy *rsp = p->getSchedulingPolicy();
        
        log->debug("PVJE :::  Job " + to_string(job_id) + " finished at globaltime " + ftos(rsp->globalTime),1);
//        cout << "ParaVer Job Exit  " << ftos(rsp->globalTime) << endl;
        
        /*
         * TODO:
         * Here, adjust the end time of the Paraver Body Entry
         */
        
        
        

        //TODO: Rewrite with the new ResourceBuckets. There is no getCPU() anymore since a bucket may have more than one.

        // const jobListBucketPenalizedTime_t * pTimes;

        // //search paraver application id regarded job_id
        // map<int, int>::iterator it = this->idJob2Applid.find(job_id);


        // if ( it == this->idJob2Applid.end() )
        // return; // it doesn't exist

        // int applid = it->second;
        // int task = 1;


//        cout << "SIZE " << allocations.size() << endl;
//        for (deque<ResourceBucket*>::iterator it = allocations.begin(); it != allocations.end(); ++it) {
//            ResourceBucket* bucket = *it;
//            std::map<Job*, ReservationList> jra = bucket->getJobReservations();
//            cout << "HERE" << jra.size() << endl;
//        }
//        cout << "ParaVer Job Exit 2" << endl;
        // //real record trace
        // this->paraverBody.insert(new ParaverEntry("1:" + to_string(bucket->getCpu()+1) + ":" +
        // to_string(applid) + ":" + to_string(task) + ":1:" + ftos(bucket->getStartTime()) +
        // ":" + /*ftos(bucket->getEndTime())*/ ftos(j->getJobSimFinishTime()) + ":" + to_string(JOB_RUNNING_STATE),
        // bucket->getStartTime(), ++this->lastid));


        // //get all proably penaltie times for this bucket
        // if ( ( pTimes = j->getBucketPenalizedTimes(bucket->getId())) != NULL )
        // {
        // //this bucket has some penalized times
        // for ( jobListBucketPenalizedTime_t::const_iterator itp = pTimes->begin(); itp != pTimes->end(); itp++)
        // {
        // double startTime = itp->first;
        // double penalizedTime = itp->second;

        // this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu() + 1 ) + ":" +
        // to_string(applid) + ":" + to_string(task) + ":1:" +
        // ftos(startTime) +":"+ to_string(JOB_PENALIZEDTIMES_EVENT_TYPE) + ":" + ftos(penalizedTime)
        // , startTime, ++this->lastid));
        // }
        // }

        // //mark end of penalyzed times with 0
        // this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu() + 1 ) + ":" +
        // to_string(applid) + ":" + to_string(task) + ":1:" +
        // ftos(j->getJobSimFinishTime())+":"+ to_string(JOB_PENALIZEDTIMES_EVENT_TYPE) + ":" + ftos(0)
        // , bucket->getEndTime(), ++this->lastid));



        // if ( bucket->getJob()->getBWMemoryUsed() != -1 )
        // {
        // this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu() + 1 ) + ":" +
        // to_string(applid) + ":" + to_string(task) + ":1:" +
        // ftos(j->getJobSimFinishTime())+":"+ to_string(JOB_BWMEM_EVENT_TYPE) + ":0"
        // , bucket->getEndTime(), ++this->lastid));
        // }

        // //this job has been killed ?
        // if ( j->getJobSimisKilled() == true)
        // {
        // this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu()+1) + ":" + to_string(applid) + ":" +
        // to_string(task) + ":1:" + ftos(j->getJobSimFinishTime()) + ":" +
        // to_string(JOB_STATE_EVENT_TYPE) + ":" + to_string(JOB_STATE_KILLED_EVENT), bucket->getEndTime(),
        // ++this->lastid));
        // }
        // else
        // {
        // this->paraverBody.insert(new ParaverEntry("2:" + to_string(bucket->getCpu()+1) + ":" + to_string(applid) + ":" +
        // to_string(task) + ":1:" + ftos(j->getJobSimFinishTime()) + ":" +
        // to_string(JOB_STATE_EVENT_TYPE) + ":" + to_string(JOB_STATE_FINISH_EVENT), bucket->getEndTime(),
        // ++this->lastid));
        // }


        // task++;
        // }

    }

    /**
     * Sets the Log to the ParaverTrace
     * @param theValue The Log
     */
    void ParaverTrace::setLog(Log* log)
    {
        this->log = log;
        cout << "Log Set" << endl;
        log->debug("Log set", 1);
    }

    /**
     * Returns the last_arrival
     * @return A double containing the last_arrival
     */
    double ParaverTrace::getlast_arrival() const
    {
        return last_arrival;
    }

    /**
     * Sets the last_arrival to the ParaverTrace
     * @param theValue The last_arrival
     */
    void ParaverTrace::setLastArrival(double theValue)
    {
        last_arrival = theValue;
    }

    /*
     * Generate the Names Configuration File (the .row file) for Paraver
     * .row = names files
     * .pfc = config file
     * .prv = ascii data
     */
    void ParaverTrace::generateNamesConfigFile()
    {

        /* Get a local ref to the architecture for parsing. */
        ArchitectureConfiguration* arch = this->architecture;

        /*
         * This can be expanded to include things below Cpus if required
         * In this project, we don't desperately care as we currently assume all nodes are run in
         * exclusive mode (for ease of thinking).
         * The Paraver manual gives the strict hierarchical ordering as
         * SYSTEM, NODE, CPU, WORKLOAD, APPL, TASK, THREAD
         */
        vector<string> rowCpus;
        vector<string> rowNodes;



        rowCpus.push_back("LEVEL CPU SIZE " + to_string(arch->getNumberCPUs()));

        /*
         * You have to parse the partitions, then the nodes in each partitionlist to count the number of nodes.
         */
        this->numNodes = 0;
        std::vector<ArchDataModel::PartitionList*> partlist = arch->getPartitionListsInSystem();
        for (uint32_t i = 0; i < partlist.size(); i++) {
            ArchDataModel::PartitionList *pl = partlist[i];
            std::vector<ArchDataModel::Node*> nodes = pl->getNodesInPartitionList();
            this->numNodes += nodes.size();
            for (uint32_t j = 0; j < nodes.size(); j++) {
                /*
                 * Paraver cannot use node=0, it moans, so number nodes from 1 onwards
                 */
                rowNodes.push_back("NODE " + to_string(j+1) + "  # In Partition " + to_string(i+1));
                // Could iterate across nodes here, in case there are heterogenous nodes
            }

        }

        rowNodes.insert(rowNodes.begin(), "LEVEL NODE SIZE " + to_string(this->numNodes));


        log->debug("ParaVer Names File: " + to_string(numNodes) + " nodes.", 2);
        log->debug("ParaVer Names File: " + to_string(arch->getNumberCPUs()) + " CPUs.", 2);




        //
        string rowPath;

        // generating the row file

        /*
         * Is there a basic library function to do this, seems a bit hacky?
         */

        if (dir.length() == 0 && trace_name.length() == 0)
            //ok no folder provided, we assume dir as a cfg
            rowPath = "paraver_simulation.row";
        else
            rowPath = this->dir + "/" + this->trace_name + ".pfc";

        cout << "Creating paraver row file " << rowPath << endl;


        // assert(false);



        std::ofstream row(rowPath.c_str(), ios::trunc);

        if (!row) {
            cout << " Error when creating the file " << rowPath.c_str() << endl;
            exit(1);
        }

        assert(!row.fail());

        for (vector<string>::iterator it = rowNodes.begin(); it != rowNodes.end(); ++it)
            row << *it << endl;


        for (vector<string>::iterator it = rowCpus.begin(); it != rowCpus.end(); ++it)
            row << *it << endl;


        row.close();
        //
        //        // now we generate the cfg configuration file
        //        string cfgPath;
        //
        //        if (dir.length() == 0 && trace_name.length() == 0)
        //            //ok no folder provided, we assume dir as a cfg
        //            cfgPath = "paraver_simulation.pfc";
        //        else
        //            cfgPath = this->dir + "/" + this->trace_name + ".pfc";
        //
        //
        //
        //        cout << "Creating paraver cfg file " << cfgPath << endl;
        //        ofstream cfg(cfgPath.c_str(), ios::trunc);
        //
        //        if (!cfg) {
        //            cout << " Error when creating the file " << cfgPath.c_str() << endl;
        //            exit(1);
        //        }
        //
        //        assert(!cfg.fail());
        //
        //        cfg << "STATES" << endl << endl;
        //
        //        cfg << "0       NOT CREATED" << endl;
        //        cfg << "1       RUN" << endl;
        //        cfg << "2       WAITTING" << endl;
        //        cfg << "3       BLOCKED" << endl;
        //        cfg << "4       IDLE" << endl;
        //        cfg << "5       STOPPED" << endl;
        //        cfg << "6       NO_INFO" << endl;
        //        cfg << "7       FINISHED" << endl << endl;
        //
        //        cfg << "STATES_COLOR" << endl << endl;
        //
        //        cfg << "0       {0,0,0}" << endl;
        //        cfg << "1       {0,0,255}" << endl;
        //        cfg << "2       {150,240,240}" << endl;
        //        cfg << "3       {255,0,0}" << endl;
        //        cfg << "4       {255,255,102}" << endl;
        //        cfg << "5       {0,0,0}" << endl;
        //        cfg << "6       {204,204,204}" << endl;
        //        cfg << "6       {204,204,204}" << endl << endl;
        //
        //        cfg << "EVENT_TYPE" << endl << endl;
        //        cfg << "10      " << JOB_STATE_EVENT_TYPE << "     JOB_STATE" << endl;
        //        cfg << "10      " << JOB_BWMEM_EVENT_TYPE << "     JOB_MEM" << endl;
        //        cfg << "10      " << JOB_PENALIZEDTIMES_EVENT_TYPE << "     JOB_PENALIZEDTIMES" << endl;
        //        cfg << endl;
        //        cfg << "VALUES" << endl << endl;
        //        cfg << JOB_STATE_FINISH_EVENT << "     JOB_FINISH" << endl;
        //        cfg << JOB_STATE_SUBMIT_EVENT << "     JOB_SUBMIT" << endl;
        //        cfg << JOB_STATE_START_EVENT << "     JOB_START" << endl;
        //        cfg << JOB_STATE_KILLED_EVENT << "     JOB_STATE_KILLED_EVENT" << endl;
        //
        //        cfg.close();

    }

    /*
     * Factored out from older versions, this function generates the architecture definition for the header, using the new
     * architecture format.
     */
    void ParaverTrace::generateArchitectureDefinition()
    {

        ArchitectureConfiguration* arch = this->architecture;



        /* This is for the trace file header, maybe */
        string sarchitecture = to_string(this->numNodes) + "(";
        std::vector<ArchDataModel::PartitionList*> partlist = arch->getPartitionListsInSystem();
        for (uint32_t i = 0; i < partlist.size(); i++) {
            ArchDataModel::PartitionList *pl = partlist[i];
            std::vector<ArchDataModel::Node*> nodes = pl->getNodesInPartitionList();
            this->numNodes += nodes.size();
            for (uint32_t j = 0; j < nodes.size(); j++) {
//                sarchitecture += to_string(nodes[j]->getNumberCPUs()) + ",";
                sarchitecture += "1,";
                // Could iterate across nodes here, in case there are heterogenous nodes
            }
            sarchitecture.pop_back(); // Remove the last, trailing comma.

        }
        sarchitecture += ")";
        this->architectureDefintion = sarchitecture;


        //
        //        for (int i = 0; i < config->gethpcsystem()->getTotalBlades(); i++) {
        //            BladeCenter* blade = config->gethpcsystem()->getBladeCenter(i);
        //
        //
        //            for (int j = 0; j < blade->getTotalNodes(); j++) {
        //                rowNodes.push_back("Blade " + to_string(i) + ".Node " + to_string(j));
        //                log->debug("Paraver Header ROW Nodes -> Blade " + to_string(i) + ".Node " + to_string(j), 2);
        //
        //                Node* node = blade->getNode(j);
        //                if (!(j == 0 && i == 0))
        //                    sarchitecture += "," + to_string(node->getNumberCPUs());
        //                else
        //                    sarchitecture += to_string(node->getNumberCPUs());
        //
        //                for (int k = 0; k < node->getNumberCPUs(); k++) {
        //                    rowCpus.push_back("Blade " + to_string(i) + ".Node " + to_string(j) + ".cpu " + to_string(k));
        //                    log->debug("Paraver Header ROW CPUS -> Blade " + to_string(j) + ".Node " + to_string(i) + ".cpu " + to_string(k), 2);
        //                }
        //            }
        //        }
        //
        //        sarchitecture += ")";
        //
        //        this->architectureDefintion = sarchitecture;
    }

    /**
     * Returns the total_jobs
     * @return A integer containing the total_jobs
     */
    int ParaverTrace::gettotal_jobs() const
    {
        return total_jobs;
    }

    /**
     * Sets the total_jobs to the ParaverTrace
     * @param theValue The total_jobs
     */
    void ParaverTrace::setTotalJobs(const int& theValue)
    {
        total_jobs = max(theValue, theValue);
    }

    /**
     * Returns the startJobTrace
     * @return A double containing the startJobTrace
     */
    int ParaverTrace::getstartJobTrace() const
    {
        return startJobTrace;
    }

    /**
     * Sets the startJobTrace to the ParaverTrace
     * @param theValue The startJobTrace
     */
    void ParaverTrace::setStartJobTrace(const int& theValue)
    {
        startJobTrace = theValue;
    }

    /**
     * Returns the endJobTrace
     * @return A double containing the endJobTrace
     */
    int ParaverTrace::getendJobTrace() const
    {
        return endJobTrace;
    }

    /**
     * Sets the endJobTrace to the ParaverTrace
     * @param theValue The endJobTrace
     */
    void ParaverTrace::setEndJobTrace(const int& theValue)
    {
        endJobTrace = theValue;
    }

}

