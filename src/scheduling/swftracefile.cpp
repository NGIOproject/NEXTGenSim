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
#include <scheduling/swftracefile.h>
#include <scheduling/job.h>
//#include <scheduling/gridjob.h>
#include <utils/utilities.h>
#include <utils/log.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <cerrno>

using namespace std;
using std::vector;
using std::deque;

namespace Simulator {

    /***************************
      Constructor and destructors
     ***************************/

    /**
     * The default constructor for the class
     */
    SWFTraceFile::SWFTraceFile() : TraceFile() {
    }

        /**
     * The constructor that should be used when using this class
     * @param FilePath The file path of the file that contains the workload definition
     * @param log The logging engine 
     */
    SWFTraceFile::SWFTraceFile(string FilePath, Log* log, bool is_output) : TraceFile(FilePath, log) {
    
        this->isGrid = false;
        this->isExtended = false;
        this->is_output = is_output;
    }

    /**
     * The default destructor for the class 
     */
    SWFTraceFile::~SWFTraceFile() {
    }

    /***************************
      Auxiliary data functions  
     ***************************/

    /**
     * Function that loads the workload 
     * @return True if no problem has occurred, false otherwise
     */
    bool SWFTraceFile::loadTrace() {
        if (is_output)
            return openTrace();

        string line;
        string separator(" ");
        int processed_lines = 0;
        int processed_fields = 0;
        int jobWithErrors = 0;

        Job* currentJob = NULL; //will be used to set up the last arrival in the sim

        bool ok = true;
        this->LoadedJobs = 0;

        //Stuff related to the input file
        bool is_gz = false;

        if (!fileExists(this->filePath)) {
            log->error("File " + this->filePath + " does not exists please check the configuration file");
            exit(1);
        }

        //If not gzip we directly fetch from the trace
        ifstream trace(this->filePath.c_str());

        //Otherwise from the istream
        boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf; //to be used in case of gz stream
        istream * gz_trace;

        cout << " Opening SWF Trace " << filePath << endl;

        if (trace.is_open()) {
            //now .. we have to double check if the trace is in gz format or raw
            //in case of raw use boost iostreams otherwise regular istream    
            if (this->filePath.substr(this->filePath.find_last_of(".") + 1) == "gz") {
                log->debug("Compressed trace file", 5);
                inbuf.push(boost::iostreams::gzip_decompressor());
                inbuf.push(trace);
                gz_trace = new istream(&inbuf);
                is_gz = true;
            } else {
                log->debug("Regular trace file", 5);
            }

            while (LoadMoreJobs() &&
                    ((!is_gz && getline(trace, line)) || //regular stream
                    (is_gz && getline(*gz_trace, line))) //gz stream
                    ) {
                log->debug("Reading the trace file from the " + line, 6);

                //for each line we try to parse the swf
                //if ";" means its header stuff so we should process it
                string::size_type loc = line.find(";", 0);

                if (loc != string::npos) {
                    this->processHeaderEntry(line);
                    log->debug("Processing header line " + line, 6);
                    continue;
                }

                //should contain information about a job
                deque<string> fields;
                SplitLine(line, separator, fields);

                //cout << "processing line " << line << endl;

                //log->debug("The line has "+to_string(fields.size())+ "parameters",6);
                //cout << "The line has " << to_string(fields.size()) << " parameters";

                /*for debug */
                if (log->getLevel() >= 6)
                    for (int i = 0; i < (int) fields.size(); i++)
                        log->debug("Field number " + to_string(i) + " has the value" + ((string) fields[i]), 6);
                /*	 
                      if(fields.size() != 18)
                      {
                        log->error("Line "+to_string(processed_lines)+" of the workload file "+this->filePath+ " has not 18 fields or some separator is different than a space");
                        log->debug("Line "+to_string(processed_lines)+" of the workload file "+this->filePath+ " has no good format. Avoiding job.");
                        ok = false;
                        jobWithErrors++;
                        continue;	
                      }
                 */
                //processed_fields = 17;     
                processed_fields = fields.size();
                processed_fields--;

                //if(this->isGrid)
                //  currentJob = new GridJob();
                //else 
                currentJob = new Job();

                for (; !fields.empty(); fields.pop_back()) {
                    string field = (string) fields.back();

                    log->debug("Processing the field with value " + field, 6);

                    switch (processed_fields) {
                        case 0:
                            currentJob->setJobNumber(atoi(field.c_str()));
                            break;
                        case 1:
                            currentJob->setSubmitTime(atof(field.c_str()));
                            break;
                        case 2:
                            currentJob->setWaitTime(atof(field.c_str()));
                            break;
                        case 3:
                            currentJob->setRunTime(atof(field.c_str()));
                            break;
                        case 4:
                            currentJob->setNumberProcessors(atoi(field.c_str()));
                            currentJob->setMinProcessors(atoi(field.c_str()));
                            break;
                        case 5:
                            currentJob->setAvgCPUTimeUsed(atoi(field.c_str()));
                            break;
                        case 6:
                            currentJob->setUsedMemory(atof(field.c_str()));
                            break;
                        case 7:
                            currentJob->setRequestedProcessors(atoi(field.c_str()));
                            break;
                        case 8:
                            currentJob->setRequestedTime(atof(field.c_str()));
                            currentJob->setOriginalRequestedTime(atof(field.c_str()));
                            break;
                        case 9:
                            currentJob->setRequestedMemory(atof(field.c_str()));
                            break;
                        case 10:
                            currentJob->setStatus(atoi(field.c_str()));
                            break;
                        case 11:
                            currentJob->setUserID(atoi(field.c_str()));
                            break;
                        case 12:
                            currentJob->setGroupID(atoi(field.c_str()));
                            break;
                        case 13:
                            currentJob->setExecutable(atoi(field.c_str()));
                            break;
                        case 14:
                            currentJob->setQueueNumber(atoi(field.c_str()));
                            break;
                        case 15:
                            currentJob->setPartitionNumber(atoi(field.c_str()));
                            break;
                        case 16:
                            currentJob->setPrecedingJobNumber(atoi(field.c_str()));
                            break;
                        case 17:
                            currentJob->setThinkTimePrecedingJob(atoi(field.c_str()));
                            break;
                        case 18:
                            currentJob->setMalleable(atoi(field.c_str()));
                            break;
                        case 19:
                            currentJob->setMaxProcessors(atoi(field.c_str()));
                            break;
                        default:
                            break;
                    }
                    processed_fields--;
                }
                fields.clear();
                processed_lines++;
                cout << "PART " << currentJob->getPartitionNumber() << endl;
                assert(currentJob != NULL);

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
        log->debug(to_string(LoadedJobs) + " loaded jobs ", 1);
        log->debug(to_string(processed_lines) + " jobs of the workload trace have been processed ", 1);

        this->lastArrival = currentJob->getSubmitTime();

        return ok;
    }

    void SWFTraceFile::processHeaderEntry(string headerLine) {
    }

    bool SWFTraceFile::openTrace() {
        outtrace.open(this->filePath.c_str(), ios::trunc);

        if (not outtrace.is_open()) {
            cout << "Error: The SWF Output Trace file " << this->filePath << " coult not be opened. Some error has occurred: " << strerror(errno) << endl;
            exit(-1);
        }
        return true;
    }

    void SWFTraceFile::logJob(Job *job) {
        //TODO: Include malleability

        int info[18]; // SWF has 18 fields

        info[0] = job->getJobNumber();
        info[1] = job->getSubmitTime();
        info[2] = job->getJobSimWaitTime();
        info[3] = job->getRunTime();
        info[4] = job->getNumberProcessors();
        info[5] = job->getAvgCPUTimeUsed();
        info[6] = job->getUsedMemory();
        info[7] = job->getRequestedProcessors();
        info[8] = job->getOriginalRequestedTime();
        info[9] = job->getRequestedMemory();
        info[10] = job->getStatus();
        info[11] = job->getUserID();
        info[12] = job->getGroupID();
        info[13] = job->getExecutable();
        info[14] = job->getQueueNumber();
        info[15] = job->getPartitionNumber();
        info[16] = job->getPrecedingJobNumber();
        info[17] = job->getThinkTimePrecedingJob();


        bool first = true;
        for (uint32_t i = 0; i < 18; i++) {
            if (not first)
                outtrace << " ";
            else
                first = false;
            outtrace << info[i];
        }
        outtrace << "\n";
    }

}
