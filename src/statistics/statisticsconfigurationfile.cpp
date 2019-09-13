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
#include <statistics/statisticsconfigurationfile.h>
#include <utils/utilities.h>
#include <sstream>


namespace Utils {

/**
 * The constructor for the class
 * @param StatisticsConfigurationFilePath The statistic configuration file path
 * @param log The logging engine
 */
StatisticsConfigurationFile::StatisticsConfigurationFile(string StatisticsConfigurationFilePath,Log* log)
{
    this->StatisticsConfigurationFilePath = expandFileName(StatisticsConfigurationFilePath);
    log->debugConfig("[StatisticsConfiguration] Loading the statistics configuration file: "+ StatisticsConfigurationFilePath);
    //Init XML2 Lib Parser
    xmlInitParser();
    //Parse the Xml document
    this->doc = xmlParseFile(this->StatisticsConfigurationFilePath.c_str());
    
    if(this->doc == NULL)
    {
        cout << "[StatisticsConfiguration] Error when openening " << this->StatisticsConfigurationFilePath << endl;
        exit(1);
    }
  
    //JobsMetrics first we collect Job Metrics 
    xmlNodeSetPtr metrics = getNodeSetFromXPathExpression("/StatisticsSimulationConfiguration/MetricsToCollect/JobsMetrics/Variable",this->doc);

    for(int i = 0;metrics && i < metrics->nodeNr;i++)
    {
        xmlDocPtr metricDoc = (xmlDocPtr) metrics->nodeTab[i];

        //Type
        string Type= getStringFromXPathExpression("/Name",metricDoc,true);
        log->debug("[StatisticsConfiguration] Name= " + Type); 
  
        xmlNodeSetPtr statistics = getNodeSetFromXPathExpression("/Statistic",metricDoc);
 
        for(int i = 0;statistics && i < statistics->nodeNr;i++)
        {
            xmlDocPtr statisticDoc = (xmlDocPtr) statistics->nodeTab[i];

            //Type
            string statisticType = getStringFromXPathExpression("/Type",statisticDoc,true);
            log->debug("[StatisticsConfiguration] Type= " + Type); 
            log->debugConfig("[JobsMetrics] Loading metric "+ Type+" with statistic " + statisticType);
            //Now we have to add the metric to the corresponding entity type
            this->addJobMetric(Type,statisticType);
                    
        }
        xmlXPathFreeNodeSet(statistics);
    }
    xmlXPathFreeNodeSet(metrics);
    //JobsMetrics first we collect Job Metrics 
    metrics = getNodeSetFromXPathExpression("/StatisticsSimulationConfiguration/MetricsToCollect/PolicyMetrics/Variable",this->doc);

    for(int i = 0;metrics && i < metrics->nodeNr;i++)
    {
        xmlDocPtr metricDoc = (xmlDocPtr) metrics->nodeTab[i];

        //Type
        string Type= getStringFromXPathExpression("/Name",metricDoc,true);
        log->debug("[StatisticsConfiguration] Name= " + Type); 
  
        xmlNodeSetPtr statistics = getNodeSetFromXPathExpression("/Statistic",metricDoc);
 
        for(int i = 0;statistics && i < statistics->nodeNr;i++)
        {
            xmlDocPtr statisticDoc = (xmlDocPtr) statistics->nodeTab[i];

            //Type
            string statisticType = getStringFromXPathExpression("/Type",statisticDoc,true);
            log->debug("[StatisticsConfiguration] Type= " + Type); 

            log->debugConfig("[PolicyMetrics] Loading metric " + Type + " with statistic "+ statisticType);
            //Now we have to add the metric to the corresponding entity type
            this->addPolicyMetric(Type,statisticType);
                    
        }
        xmlXPathFreeNodeSet(statistics);
    }
    xmlXPathFreeNodeSet(metrics);
}

/**
 * The default destructor for the class 
 */
StatisticsConfigurationFile::~StatisticsConfigurationFile()
{
}

/**
 * Adds a new job metric to the definition 
 * @param variable The string containing the type of variable 
 * @param estimator The string containing the statistic to use
 */
void StatisticsConfigurationFile::addJobMetric(string variable,string estimator)
{
   this->addMetric(variable,estimator,&this->jobMetrics);
}


/**
 * Returns the policy metrics 
 * @return A reference to the policy metrics
 */
vector< Metric*>* StatisticsConfigurationFile::getPolicyMetrics()
{
   return &this->policyMetrics;
}


/**
 * Adds a new job metric to the definition 
 * @param variable The string containing the type of variable 
 * @param estimator The string containing the statistic to use
 */
void StatisticsConfigurationFile::addPolicyMetric(string variable,string estimator)
{
    cout << variable << " ! " << estimator << endl;
   this->addMetric(variable,estimator,&this->policyMetrics); 
}


/**
 * Adds a new metric to the definition 
 * @param variable The string containing the type of variable 
 * @param estimator The string containing the statistic to use 
 * @param metrics The vector where the metric has to be added
 */
void StatisticsConfigurationFile::addMetric(string variable,string estimator,vector<Metric*>* metrics)
{
   metric_t type = this->getMetricType(variable);
   statistic_t statisticUsed = this->getStatisticType(estimator);   
   native_t nativeType = this->getNativeType(variable);
   
   if(statisticUsed == OTHER_STAT)
   {
     std::cout << "The statistical estimator " << estimator << " specified in the var " << variable << " does not exists in the simulator, please check the file." << std::endl;
     return;
     
   }
   
   Metric* newMetric = new Metric(type,statisticUsed,variable,nativeType);
   newMetric->setEstimatorName(estimator);
   metrics->push_back(newMetric);

}


/**
 * Returns the statistic type given its string format 
 * @param estimator Thes statistic type in string format 
 * @return A statistic_t containing the type of the estimator 
 */
statistic_t StatisticsConfigurationFile::getStatisticType(string estimator)
{
   if(estimator == "MEAN")
   {
     return AVG;     
   }
   else if(estimator == "MEDIAN")
   {     
     return MEDIAN;     
   }   
   else if(estimator == "PERCENTILE95")
   {     
     return PERCENTILE95;     
   }   
   else if(estimator == "PERCENTILES")
   {     
     return PERCENTILES;     
   }      
   else if(estimator == "MIN")
   {     
     return MIN;     
   }   
   else if(estimator == "MAX")
   {     
     return MAX;     
   }   
   else if(estimator == "STDEV")
   {     
     return STDEV;     
   }  
   else if(estimator == "IQR")
   {     
     return IQR;     
   }  
   else if(estimator == "ACCUMULATE")
   {     
     return ACCUMULATE;     
   }    
   else if(estimator == "COUNT_LITERALS")
   {     
     return COUNT_LITERALS;          
   }          
   else if(estimator == "ACCUMULATE_LITERALS")
   {     
     return ACCUMULATE_LITERALS;     
   }    
   else
    return OTHER_STAT;  

}

/**
 * Returns the native type given the metric string format 
 * @param variable Thes metric type in string format 
 * @return A native_t containing the native type of the metric 
 */
native_t StatisticsConfigurationFile::getNativeType(string variable)
{   if(variable == "SLD")
   {
     return DOUBLE;     
   }
   else if(variable == "BSLD")
   {     
     return DOUBLE;     
   }
   else if(variable == "BSLD_PRT")
   {     
     return DOUBLE;     
   }      
   else if(variable == "WAITTIME")
   {     
     return DOUBLE;     
   }  
   else if(variable == "BACKFILLED_JOB")
   {     
     return DOUBLE;     
   }     
   else if(variable == "kILLED_JOB")
   {     
     return DOUBLE;     
   }   
   else if(variable == "BACKFILLED_JOBS")
   {     
     return DOUBLE;     
   }        
   else if(variable == "JOBS_IN_WQUEUE")
   {     
     return DOUBLE;     
   }        
   else if(variable == "CPUS_USED")
   {     
     return DOUBLE;     
   }      
   else if(variable == "LEAST_WORK_LEFT")
   {     
     return DOUBLE;     
   }   
   else if(variable == "SUBMITED_JOBS")
   {     
     return DOUBLE;     
   }      
   else if(variable == "SUBMITED_JOBS_CENTER")
   {     
     return STRING;     
   }   
   else if(variable == "RSP_USED")
   {     
     return STRING;     
   }
   else if(variable == "COMPUTATIONAL_COST_RESPECTED")
   {     
     return STRING;     
   }  
   else if(variable == "PENALIZED_RUNTIME")
   {     
     return DOUBLE;     
   }       
   else if(variable == "PERCENTAGE_PENALIZED_RUNTIME")
   {     
     return DOUBLE;     
   }            
   else if(variable == "LOAD_JOBS_CENTERS")
   {     
     return COMPOSED_DOUBLE;     
   }       
   else if(variable == "COMPUTATIONAL_COST")
   {     
     return DOUBLE;     
   }  
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_WT")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_RUNTIME")
   {     
     return DOUBLE;
   }
   else if(variable == "SLD_PREDICTED")
   {     
     return DOUBLE;
   }
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST_ERROR")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_WT_ERROR")
   {     
     return DOUBLE;     
   }  
   else if(variable == "PREDICTED_RUNTIME_ERROR")
   {     
     return DOUBLE;
   }
   else if(variable == "SLD_PREDICTED_ERROR")
   {     
     return DOUBLE;
   }
   else if(variable == "ESTIMATED_START_TIME_FIRST_FIT")
   {
     return DOUBLE;
   }
   else if (variable == "DELAY_SECONDS")
   {
       return INTEGER;
   }
   else if (variable == "ARCHER_RATIO")
   {
       return DOUBLE;
   }
   else
    return OTHER_NATIVE_T;

}


