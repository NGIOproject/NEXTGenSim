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
 * File:   nntfInput.cpp
 * Author: nick
 * 
 * Created on 27 June 2018, 14:21
 */
#include <scheduling/job.h>
#include <utils/utilities.h>
#include <utils/log.h>
#include <utils/nntfInput.h>
#include <algorithm>
#include <cctype>

namespace Simulator {

    nntfInput::nntfInput() {
    }

    nntfInput::nntfInput(const nntfInput& orig) {
    }

    nntfInput::nntfInput(string FilePath, Log* log, bool is_output) : TraceFile(FilePath, log) {
        log->debug("Entered nntfInput method.", 4);
    }

    nntfInput::~nntfInput() {
    }

    bool nntfInput::loadTrace() {
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
        log->debug("Opening NNTF Input: " + filePath, 1);

        if (trace.is_open()) {

            while (LoadMoreJobs() && getline(trace, line)) {
                log->debug("Reading the trace file from the " + line, 6);

                line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
                // Put in a trap to check the first field does not begin with a # which indicates a comment.
                deque<string> fields;
                SplitLine(line, separator, fields); // This returns fields into a deque and not an array, upgrade later.
                size_t found = fields[0].find_first_of("#");
                if (found != std::string::npos) {
                    log->debug("Found a comment: " + line, 4);
                    continue;
                }

                /*for debug */
                if (log->getLevel() >= 6)
                    for (int i = 0; i < (int) fields.size(); i++)
                        log->debug("Field number " + to_string(i) + " has the value" + ((string) fields[i]), 6);

                currentJob = new Job();

                /* Mapping from NNTF Spec:
                 * NNTF declaration line - required
                 * Header Line - optional but strongly encouraged
                 * JobID - starts from 1 in the file
                 * WorkflowID - 0 for no workflow, integer for a workflowID
                 * PreceedingJobs - 0 for no preceeding jobs, integer JobID's otherwise, always contained in a []
                 * Workflow Status - 0 for no workflow, 1 for begin, 2 for continue, 3 for end
                 * User - string 
                 * Number of Nodes required - int
                 * IO fraction - float - 0 < X < 1
                 * Arrival time - integer t >= 0
                 * Wait time - integer t >= 0
                 * Run time - integer t >= 0
                 * Requested time - integer t >= 0, t >= run time
                 */
                
                string preceeding_array;

                processed_fields = 0;
                for (deque<string>::iterator it = fields.begin(); it != fields.end(); ++it, processed_fields++) {
                    log->debug("Processing the field with value " + *it + " PF = " + to_string(processed_fields), 6);

                    switch (processed_fields) {
                        case 0: //JobID - starts from 1 in the file
                            currentJob->setJobNumber(this->LoadedJobs + 1);
                            break;
                        case 1: //WorkflowID - 0 for no workflow, integer for a workflowID                           
                            currentJob->setworkflowID(atoi(it->c_str()));
                            break;
                        case 2: //PreceedingJobs - 0 for no preceeding jobs, integer JobID's otherwise, always contained in a []
                            preceeding_array = it->c_str();
                            break;
                        case 3: //Workflow Status - 0 for no workflow, 1 for begin, 2 for continue, 3 for end
                            currentJob->setworkflowStatus(atoi(it->c_str()));
                            break;
                        case 4: //User - string 
                            currentJob->setUserID(1000); // We currently don't implement this, but need to for FAIRSHARE.
                            break;
                        case 5: //Number of Nodes required - int
                            currentJob->setRequestedProcessors(atoi(it->c_str()));
                            currentJob->setNumberProcessors(atoi(it->c_str()));
                            currentJob->setMinProcessors(atoi(it->c_str()));
                            break;
                        case 6: //IO fraction - float - 0 < X < 1
                            currentJob->setIOCompRatio(atof(it->c_str())); // Temporary for test.
                            break;
                        case 7: //Arrival time - integer t >= 0
                            currentJob->setSubmitTime(atoi(it->c_str()));
                            break;
                        case 8: //Wait time - integer t >= 0
                            currentJob->setWaitTime(atoi(it->c_str()));
                            break;
                        case 9: //Run time - integer t >= 0
                            currentJob->setRunTime(atoi(it->c_str()));
                            break;
                        case 10:
                            currentJob->setRequestedTime(atoi(it->c_str()));                            
                            break;
                        default:
                            break;
                    }


                }


                fields.clear();
                processed_lines++;
                
                /*
                 * Convert the preceeding jobs from a string to an array.
                 */
                preceeding_array = preceeding_array.substr(1, preceeding_array.size() - 2);
                log->debug("Preceeding Array for job " + to_string(this->LoadedJobs) + " [" + preceeding_array + "]", 2);
                deque<string> preceeding_fields;
                string pseparator = "-";
                SplitLine(preceeding_array, pseparator, preceeding_fields);
                vector<int> preceeding_ints;
                
                for (deque<string>::iterator it = preceeding_fields.begin(); it != preceeding_fields.end(); ++it) {
                    if (strcmp(it->c_str(),  "0") != 0) {
                        preceeding_ints.push_back(atoi(it->c_str()));
                    }
                }

                currentJob->setPreceedingJobs(preceeding_ints);
                currentJob->setRequestedMemory(1);
                currentJob->setGroupID(1);
                currentJob->setThinkTimePrecedingJob(0);
                currentJob->setExecutable(0);
                currentJob->setQueueNumber(0);
                currentJob->setPartitionNumber(-1);
                currentJob->setslurmprio(1000);

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

    void nntfInput::processHeaderEntry(string headerLine) {
    }

}