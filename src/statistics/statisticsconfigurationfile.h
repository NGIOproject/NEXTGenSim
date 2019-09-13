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
#ifndef UTILSSTATISTICSCONFIGURATIONFILE_H
#define UTILSSTATISTICSCONFIGURATIONFILE_H

#include <scheduling/metric.h>
#include <utils/log.h>

#include <vector>

/* libxml2 stuff */
#include <libxml/tree.h>
#include <libxml/parser.h>


using namespace Simulator;
using namespace std;
using std::vector;


namespace Utils {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class contains the configuration file for the statistics definition */
class StatisticsConfigurationFile{
public:
  StatisticsConfigurationFile(string StatisticsConfigurationFilePath,Log* log);
  ~StatisticsConfigurationFile();
  vector< Metric*>* getJobMetrics();
  vector< Metric*>* getPolicyMetrics();
	

private:
  metric_t getMetricType(string variable);
  statistic_t getStatisticType(string estimator);
  native_t getNativeType(string variable);   
  void addMetric(string variable,string estimator,vector<Metric*>* metrics);
  void addJobMetric(string variable,string estimator);
  void addPolicyMetric(string variable,string estimator);

  xmlDocPtr doc; /**< XML containing the definition for the architecture */ 
  vector<Metric*> jobMetrics /**< The set of metrics to be collected for the job entity */;
  vector<Metric*> policyMetrics /** The set of metrics to be collected for the policy entity */;
  string StatisticsConfigurationFilePath; /** The statistics configuration file path*/

};

}

#endif
