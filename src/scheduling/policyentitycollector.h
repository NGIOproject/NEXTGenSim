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
#ifndef SIMULATORPOLICYENTITYCOLLECTOR_H
#define SIMULATORPOLICYENTITYCOLLECTOR_H

#include <scheduling/schedulingpolicy.h>


#include <vector>

using namespace std;
using std::vector;

namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
* This function collects information form the policy each time that is required. This collector is used for make a sampling for several policy metrics of the system.
*/
class PolicyEntityCollector{
public:
  PolicyEntityCollector(SchedulingPolicy* policy,Log* log);
  ~PolicyEntityCollector();
  
  void pullValues(double timeStamp); //gathers all the values from the policy 
 
  vector<double>* getbackfilledJobs();
  vector<double>* getnumberJobsInTheQueue();
  vector<double>* getNumberCPUsUsed();
  vector<double>* getrunningJobs();
  vector<double>* getleftWork();
  vector<double>* gettimeStamp();
  vector<string>* getcenterName();
  
  Log* log;

protected: 
  SchedulingPolicy* policy;
  
public:  
  vector<double> backfilledJobs; /**< number of backfilled jobs in each interval of time*/
  vector<double> numberJobsInTheQueue; /**< number of the jobs that where waitting in the queue*/
  vector<double> numberCpusUsed; /**< Tne number of cpus that are currenty used */
  vector<double> leftWork; /**< The amount of pending work at the current point of time **/
  vector<double> runningJobs; /**< The number of runing jobs at the current point */
  vector<double> timeStamp; /**< The timestap when a given collection has been done */
  vector<string> centerName; /**< The center where the collection has been obtained  */
  
  double lastNumberOfBackfilledJobs; /**< The last number of backfilledJobs the last time that was requested (used for compute the amount of backfilledJobs since the last query)*/

  void pullBrankInfo(SchedulingPolicy* policy,double timeStamp);
  
};

}

#endif