/**
 * Returns the metric type given its string format 
 * @param estimator Thes metric type in string format 
 * @return A metric_t containing the type of the metric 
 */
metric_t StatisticsConfigurationFile::getMetricType(string variable)
{
   if(variable == "SLD")
   {
     return SLD;     
   }
   else if(variable == "BSLD")
   {     
     return BSLD;     
   }   
   else if(variable == "BSLD_PRT")
   {     
     return BSLD_PRT;     
   }   
   else if(variable == "WAITTIME")
   {     
     return WAITTIME;     
   }  
   else if(variable == "BACKFILLED_JOB")
   {     
     return BACKFILLED_JOB;     
   }     
   else if(variable == "kILLED_JOB")
   {     
     return kILLED_JOB;     
   }   
   else if(variable == "BACKFILLED_JOBS")
   {     
     return BACKFILLED_JOBS;     
   }        
   else if(variable == "JOBS_IN_WQUEUE")
   {     
     return JOBS_IN_WQUEUE;     
   }        
   else if(variable == "CPUS_USED")
   {     
     return CPUS_USED;     
   }      
   else if(variable == "LEAST_WORK_LEFT")
   {     
     return LEAST_WORK_LEFT;     
   }   
   else if(variable == "SUBMITED_JOBS")
   {     
     return SUBMITED_JOBS;     
   }      
   else if(variable == "SUBMITED_JOBS_CENTER")
   {     
     return SUBMITED_JOBS_CENTER;     
   }   
   else if(variable == "RSP_USED")
   {     
     return RSP_USED;     
   }         
   else if(variable == "PENALIZED_RUNTIME")
   {     
     return PENALIZED_RUNTIME;     
   }       
   else if(variable == "PERCENTAGE_PENALIZED_RUNTIME")
   {     
     return PERCENTAGE_PENALIZED_RUNTIME;     
   }            
   else if(variable == "LOAD_JOBS_CENTERS")
   {     
     return LOAD_JOBS_CENTERS;     
   }       
   else if(variable == "COMPUTATIONAL_COST")
   {     
     return COMPUTATIONAL_COST;     
   }  
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST")
   {     
     return ESTIMATED_COMPUTATIONAL_COST;     
   }  
   else if(variable == "PREDICTED_WT")
   {     
     return PREDICTED_WT;     
   }  
   else if(variable == "PREDICTED_RUNTIME")
   {     
     return PREDICTED_RUNTIME;     
   }     
   else if(variable == "SLD_PREDICTED")
   {     
     return SLD_PREDICTED;     
   } 
   else if(variable == "ESTIMATED_COMPUTATIONAL_COST_ERROR")
   {     
     return ESTIMATED_COMPUTATIONAL_COST_ERROR;     
   }  
   else if(variable == "COMPUTATIONAL_COST_RESPECTED")
   {     
     return COMPUTATIONAL_COST_RESPECTED;     
   }  
   else if(variable == "PREDICTED_WT_ERROR")
   {     
     return PREDICTED_WT_ERROR;     
   }  
   else if(variable == "PREDICTED_RUNTIME_ERROR")
   {     
     return PREDICTED_RUNTIME_ERROR;
   }
   else if(variable == "SLD_PREDICTED_ERROR")
   {     
     return SLD_PREDICTED_ERROR;
   }
   else if(variable == "ESTIMATED_START_TIME_FIRST_FIT")
   {
     return ESTIMATED_START_TIME_FIRST_FIT;
   }
   else if (variable == "DELAY_SECONDS")
   {
       return DELAY_SECONDS;
   }
   else if (variable == "ARCHER_RATIO")
   {
       return ARCHER_RATIO;
   }
   else
    assert(false);  
}



/**
 * The job metrics defined in the configuration file 
 * @return A reference to the vector containing the list of jobs 
 */
vector< Metric * >* StatisticsConfigurationFile::getJobMetrics() 
{
  return &jobMetrics;
}

  
}
