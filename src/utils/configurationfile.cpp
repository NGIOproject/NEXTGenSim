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
#include <utils/utilities.h>
#include <utils/configurationfile.h>
#include<sstream>

namespace Utils {

/**
 * The default constructor for the class
 * @param ConfigFile The configuration file that will be loaded
 */
ConfigurationFile::ConfigurationFile(Utils::Log* log, string ConfigFile)
{
  
    this->log = log;
    
    //First we clear the variables that are not configured here
    this->DebugFrom = 0;
  
    log->debugDefault(" Loading the configuration file " + ConfigFile);
    //We just replace any env variable by the content it was -- like $EXPERIMENT_ROOT/Configuration.xml by the real value 
    this->ConfigFile = expandFileName(ConfigFile);
    
    //Init XML2 Lib Parser
    xmlInitParser();
    //Parse the Xml document
    this->doc = xmlParseFile(this->ConfigFile.c_str());


    //ErrorFilePath
    ErrorFilePath= getStringFromXPathExpression("/Configuration/ErrorFilePath",this->doc);
    log->debugDefault("ErrorFilePath= " + ErrorFilePath);

    //DebugFilePath
    DebugFilePath= getStringFromXPathExpression("/Configuration/DebugFilePath",this->doc);
    log->debugDefault("DebugFilePath = " + DebugFilePath);

    //DefaultWorkloadPath
    DefaultWorkloadPath= getStringFromXPathExpression("/Configuration/DefaultWorkloadPath",this->doc);
    log->debugDefault("DefaultWorkloadPath = " + DefaultWorkloadPath);

    //DefaultArchitectureConfigurationFile
    DefaultArchitectureConfigurationFile= getStringFromXPathExpression("/Configuration/DefaultArchitectureConfigurationFile",this->doc);
    log->debugDefault("DefaultArchitectureConfigurationFile = " + DefaultArchitectureConfigurationFile);

    //DefaultPolicy
    DefaultPolicy= getStringFromXPathExpression("/Configuration/DefaultPolicy",this->doc);
    log->debugDefault("DefaultPolicy = " + DefaultPolicy);

    //DefaultParaverOutputDir
    DefaultParaverOutputDir= getStringFromXPathExpression("/Configuration/DefaultParaverOutputDir",this->doc);
    log->debugDefault("DefaultParaverOutputDir = " + DefaultParaverOutputDir);

    //DefaultParaverTraceName
    DefaultParaverTraceName= getStringFromXPathExpression("/Configuration/DefaultParaverTraceName",this->doc);
    log->debugDefault("DefaultParaverTraceName = " + DefaultParaverTraceName);

    //DebugLevel
    DebugLevel= atoi((getStringFromXPathExpression("/Configuration/DebugLevel",this->doc).c_str()));
    log->debugDefault("DebugLevel = " + to_string(DebugLevel));

    //DefaultOutPutDir
    DefaultOutPutDir= getStringFromXPathExpression("/Configuration/DefaultOutPutDir",this->doc);
    log->debugDefault("DefaultOutPutDir = " + DefaultOutPutDir);

    //JavaBinaryPath
    JavaBinaryPath= getStringFromXPathExpression("/Configuration/JavaBinaryPath",this->doc);
    log->debugDefault("JavaBinaryPath = " + JavaBinaryPath);

    //JavaClassPath
    JavaClassPath= getStringFromXPathExpression("/Configuration/JavaClassPath",this->doc);
    log->debugDefault("JavaClassPath = " + JavaClassPath);

    //WekaJarPath
    JavaClassPath= getStringFromXPathExpression("/Configuration/WekaJarPath",this->doc);
    log->debugDefault("WekaJarPath = " + WekaJarPath);

    //statisticsConfigFile
    statisticsConfigFile= getStringFromXPathExpression("/Configuration/StatisticsConfigFile",this->doc);
    log->debugDefault("statisticsConfigFile = " + statisticsConfigFile);

    //RBinaryPath
    RBinaryPath= getStringFromXPathExpression("/Configuration/RBinaryPath",this->doc);
    log->debugDefault("RBinaryPath = " + RBinaryPath);

    //RScriptsBaseDir
    RScriptsBaseDir= getStringFromXPathExpression("/Configuration/RScriptsBaseDir",this->doc);
    log->debugDefault("RScriptsBaseDir = " + RScriptsBaseDir);

    //TemporaryDirectory
    TemporaryDirectory= getStringFromXPathExpression("/Configuration/TemporaryDirectory",this->doc);
    log->debugDefault("TemporaryDirectory = " + TemporaryDirectory);

    //We extract now RA Analyzers
    xmlNodeSetPtr RAnalyzers = getNodeSetFromXPathExpression("/Configuration/RPostProcessingFiles/Script",this->doc);

    for(int i = 0; RAnalyzers && i < RAnalyzers->nodeNr;i++)
    {
        string AnalyzerType = getStringFromXPathExpression("/AnalyzerType",(xmlDocPtr) RAnalyzers->nodeTab[i],true);
        string ScriptPath = getStringFromXPathExpression("/AnalyzerType",(xmlDocPtr) RAnalyzers->nodeTab[i],true);

        if(ScriptPath.compare("") == 0 || AnalyzerType.compare("") ==  0)
        {
            continue;
            std::cout << "The configuration file has a wrong pair value of script path and csv file type" << endl;
        }
                      
        this->RScripts.insert(pair<r_scripts_type,string>(this->getAnalyzerType(AnalyzerType),ScriptPath));
    }
    xmlXPathFreeNodeSet(RAnalyzers);
  
      
    
 /* setting default values for the others ..*/

  this->policy = FCFS;
  this->workload = SWF;
  this->workloadPath = this->DefaultWorkloadPath;
  this->paraverOutPutDir = this->DefaultParaverOutputDir;
  this->paraverTrace = this->DefaultParaverTraceName;
  this->architectureFile = this->DefaultArchitectureConfigurationFile;
  this->architectureType = HPC;
}

/**
 * The default destructor for the class 
 */

ConfigurationFile::~ConfigurationFile()
{
}

/**
 * Imports the configuration from another configuration file 
 * @param ConfigFileImp The source configuration file 
 */
void ConfigurationFile::import(ConfigurationFile* ConfigFileImp)
{
  
  ConfigFile = ConfigFileImp->ConfigFile;
  DefaultWorkloadPath = ConfigFileImp->DefaultWorkloadPath; 
  DefaultArchitectureConfigurationFile = ConfigFileImp->DefaultArchitectureConfigurationFile;
  DefaultPolicy = ConfigFileImp->DefaultPolicy;
  DefaultParaverOutputDir = ConfigFileImp->DefaultParaverOutputDir;
  DefaultParaverTraceName = ConfigFileImp->DefaultParaverTraceName;
  ErrorFilePath = ConfigFileImp->ErrorFilePath;
  DebugFilePath = ConfigFileImp->DebugFilePath;
  DebugLevel = ConfigFileImp->DebugLevel;
  DefaultOutPutDir = ConfigFileImp->DefaultOutPutDir;
  policy = ConfigFileImp->policy;
  workload = ConfigFileImp->workload;
  workloadPath = ConfigFileImp->workloadPath;
//  paraverOutPutDir = ConfigFileImp->paraverOutPutDir;
  outputTraceFilename  = ConfigFileImp->paraverTrace;
  jobRequirementsFile = ConfigFileImp->jobRequirementsFile;
  GlobalStatisticsOutputFile = ConfigFileImp->GlobalStatisticsOutputFile;
}


/**
 * Given a r_analyzer returns the analyzer file type indicated in the string 
 * @param r_analyzer The string containing the analizer type
 * @return The r_analyzer indicating the analyzer type 
 */
r_scripts_type ConfigurationFile::getAnalyzerType(string r_analyzer)
{
    if(r_analyzer.compare("JOB_CSV_R_ANALYZER_BASIC") == 0)
      return JOB_CSV_R_ANALYZER_BASIC;
    else 
      return DEFAULT_CSV_R_ANALYER;
}


}

