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
#ifndef SIMULATORSTATISTICS_H
#define SIMULATORSTATISTICS_H

#include <scheduling/metric.h>
#include <utils/log.h>
#include <scheduling/job.h>
#include <scheduling/policyentitycollector.h>
#include <boost/filesystem.hpp>

#include <vector>
#include <map> 

using namespace std;
using namespace Utils;
using namespace boost::filesystem;

using std::vector;
using std::map;


namespace Simulator {

/**
* @author Francesc Guim,C6-E201,93 401 16 50,
*/


/** Defines a set of metrics */
typedef vector<Metric*>  Metrics;

/** 
* This class is the responsible for compute all the statistics for the simulation given a set of metrics, all the jobs that have been simulated till a given point of time, and the reference to the scheduling policy, it computes the required statistics
*/
class SimStatistics{
public:
  SimStatistics(Log* log);
  ~SimStatistics();
  void addJobMetrics(Metrics* metrics);
  Metrics* computeJobMetrics(map<int,Job*>* jobs);
  Metrics* computePolicyMetrics(PolicyEntityCollector* collector);
  void setJobMetricsDefinition(Metrics* jobMetrics);
  void setPolicyMetricsDefinition(Metrics* policyMetrics);
  void setGlobalStatisticsOutputFile(const string& theValue);
  string getGlobalStatisticsOutputFile() const;
  void setsimOutputPath(const string& theValue);
  path getsimOutputPath(void);

  void setSchedDataFile(const string& theValue);
  path getSchedDataFile();

  
  void setSchedCountFile(const string& theValue);
  path getSchedCountFile();

  
  void saveGlobalStatistics();
  void ShowCurrentStatisticsStd();
  bool getshowStatsStdOut() const;
  void setShowStatsStdOut(bool theValue);
  void doHeatmap(map<int,Job*>* jobs);
  
  
private:
  Metrics* jobMetrics; /**< The set of metrics that will be computed for the jobs  */
  Metrics* policyMetrics; /**< The set of metrics that will computed for the policy  */
  
  //Auxiliar methods for parsing the strings
  void getJobVariableValues(vector<double>* output,map<int,Job*>* jobs,Metric* metric);
  void getJobVariableValues(vector<int>* output,map<int,Job*>* jobs,Metric* metric);
  void getJobVariableValues(vector<string>* output,map<int,Job*>* jobs,Metric* metric);
  void getJobVariableValues(map<string,vector<double>*>* output,map<int,Job*>* jobs,Metric* metric);
  
  void computeMetricValue(vector<double>* input,Metric* metric);
  void computeMetricValue(vector<int>* input,Metric* metric);
  void computeMetricValue(vector<string>* input,Metric* metric);
  void computeMetricValue(map<string,vector<double>*>* input,Metric* metric);
  string getStringOfRSP( RS_policy_type_t RSP);
  
  void MetricsValues(Metrics* metrics, vector<string>* descriptions,bool dumpStdOut);
  string GetStringValue(Metric* metric);
  
  vector<double> * getPolicydVariableMetrics(PolicyEntityCollector* collector,Metric* metric);
  vector<int> * getPolicyiVariableMetrics(PolicyEntityCollector* collector,Metric* metric);


  Log* log;  /**< A reference to the logging engine*/
  string GlobalStatisticsOutputFile; /**< a string that contains the file path for the file where the statistics will be stored */
  path simOutputPath;
  path schedDataFile;
  path schedCountFile;
  bool showStatsStdOut; /**< indicates if the statistics has to be shown in the stdout of the simulation or not.*/


};

}

#endif
