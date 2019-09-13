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
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pbsInput.cpp
 * Author: nick
 * 
 * Created on 27 June 2018, 14:21
 */
#include <scheduling/job.h>
#include <utils/utilities.h>
#include <utils/log.h>
#include <utils/pbsInput.h>

namespace Simulator {

    pbsInput::pbsInput() {
    }

    pbsInput::pbsInput(const pbsInput& orig) {
    }

    pbsInput::pbsInput(string FilePath, Log* log, bool is_output) : TraceFile(FilePath, log) {    
        log->debug("Entered pbsInput method." ,4);
    }
    
    pbsInput::~pbsInput() {
    }

    bool pbsInput::loadTrace() {
        //        if (is_output)
        //            return openTrace();

        /*
         * This is the format expected, gained from running my cleanup.bash script on the output from PBS
         * ArcherPBSRecordRecordID,node_count,ncpus,mem,vmem,place,end,Exit_status,start,id_string,queue,ctime,runtime,MachineID,ResourcePoolID,QueueID,serial,walltime,num_bigmem,mpiprocs,LowPri
         * 2609341,1,1,423178240,1855676416,free,1420073489,0,1420072391,2597740.sdb,serial,1420061877,1098000,36,11,3494,1,86400000,0,24,0
         * And subsequently many lines of above
         * 
         */
        string line;
        string separator(",");
        int processed_lines = 0;
        int processed_fields = 0;
        int jobWithErrors = 0;

        Job* currentJob = NULL; //will be used to set up the last arrival in the sim

        bool ok = true;
        this->LoadedJobs = 0;

        if (!fileExists(this->filePath)) {
            log->error("File " + this->filePath + " does not exists please check the configuration file");
            exit(1);
        }

        //If not gzip we directly fetch from the trace
        ifstream trace(this->filePath.c_str());

        log->debug("Opening PBS Trace: " + filePath, 1);

        if (trace.is_open()) {


            while (LoadMoreJobs() && getline(trace, line)) {
                log->debug("Reading the trace file from the " + line, 6);

                // Put in a trap to check the first field on the first line is ArcherPBSRecordRecordID
                //should contain information about a job
                deque<string> fields;
                SplitLine(line, separator, fields); // This returns fields into a deque and not an array, upgrade later.
                size_t found = fields[0].find_first_of("#");
                if (found != std::string::npos)
                    continue;
                

                /*for debug */
                if (log->getLevel() >= 6)
                    for (int i = 0; i < (int) fields.size(); i++)
                        log->debug("Field number " + to_string(i) + " has the value" + ((string) fields[i]), 6);

                currentJob = new Job();

                /* Mapping from PBS inputs to fields for directly mapped values.
                0 ArcherPBSRecordRecordID   setJobNumber
                1 node_count                --> this is nodes on ARCHER
                2 ncpus                     setRequestedProcessors --> this is cores on ARCHER
                3 mem                       setRequestedMemory
                4 vmem
                5 place
                6 end                       
                7 Exit_status
                8 start                     
                9 id_string
                10 queue
                11 ctime                    setSubmitTime
                12 runtime
                13 MachineID
                14 ResourcePoolID
                15 QueueID
                16 serial
                17 walltime                 setRequestedTime
                18 num_bigmem
                19 mpiprocs
                20 LowPri
                 */
                
                /*
                 * 21 - Preceeding Job Number
                 */

                /* Mapping from PBS inputs to fields for computed values.
                 * SWF::waittime is PBS::start-ctime --> setWaitTime(ctime-start)
                 * SWF::RunTime is end-start --> setRunTime(end-start)
                 * 
                 */
                // Why are these floats? Should be big fat unsigned ints.
                float temp_start = 0;
                float temp_end = 0;
                int pjn = 0;

                processed_fields = 0;
                for(deque<string>::iterator it = fields.begin(); it!=fields.end(); ++it, processed_fields++){
                    log->debug("Processing the field with value " + *it + " PF = " + to_string(processed_fields), 6);

                    switch (processed_fields) {
                        case 0:
                            currentJob->setJobNumber(this->LoadedJobs + 1); 
//                            currentJob->setJobNumber(atoi(it->c_str())); 
                            break;
                        case 1:
                            break;
                        case 2:
                            currentJob->setRequestedProcessors(atoi(it->c_str()));
                            currentJob->setNumberProcessors(atoi(it->c_str()));
                            currentJob->setMinProcessors(atoi(it->c_str()));
                            break;
                        case 3:
                            currentJob->setRequestedMemory(atof(it->c_str()));
                            currentJob->setUsedMemory(atof(it->c_str()));
                            break;
                        case 4:
                            break;
                        case 5:
                            break;
                        case 6:
                            temp_end = atof(it->c_str());
                            break;
                        case 7:
                            if (atoi(it->c_str()) == 0) {
                                currentJob->setStatus(1);
                            }
                            break;
                        case 8:
                            temp_start = atof(it->c_str());
                            break;
                        case 9:
                            break;
                        case 10:
                            break;
                        case 11:
                            currentJob->setSubmitTime(atof(it->c_str()));
                            break;
                        case 12:
                            break;
                        case 13:
                            break;
                        case 14:
                            break;
                        case 15:
                            break;
                        case 16:
                            break;
                        case 17:
                            currentJob->setRequestedTime(atof(it->c_str()));
                            break;
                        case 18:
                            break;
                        case 19:
                            break;
                        case 20:
 //                           pjn = atoi(it->c_str());
                            break;
//                        case 22:
//                            break;
                        default:
                            break;
                    }
                    

                }
                
                
                fields.clear();
                processed_lines++;
                if (pjn > 0) {
                    currentJob->setPrecedingJobNumber(pjn);
                    log->debug("HAVE PRECEEDER", 8);
                } else {
                    currentJob->setPrecedingJobNumber(-1);
                    log->debug("NO PRECEEDER", 8);
                }
                currentJob->setRunTime(temp_end - temp_start);
                currentJob->setWaitTime(temp_start - currentJob->getSubmitTime());
                currentJob->setUserID(1000);
                currentJob->setGroupID(1);
                currentJob->setThinkTimePrecedingJob(0);
                currentJob->setExecutable(0);
                currentJob->setQueueNumber(0);
                currentJob->setPartitionNumber(-1);
                currentJob->setslurmprio(1000);
                currentJob->setIOCompRatio(0.2); // Temporary for test.
                assert(currentJob != NULL);



//                currentJob->setCPUsPerNode(8); This is weird and breaks stuff. Badly.




                if (currentJob->getJobNumber() < this->LoadFromJob) {
                    //the current job has not to be loaded .. so we just skip it 
                    delete currentJob;
                    continue;
                }

                if (currentJob->getJobNumber() > this->LoadTillJob && this->LoadTillJob != -1) {
                    log->debug("The trace is load till the job " + to_string(currentJob->getJobNumber()), 2);
                    delete currentJob;
                    break;
                }

                this->JobList.insert(pairJob(currentJob->getJobNumber(), currentJob));
                this->LoadedJobs++;

                lastProcessedJob = currentJob;

            }
            trace.close();
        } else {
            this->error = "Unable to open file";
            exit(1);
            return false;
        }

        log->debug(to_string(jobWithErrors) + " lines of the workload trace have errors ", 1);
//        log->debug(to_string(LoadedJobs) + " loaded jobs ", 1);
        log->debug(to_string(processed_lines) + " jobs of the workload trace have been processed ", 1);

        this->lastArrival = currentJob->getSubmitTime();

        return ok;

    }
    
    void pbsInput::processHeaderEntry(string headerLine) {
    }

}