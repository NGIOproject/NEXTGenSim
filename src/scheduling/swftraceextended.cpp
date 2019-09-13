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
#include <scheduling/swftraceextended.h>
#include <utils/utilities.h>
#include <utils/log.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>


namespace Simulator {

/**
 * The default constructor for the class
 */
SWFTraceExtended::SWFTraceExtended()
{
 this->JobRequirementsFile = "";
}

/**
 * The default destructor for the class 
 */
SWFTraceExtended::~SWFTraceExtended()
{
}

/**
 * The constructor that should be used 
 * @param FilePath The file path that contains the standard workload file with the jobs definition
 * @param JobRequirementsFile  The file path that contains the job requirements
 * @param log The logging engine 
 */
SWFTraceExtended::SWFTraceExtended(string FilePath,string JobRequirementsFile,Log* log) : SWFTraceFile(FilePath,log)
{
  this->JobRequirementsFile = JobRequirementsFile;  
  
  assert(JobRequirementsFile.compare("") != 0);

  this->isExtended = true;
}


/**
 * Function that loads resource the requirements file for the workload 
 * @return True if no problem have occurred, false otherwise (in this case the error will be shown in the logging engine)
 */
bool SWFTraceExtended::LoadCostRequirements()
{
  string line;
  string separator(" ");
  int processed_lines = 0;
  int processed_fields = 0;
  int jobWithErrors = 0;  
  
  bool ok = true;
  
  this->LoadedJobs = 0;
  this->lastProcessedJob = NULL;
  
  //Stuff related to the input file
  bool is_gz = false;
  
  this->JobRequirementsFile = expandFileName(this->JobRequirementsFile);
  
  if(!fileExists(this->JobRequirementsFile))
  {
    log->error("File " + this->JobRequirementsFile + " does not exists please check the configuration file");
    exit(1);
  }
    
    //If not gzip we directly fetch from the trace
  ifstream trace (this->JobRequirementsFile.c_str());
    
  //Otherwise from the istream
  boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf; //to be used in case of gz stream
  istream * gz_trace;
   
  cout << " Opening ExtendedSWF with costs Trace " << JobRequirementsFile << endl;

  if (trace.is_open())
  {
    //now .. we have to double check if the trace is in gz format or raw
    //in case of raw use boost iostreams otherwise regular istream    
    if(this->filePath.substr(this->JobRequirementsFile.find_last_of(".") + 1) == "gz") {
      log->debug("Compressed trace file",5);       
      inbuf.push(boost::iostreams::gzip_decompressor());
      inbuf.push(trace);
      gz_trace = new istream(&inbuf);
      is_gz = true;

    } else {
      log->debug("Regular trace file",5);
    }
    
    
    while (LoadMoreJobs() && 
          (
            (!is_gz && getline(trace,line)) ||     //regular stream
            (is_gz && getline(*gz_trace,line))     //gz stream
          ))    
    {
      log->debug("Reading the trace file from the "+line,6);
      
      //for each line we try to parse the swf
      //if ";" means its header stuff so we should process it
      string::size_type loc = line.find(";", 0 );
      
      if( loc != string::npos )
      {
      	this->processHeaderEntry(line);
        log->debug("Processing header line "+line,6);
        continue;
      }
      //should contain information about a job
      deque<string> fields;     
      SplitLine(line,separator,fields);
      log->debug("The line has "+to_string(fields.size())+ "parameters",6);
      /*for debug */
      if(log->getLevel() >= 2)  
        for(int i = 0;i < (int)fields.size();i++)
          log->debug("Field number "+to_string(i) +" has the value"+((string)fields[i]) ,6);
      if(fields.size() != 2)
      {
      	log->error("Line "+to_string(processed_lines)+" of the workload job requirements file "+this->filePath+ " is not 2 fields or some separator is different than a space");
        log->debug("Line "+to_string(processed_lines)+" of the workload job requirements file "+this->filePath+ " has no good format. Avoiding job.");
        ok = false;
        jobWithErrors++;
        continue;	
      }
      processed_fields = 0;   
      Job* job = NULL;
      bool cont=true;
      int jobid;
      map<int,Job*>::iterator itJob;
      double costAllowed;
      for(;!fields.empty() && cont;fields.pop_front())
      {
        string field = (string) fields.front();
        switch(processed_fields)
        {
          case 0: 
            jobid=atoi(field.c_str());
            if( jobid < this->LoadFromJob)
            {
              //the current job has not to be loaded .. so we just skip it 
              cont=false;
              break;
            }
            if(jobid > this->LoadTillJob && this->LoadTillJob != -1)
            {
              log->debug("The trace is load till the job "+to_string(jobid),2);   
              cont=false;
              break;
            }

            //this key should be the job id 
            itJob =  JobList.find(atoi(field.c_str()));
            if(itJob == JobList.end())
            {
              log->error("There is no job with id "+field+ " in the main trace file .. please check the job requirements file.");
              break;
            }
            job = itJob->second;	
            assert(job != NULL);
            break;
          case 1: 
            //this should be the BWMemory used 
            costAllowed = atof(field.c_str());
            job->setMaximumAllowedCost(costAllowed);
          default:
            break;
        }
        processed_fields++;                
      }
     
      if(cont)
      {
        this->lastProcessedJob = job;
        this->LoadedJobs++;
      }
    }
  }
  return ok;
}

/**
 * Function that loads resource the requirements file for the workload 
 * @return True if no problem have occurred, false otherwise (in this case the error willl be shown in the logging engine)
 */
bool SWFTraceExtended::LoadRequirements()
{
  string line;
  string separator(" ");
  int processed_lines = 0;
  int processed_fields = 0;
  int jobWithErrors = 0;  
  
  bool ok = true;
  
  this->LoadedJobs = 0;
  this->lastProcessedJob = NULL;
  
  
  //Stuff related to the input file
  bool is_gz = false;
  this->JobRequirementsFile = expandFileName(this->JobRequirementsFile);
  
  if(!fileExists(this->JobRequirementsFile))
  {
    log->error("File " + this->JobRequirementsFile + " does not exists please check the configuration file");
    exit(1);
  }
   
  
  //If not gzip we directly fetch from the trace
  ifstream trace (this->JobRequirementsFile.c_str());
    
  //Otherwise from the istream
  boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf; //to be used in case of gz stream
  istream * gz_trace;
   
  cout << "Opening ExtendedSWF  Trace " << JobRequirementsFile << endl;
  

  if (trace.is_open())
  {
    if(this->JobRequirementsFile.substr(this->JobRequirementsFile.find_last_of(".") + 1) == "gz") {
      log->debug("Compressed trace file",5);       
      inbuf.push(boost::iostreams::gzip_decompressor());
      inbuf.push(trace);
      gz_trace = new istream(&inbuf);
      is_gz = true;
    } else {
      log->debug("Regular trace file",5);
    }
    
    
    while (LoadMoreJobs() && 
          ((!is_gz && getline(trace,line)) ||     //regular stream
          (is_gz && getline(*gz_trace,line)))    //gz stream
          )
    { 
      log->debug("Reading the trace file from the "+line,6);
      processed_lines++;
      
      //for each line we try to parse the swf
      //if ";" means its header stuff so we should process it
      string::size_type loc = line.find(";", 0 );
      if( loc != string::npos )
      {
      	this->processHeaderEntry(line);
        log->debug("Processing header line "+line,6);
        continue;
      }
      //should contain information about a job
      deque<string> fields;     
      SplitLine(line,separator,fields);
      
      log->debug("The line has "+to_string(fields.size())+ "parameters",6);
      
      /*for debug */
      if(log->getLevel() >= 2)  
        for(int i = 0;i < (int)fields.size();i++)
          log->debug("Field number "+to_string(i) +" has the value"+((string)fields[i]) ,6);
      if(fields.size() != 5)
      {
      	log->error("Line "+to_string(processed_lines)+" of the workload job requirements file "+this->filePath+ " is not 5 fields or some separator is different than a space");
        log->debug("Line "+to_string(processed_lines)+" of the workload job requirements file "+this->filePath+ " has no good format. Avoiding job.");
        ok = false;
        jobWithErrors++;
        continue;	
      }      
      processed_fields = 0;    
      Job* job = NULL;
      bool cont=true;
      int jobid;
      map<int,Job*>::iterator itJob;
      double BWMemory,Memory,BWNetwork,BWEthernet;
      for(;!fields.empty() && cont ;fields.pop_front())
      {
        string field = (string) fields.front();
        switch(processed_fields)
        {
          case 0: 
            jobid=atoi(field.c_str());
            if( jobid < this->LoadFromJob)
            {
              //the current job has not to be loaded .. se we just skip it 
              cont=false;
              break;
            }
            if(jobid > this->LoadTillJob && this->LoadTillJob != -1)
            {
              log->debug("The trace is load till the job "+to_string(jobid),2);          
              cont=false;
              break;
            }
            //this key should be the job id 
            itJob =  JobList.find(atoi(field.c_str()));
            if(itJob == JobList.end())
            {
              log->error("There is no job with id "+field+ " in the main trace file .. please check the job requirements file.");
              goto NEXT;
            }
            job = itJob->second;	
            assert(job != NULL); 
            break;
          case 1: 
            //this should be the BWMemory used 
            BWMemory = atof(field.c_str());
            job->setBWMemoryUsed(BWMemory);
            break;
          case 2:
            //this should be the Memory needed
            Memory = atof(field.c_str());
            job->setMemoryUsed(Memory);
            break;    
          case 3:
            //this should be the BWNetwork
            BWNetwork = atof(field.c_str());
            job->setBWNetworkUsed(BWNetwork);
            break;
          case 4:
            //this should be the BWEthernet
            BWEthernet = atof(field.c_str());
            job->setBWEthernedUsed(BWEthernet);
            break;   
        }
        processed_fields++;                
      }
NEXT: if(cont)
      {
        this->lastProcessedJob = job;
        this->LoadedJobs++;
      }
    }
  }
  log->debug(to_string(jobWithErrors)+ " lines of the workload trace have errors ",1);
  log->debug(to_string(LoadedJobs)+ " loaded jobs ",1);
  log->debug(to_string(processed_lines)+ " jobs of the workload trace have been processed ",1);
  return ok;
}

}
