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
#include <utils/simulationconfiguration.h>
#include <statistics/statisticsconfigurationfile.h>
#include <utils/utilities.h>
#include <cerrno>

#include <string>
#include <iostream>


namespace Utils {

    /**
     * The default constructor for the class
     */
    SimulationConfiguration::SimulationConfiguration() {
        this->init();
    }

    /**
     * Initialize all the variables for the class
     */
    void SimulationConfiguration::init() {

        /*
         * This is where the defaults are set. Not in an external file.
         */
        
        
        ConfigFile = "$PWD/simulation_configuration.xml";
        StatisticsConfigFile = "$PWD/statistics_configuration.xml";
        ErrorFilePath = "$PWD/logs/error.log";
        DebugFilePath = "$PWD/logs/debug.log";
        DebugLevel = 1;

        DSCPparaverOutPutDir = "";
        DSCPparaverTrace = "";
        // PredictorConfigFile = "";

        architectureType = HPC;
        policy = SLURM;
        workload = NNTF;
        workloadPath = "";
        defworkloadPath = "./workload.nntf";
        jobRequirementsFile = "";
        jobMonetaryRequirementsFile = "";
        outputTraceFilename = "nextgensim";
        schedCoeffDataFilename = "";
        schedCoeffCountFilename = "";
        startJobTrace = -1;
        endJobTrace = -1;
        architectureFile = "./hardware_configuration.xml";
        outPutDir = "./output";
        fileLoaded = false;
        GlobalPredictorConfigFile = "";
//        GlobalPredictionService = NULL;

        architectureLoaded = false;
        policyLoaded = false;
        workloadLoaded = false;
        DSCPmetricTypeLoaded = false;
        DSCPstatisticTypeLoaded = false;
        fileLoaded = false;
        computeEnergy = false;

        collectStatisticsInterval = 10 * 60;

        numberOfReservations = -1;
        malleableExpand = false;
        malleableShrink = false;
        // backfillingVariant = EASY_BACKFILLING;
        // SimulateResourceUsage = false;
        // AllocateAllJobsInTheRT = false;

        // PredictorConfigFile = "";
        JobsSimPerformanceCSVFile = "./output/JobsPerformanceFile.csv";
        GlobalStatisticsOutputFile = "/output/stats.out";

        ArrivalFactor = 1;
        EmulateCPUFactor = 1;
        JobsToLoad = -1;
        LoadFromJob = 1;
        LoadTillJob = -1;
        LoadFactor = -1;

        SlowdownBound = -1;

        showSimulationProgress = true;
        simPartitions = true;

        ResourceSelectionPolicy = FIRST_FIT;


        log = NULL;
        DebugFrom = 0;

        maxAllowedRuntime = -1;
        CostAllocationUnit = -1;
        AllocationUnitHour = -1;
        
        SLURMWQsize = 10;
        SLURMPmem = 0;



        //we initialize all the job information fileds that can be used by the simulator
        AttributeJobType["jobNumber"] = DOUBLE;
        AttributeJobType["jobSimSubmitTime"] = DOUBLE;
        AttributeJobType["jobSimWaitTime"] = DOUBLE;
        AttributeJobType["jobSimCenter"] = STRING;
        AttributeJobType["numberProcessors"] = INTEGER;
        AttributeJobType["requestedTime"] = DOUBLE;
        AttributeJobType["originalRequestedTime"] = DOUBLE;
        AttributeJobType["requestedMemory"] = DOUBLE;
        AttributeJobType["userId"] = INTEGER;
        AttributeJobType["groupId"] = INTEGER;
        AttributeJobType["executable"] = INTEGER;
        AttributeJobType["queueNumber"] = INTEGER;
        AttributeJobType["bwMemoryUsed"] = INTEGER;
        AttributeJobType["memoryUsed"] = DOUBLE;
        AttributeJobType["bwEthernedUsed"] = DOUBLE;
        AttributeJobType["bwNetworkUsed"] = DOUBLE;
        AttributeJobType["jobSimStartTime"] = DOUBLE;
        AttributeJobType["jobSimStatus"] = DOUBLE;
        AttributeJobType["runTime"] = DOUBLE;
        AttributeJobType["LessConsumeAlternativeThreshold"] = DOUBLE;
        
      

    }

    /**
     * The constructor that should be more used
     * @param ConfigFile The path for the configuration file
     * @param log The logging engine
     */
    SimulationConfiguration::SimulationConfiguration(string ConfigFile, Log* log, ConfigurationFile* simulatorConfiguration) {
        //Init configuration vars to default values
        this->init();

        //Expand variables
        this->ConfigFile = expandFileName(ConfigFile);
//        this->simulatorConfiguration = simulatorConfiguration;
        this->log = log;

        log->debug("Creating simulation configuration based on " + this->ConfigFile, 1);

        //Init XML2 Lib Parser
        xmlInitParser();
        //Parse the Xml document
        this->doc = xmlParseFile(this->ConfigFile.c_str());

        log->debug("Loading the simulation configuration file " + ConfigFile, 1);

        if (doc == NULL) {
            cout << "Error: The file " << this->ConfigFile << " has not been openend. Some error has occurred: " << strerror(errno) << endl;
            exit(-1);
        }
        
        
        string showSimProg = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ShowSimulationProgress", this->doc);
        if (!showSimProg.compare("1"))
            this->showSimulationProgress = true;
        else
            this->showSimulationProgress = false;

        
        
        //Workload Path
        workloadPath = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/WorkloadPath", this->doc);
        if (workloadPath == ""){
            workloadPath = "./workload.nntf";
        }
        log->debugConfig("WorkloadPath = " + workloadPath);


        
        //Workload Type
        string workloadS = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/workload", this->doc);
        if (workloadS == ""){
            workloadS = "NNTF";
        }
        
        

        this->workloadLoaded = true;
        log->debugConfig("workload = " + workloadS);

        if (workloadS == "SWF")
            this->workload = SWF;
        else if (workloadS == "SWFExtReqs" || workloadS == "SWFExt" /* for backward compatibility */)
            this->workload = SWFExtReqs;
        else if (workloadS == "SWFExtCost")
            this->workload = SWFExtCosts;
        else if (workloadS == "PBSPro")
            this->workload = PBSPro;
        else if (workloadS == "NNTF")
                this->workload = NNTF;
        //else if(workloadS == "SWFGrid")
        //	this->workload = SWFGrid;
        
        //Architecture File
        architectureFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ArchitectureConfigurationFile", this->doc);
        if (architectureFile == ""){
            architectureFile = "./hardware_configuration.xml";
        }
        log->debugConfig("ArchitectureConfigurationFile = " + architectureFile);

        string archType = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ArchitectureType", this->doc);
        if (archType == ""){
            archType = "HPC";
        }
        log->debugConfig("ArchitectureType = " + archType);

        architectureType = getArchitectureType(archType);
        architectureLoaded = true;

        //Statistics configuration file
        StatisticsConfigFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/StatisticsConfigFile", this->doc);
        if (StatisticsConfigFile == ""){
            StatisticsConfigFile = "./statistics_configuration.xml";
        }
        log->debugConfig("StatisticsConfigFile = " + StatisticsConfigFile);

        
        //Policy
        string policy_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/Policy", this->doc);
        if (policy_s == ""){
            policy_s = "SLURM";
        }
        log->debugConfig("Policy = " + policy_s);

        this->policyLoaded = true;
        this->policy = getPolicyType(policy_s);


        //Output statistics output file
        GlobalStatisticsOutputFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/GlobalStatisticsOutputFile", this->doc);
        if (GlobalStatisticsOutputFile == ""){
            GlobalStatisticsOutputFile = "./output/stats.out";
        }
        log->debugConfig("GlobalStatisticsOutputFile = " + GlobalStatisticsOutputFile);

        //Paraver trace name
        //	paraverTrace = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ParaverTraceName",this->doc);
        //	log->debugConfig("ParaverTraceName = \"" + paraverTrace +"\"");

        this->outputTraceFilename = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/outputTraceFilename", this->doc);
        if (outputTraceFilename == ""){
            outputTraceFilename = "nextgensim_output";
        }
        log->debugConfig("outputTraceFilename = " + this->outputTraceFilename);
        
        this->schedCoeffDataFilename = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/schedCoeffDataFileName", this->doc);
        log->debugConfig("schedCoeffDataFilename = " + this->schedCoeffDataFilename);

        this->schedCoeffCountFilename = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/schedCoeffCountFileName", this->doc);
        log->debugConfig("schedCoeffCountFilename = " + this->schedCoeffCountFilename);


        //start Job Trace
        startJobTrace = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/startJobTrace", this->doc)).c_str());
        if (startJobTrace == 0)startJobTrace = -1;
        log->debugConfig("startJobTrace = " + to_string(startJobTrace));

        //end Job Trace
        endJobTrace = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/endJobTrace", this->doc)).c_str());
        if (endJobTrace == 0) endJobTrace = -1;
        log->debugConfig("startJobTrace = " + to_string(endJobTrace));

        //DebugLevel
        DebugLevel = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/DebugLevel", this->doc)).c_str());
        log->debugConfig("DebugLevel = " + to_string(DebugLevel));


        //DebugFilePath
        DebugFilePath = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/DebugFilePath", this->doc);
        if (DebugFilePath == ""){
            DebugFilePath = "./logs/debug.log";
        }
        log->debugConfig("DebugFilePath = " + DebugFilePath);
        

        //ErrorFilePath
        ErrorFilePath = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ErrorFilePath", this->doc);
        if (ErrorFilePath == ""){
            ErrorFilePath = "./logs/error.log";
        }
        log->debugConfig("ErrorFilePath = " + ErrorFilePath);
        

        //OutPutDir
        outPutDir = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/OutPutDir", this->doc);
        if (outPutDir == ""){
            outPutDir = "./output/";
        }
        log->debugConfig("OutPutDir = " + outPutDir);

        //ArrivalFactor
        ArrivalFactor = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ArrivalFactor", this->doc)).c_str());
        if (ArrivalFactor == 0) ArrivalFactor = 1;
        log->debugConfig("ArrivalFactor = " + ftos(ArrivalFactor));

        //Bounded Slowdown
        SlowdownBound = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/SlowdownBound", this->doc)).c_str());
        if (SlowdownBound == 0) SlowdownBound = -1;
        log->debugConfig("SlowdownBound = " + ftos(SlowdownBound));

        //generateParaver
        string generateParaver_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/generateParaver", this->doc);

        if (!generateParaver_s.compare("") || !generateParaver_s.compare("NO")) {
            this->generateOutputTrace = false;
            log->debugConfig("generateParaver = FALSE");
        } else {
            this->generateOutputTrace = true;
            log->debugConfig("generateParaver = TRUE");
        }

        


        //generateSWF
        string generateSWF_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/generateSWF", this->doc);

        if (!generateSWF_s.compare("") || !generateSWF_s.compare("NO")) {
            this->generateSWF = false;
            log->debugConfig("generateSWF = FALSE");
        } else {
            this->generateSWF = true;
            log->debugConfig("generateSWF = TRUE");
        }

        

//        //simPartitions
//        string simPartitions_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/SimulatePartitions", this->doc);
//
//        if (!simPartitions_s.compare("") || !simPartitions_s.compare("NO")) {
//            this->simPartitions = false;
//            log->debugConfig("simPartitions = FALSE");
//        } else {
//            this->simPartitions = true;
//            log->debugConfig("simPartitions = TRUE");
//        }

        


        //reserveFullNode
        string reserveFullNode_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ReserveFullNode", this->doc);

        if (!reserveFullNode_s.compare("") || !reserveFullNode_s.compare("NO")) {
            this->reserveFullNode = false;
            log->debugConfig("reserveFullNode = FALSE");
        } else {
            this->reserveFullNode = true;
            log->debugConfig("reserveFullNode = TRUE");
        }

        


        //computeEnergy
        string computeEnergy_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/computeEnergy", this->doc);

        if (!computeEnergy_s.compare("") || !computeEnergy_s.compare("NO")) {
            this->computeEnergy = false;
            log->debugConfig("computeEnergy = FALSE");
        } else {
            log->debugConfig("computeEnergy = TRUE");
            this->computeEnergy = true;
        }

        //Collect statistics interval
        collectStatisticsInterval = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/collectStatisticsInterval", this->doc)).c_str());
        log->debugConfig("collectStatisticsInterval = " + ftos(collectStatisticsInterval));

        if (collectStatisticsInterval == 0) collectStatisticsInterval = 18000;

        //jobRequirementsFile
        jobRequirementsFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/jobRequirementsFile", this->doc);
        log->debugConfig("jobRequirementsFile = " + jobRequirementsFile);

        //jobMonetaryRequirementsFile
        jobMonetaryRequirementsFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/jobMonetaryRequirementsFile", this->doc);
        log->debugConfig("jobMonetaryRequirementsFile = " + jobMonetaryRequirementsFile);

        //EmulateCPUFactor
        EmulateCPUFactor = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/EmulateCPUFactor", this->doc)).c_str());
        if (EmulateCPUFactor == 0) EmulateCPUFactor = 1;
        log->debugConfig("EmulateCPUFactor = " + to_string(EmulateCPUFactor));


        //maxAllowedRuntime
        maxAllowedRuntime = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/maxAllowedRuntime", this->doc)).c_str());
        log->debugConfig("maxAllowedRuntime = " + to_string(maxAllowedRuntime));

        //LoadFactor
        LoadFactor = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/LoadFactor", this->doc)).c_str());
        if (LoadFactor == 0) LoadFactor = -1;
        log->debugConfig("LoadFactor = " + ftos(LoadFactor));

        //JobsToLoad
        JobsToLoad = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/JobsToLoad", this->doc)).c_str());
        if (JobsToLoad == 0) JobsToLoad = -1;
        log->debugConfig("JobsToLoad = " + to_string(JobsToLoad));

        //LoadFromJob
        LoadFromJob = atoi((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/LoadFromJob", this->doc)).c_str());
        if (LoadFromJob == 0) LoadFromJob = -1;
        log->debugConfig("LoadFromJob = " + to_string(LoadFromJob));

        //CostAllocationUnit
        CostAllocationUnit = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/CostAllocationUnit", this->doc)).c_str());
        if (CostAllocationUnit == 0) CostAllocationUnit = -1;
        log->debugConfig("CostAllocationUnit = " + ftos(CostAllocationUnit));

        //AllocationUnitHour
        AllocationUnitHour = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/AllocationUnitHour", this->doc)).c_str());
        if (AllocationUnitHour == 0) AllocationUnitHour = -1;
        log->debugConfig("AllocationUnitHour = " + ftos(AllocationUnitHour));

        //maxAllowedRuntime
        maxAllowedRuntime = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/maxAllowedRuntime", this->doc)).c_str());
        if (maxAllowedRuntime == 0) maxAllowedRuntime = -1;
        log->debugConfig("maxAllowedRuntime = " + ftos(maxAllowedRuntime));

        //LoadTillJob
        LoadTillJob = atof((getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/LoadTillJob", this->doc)).c_str());
        if (LoadTillJob == 0) LoadTillJob = -1;
        log->debugConfig("LoadTillJob = " + ftos(LoadTillJob));

        //ResourceSelectionPolicy
        string ResourceSelectionPolicy_s = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/ResourceSelectionPolicy", this->doc);
        if (ResourceSelectionPolicy_s == ""){
            ResourceSelectionPolicy_s = "FF_AND_FCF";
        }
        this->ResourceSelectionPolicy = getResourceSelectionPolicyType(ResourceSelectionPolicy_s);
        log->debugConfig("ResourceSelectionPolicy = " + ResourceSelectionPolicy_s);




        //JobsSimPerformanceCSVFile
        JobsSimPerformanceCSVFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/JobsSimPerformanceCSVFile", this->doc);
        log->debugConfig("JobsSimPerformanceCSVFile = " + JobsSimPerformanceCSVFile);

        //PolicySimPerfomanceCSVFile
        PolicySimPerformanceCSVFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/PolicySimPerfomanceCSVFile", this->doc);
        log->debugConfig("PolicySimPerfomanceCSVFile = " + PolicySimPerformanceCSVFile);

        //SWFOutputFile
        SWFOutputFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/SWFOutputFile", this->doc);
        log->debugConfig("SWFOutputFile = " + SWFOutputFile);

        //PredictionServiceConfigFile
        GlobalPredictorConfigFile = getStringFromXPathExpression("/SimulationConfiguration/Policy/Common/PredictionServiceConfigFile", this->doc);
        log->debugConfig("PredictionServiceConfigFile = " + GlobalPredictorConfigFile);

        //We extract now RA Analyzers
//        xmlNodeSetPtr RAnalyzers = getNodeSetFromXPathExpression("/SimulationConfiguration/Policy/Common/RAnalyzers/Analyzer", this->doc);
//
//        for (int i = 0; RAnalyzers && i < RAnalyzers->nodeNr; i++) {
//            string AnalyzerType = getStringFromXPathExpression("AnalyzerType", (xmlDocPtr) RAnalyzers->nodeTab[i]);
//
//            r_scripts_type analyzer = simulatorConfiguration->getAnalyzerType(AnalyzerType);
//            map<r_scripts_type, string>::iterator has = simulatorConfiguration->RScripts.find(analyzer);
//
//            if (has != simulatorConfiguration->RScripts.end())
//                this->RAnalyzers.push_back(analyzer);
//            else
//                this->log->error("The analyzer " + AnalyzerType + " is not described in the simulator configuration file");
//        }
//        xmlXPathFreeNodeSet(RAnalyzers);
        
        /* POLICY SPECIFIC FIELDS */

        xmlNodePtr FCFS = getNodeFromXPathExpression("/SimulationConfiguration/Policy/FCFS", this->doc);
        if (FCFS) extractFCFSFields(true, FCFS, NULL);
        
        xmlNodePtr SLURM = getNodeFromXPathExpression("/SimulationConfiguration/Policy/SLURM", this->doc);
        if (SLURM) extractSLURMFields(true, SLURM, NULL);



        fileLoaded = true;


    }

    /**
     * The default destructor for the class
     */
    SimulationConfiguration::~SimulationConfiguration() {
    }

    /**
     * Imports the information of a configuration file class if the same information in the simulation configuration file is not specified
     * @param config The configuration file
     * @see The class ConfigurationFile
     */
    void SimulationConfiguration::import(ConfigurationFile* config) {
        //this function only import the values that are no present in the current simulation configuration
        //remember that the simulation file has more priority that the configration file with those fields
        //that are related the simulation
        if (!policyLoaded)
            policy = config->policy;
        if (!workloadLoaded)
            workload = config->workload;
        if (workloadPath.size() <= 1)
            workloadPath = config->workloadPath;
        if (!workloadLoaded)
            this->workload = config->workload;
        if (jobRequirementsFile.size() <= 1)
            jobRequirementsFile = config->jobRequirementsFile;
        //	if(paraverOutPutDir.size() <= 1)
        //		paraverOutPutDir = config->paraverOutPutDir;
        if (outputTraceFilename.size() <= 1)
            outputTraceFilename = config->outputTraceFilename;
        if (architectureFile.size() <= 1)
            architectureFile = config->architectureFile;
        if (!architectureLoaded)
            architectureType = config->architectureType;
        if (outPutDir.size() <= 1)
            outPutDir = config->DefaultOutPutDir;
        if (DebugLevel == -1)
            DebugLevel = config->DebugLevel;
        if (DebugFilePath.size() <= 1)
            DebugFilePath = config->DebugFilePath;
        if (ErrorFilePath.size() <= 1)
            ErrorFilePath = config->ErrorFilePath;
        if (StatisticsConfigFile.size() <= 1)
            StatisticsConfigFile = config->statisticsConfigFile;
        // if(LessConsumeAlternativeThreshold < 0)
        // LessConsumeAlternativeThreshold = config->LessConsumeAlternativeThreshold;

    }

    /**
     * Imports the information of a simulation configuration  class if the same information in the simulation configuration file is not specified
     * @param ConfigFile The configuration file
     * @see The class ConfigurationFile
     */
    void SimulationConfiguration::import(SimulationConfiguration* ConfigFile) {

    }

    /***********************************************************************************************
     ********************PARSING TYPE FUNCTIONS  ****************************************************
     ************************************************************************************************/

    /**
     * Given an string returns the architecture type
     * @param archType The architecture type in string format
     * @return The architecture_type_t containing the architecture type specified in the parameter
     */
    architecture_type_t SimulationConfiguration::getArchitectureType(string archType) {
        assert(archType != "");

        if (archType == "HPC")
            architectureType = HPC;
        else
            printf("Error with the architecture type field in the common part .. %s\n", archType.c_str());

        return architectureType;
    }

    architecture_type_t SimulationConfiguration::getArchitectureType() {
        return architectureType;
    }

    /**
     * Given an string returns the policy type
     * @param spolicy The policy type in string format
     * @return The policy_type_t containing the policy type specified in the parameter
     */
    policy_type_t SimulationConfiguration::getPolicyType(string spolicy) {
        if (spolicy == "EASY")
            return EASY;
        else if (spolicy == "DEEP")
            return DEEP;
        if (spolicy == "FCFS")
            return FCFS;
        else if (spolicy == "GRID_BACKFILLING")
            return GRID_BACKFILLING;
        else if (spolicy == "ISIS_DISPATCHER")
            return ISIS_DISPATCHER;
        else if (spolicy == "BRANK_POLICY")
            return BRANK_POLICY;
        else if (spolicy == "META_BROKERING_SYSTEM")
            return META_BROKERING_SYSTEM;
        else if (spolicy == "SLURM")
            return SLURM;
        else
            printf("Error with the policy field in the common part .. %s\n", spolicy.c_str());

        return OTHER_POLICY;
    }

    policy_type_t SimulationConfiguration::getPolicyType() {
        return policy;
    }

    /**
     * Given an string returns the statistic type
     * @param estimator The policy type in string format
     * @return The statistic_t containing the statistic type specified in the parameter
     */
    statistic_t SimulationConfiguration::getStatisticType(string estimator) {
        if (estimator == "MEAN") {
            return AVG;
        } else if (estimator == "MEDIAN") {
            return MEDIAN;
        } else if (estimator == "PERCENTILE95") {
            return PERCENTILE95;
        } else if (estimator == "MIN") {
            return MIN;
        } else if (estimator == "MAX") {
            return MAX;
        } else if (estimator == "STDEV") {
            return STDEV;
        } else if (estimator == "IQR") {
            return IQR;
        } else if (estimator == "ACCUMULATE") {
            return ACCUMULATE;
        } else
            return OTHER_STAT;

    }

    /**
     * Given an string returns the Resource Selection Policy type
     * @param rspolicy The Resource Selection Policy type in string format
     * @return The RS_policy_type_t containing the policy type specified in the parameter
     */
    RS_policy_type_t SimulationConfiguration::getResourceSelectionPolicyType(string rspolicy) {
        //would look like a mistake in the configuration file
        assert(rspolicy != "");

        if (rspolicy == "FIRST_FIT")
            return FIRST_FIT;
        else if (rspolicy == "FIRST_CONTINUOUS_FIT")
            return FIRST_CONTINUOUS_FIT;
        else if (rspolicy == "FF_AND_FCF")
            return FF_AND_FCF;
        else if (rspolicy == "FIND_LESS_CONSUME")
            return FIND_LESS_CONSUME;
        else if (rspolicy == "FIND_LESS_CONSUME_THRESSHOLD")
            return FIND_LESS_CONSUME_THRESSHOLD;
        else if (rspolicy == "EQUI_DISTRIBUTE_CONSUME")
            return EQUI_DISTRIBUTE_CONSUME;
        else if (rspolicy == "DEFAULT_RS_P")
            return DEFAULT_RS_P;
        else if (rspolicy == "DC_FIRST_VM")
            return DC_FIRST_VM;
        else
            return FF_AND_FCF; //by default we use the First fit and first continuous fit

    }

    RS_policy_type_t SimulationConfiguration::getResourceSelectionPolicyType() {
        return ResourceSelectionPolicy;
    }

    /**
     * Given an string returns the metric type
     * @param variable The metric type in string format
     * @return The metric_t containing the metric type specified in the parameter
     */
    metric_t SimulationConfiguration::getMetricType(string variable) {
        if (variable == "SLD") {
            return SLD;
        } else if (variable == "BSLD") {
            return BSLD;
        } else if (variable == "WAITTIME") {
            return WAITTIME;
        } else if (variable == "BACKFILLED_JOB") {
            return BACKFILLED_JOB;
        } else if (variable == "kILLED_JOB") {
            return kILLED_JOB;
        } else if (variable == "BACKFILLED_JOBS") {
            return BACKFILLED_JOBS;
        } else if (variable == "JOBS_IN_WQUEUE") {
            return JOBS_IN_WQUEUE;
        } else if (variable == "CPUS_USED") {
            return CPUS_USED;
        } else if (variable == "LEAST_WORK_LEFT") {
            return LEAST_WORK_LEFT;
        } else if (variable == "SUBMITED_JOBS") {
            return SUBMITED_JOBS;
        } else if (variable == "SUBMITED_JOBS_CENTER") {
            return SUBMITED_JOBS_CENTER;
        } else if (variable == "RSP_USED") {
            return RSP_USED;
        } else if (variable == "PENALIZED_RUNTIME") {
            return PENALIZED_RUNTIME;
        } else if (variable == "PERCENTAGE_PENALIZED_RUNTIME") {
            return PERCENTAGE_PENALIZED_RUNTIME;
        } else if (variable == "LOAD_JOBS_CENTERS") {
            return LOAD_JOBS_CENTERS;
        } else if (variable == "COMPUTATIONAL_COST") {
            return COMPUTATIONAL_COST;
        } else if (variable == "ESTIMATED_COMPUTATIONAL_COST") {
            return ESTIMATED_COMPUTATIONAL_COST;
        } else if (variable == "PREDICTED_WT") {
            return PREDICTED_WT;
        } else if (variable == "PREDICTED_RUNTIME") {
            return PREDICTED_RUNTIME;
        } else if (variable == "SLD_PREDICTED") {
            return SLD_PREDICTED;
        } else if (variable == "ESTIMATED_COMPUTATIONAL_COST_ERROR") {
            return ESTIMATED_COMPUTATIONAL_COST_ERROR;
        } else if (variable == "COMPUTATIONAL_COST_RESPECTED") {
            return COMPUTATIONAL_COST_RESPECTED;
        } else if (variable == "PREDICTED_WT_ERROR") {
            return PREDICTED_WT_ERROR;
        } else if (variable == "PREDICTED_RUNTIME_ERROR") {
            return PREDICTED_RUNTIME_ERROR;
        } else if (variable == "SLD_PREDICTED_ERROR") {
            return SLD_PREDICTED_ERROR;
        } else if (variable == "ESTIMATED_START_TIME_FIRST_FIT") {
            return ESTIMATED_START_TIME_FIRST_FIT;
        } else
            assert(false);
    }



    /***********************************************************************************************
     ********************PARSING XML FUNCTIONS  ****************************************************
     ************************************************************************************************/


    /**
     * Function that given a XML node reference extracts the EASY information
     * @param here Indicates if the information should be extracted to the global variables or if a new grid backfilling class should be created
     * @param xmlNodePtr The conext node where the xml fields are included
     * @param architectureConfig The architecture configuration that will be used by the scheduling policy
     * @return A reference to the class EASY containing the information specified in the XML node or null in case that the information has been dumped in the global variables.
     */
    FCFSSchedulingPolicy* SimulationConfiguration::extractFCFSFields(bool here, xmlNodePtr fcfsFields, ArchitectureConfiguration* architectureConfig) {

        bool SimulateResourceUsageCur = false;
        FCFSSchedulingPolicy* policy = NULL;
        xmlDocPtr fcfsDoc = (xmlDocPtr) fcfsFields;

        //SimulateResourceUsage
        string SimulateResourceUsage_s = getStringFromXPathExpression("/SimulateResourceUsage", fcfsDoc);
        log->debugConfig("SimulateResourceUsage = " + SimulateResourceUsage_s);

        if (SimulateResourceUsage_s.compare("YES") == 0 || SimulateResourceUsage_s.compare("1") == 0)
            SimulateResourceUsageCur = true;

        //reservationdepth
        string ReservationDepth_s = getStringFromXPathExpression("/ReservationDepth", fcfsDoc);
        int ReservationDepthCur = atoi(ReservationDepth_s.c_str());
        //int ReservationDepthCur = atoi(getStringFromXPathExpression("/ReservationDepth",fcfsDoc)).c_str());
        if (ReservationDepthCur < 0)
            ReservationDepthCur = -1;

        //malleablility
        bool malexpand = false;
        bool malshrink = false;
        string malexpand_s = getStringFromXPathExpression("/MalleabilityExpand", fcfsDoc);
        if (malexpand_s.compare("YES") == 0 || malexpand_s.compare("1") == 0)
            malexpand = true;
        string malshrink_s = getStringFromXPathExpression("/MalleabilityShrink", fcfsDoc);
        if (malshrink_s.compare("YES") == 0 || malshrink_s.compare("1") == 0)
            malshrink = true;

        if (here) {
            SimulateResourceUsage = SimulateResourceUsageCur;
            numberOfReservations = ReservationDepthCur;
            malleableExpand = malexpand;
            malleableShrink = malshrink;
        } else {
            //the architecture, the log and the startime will have to be set for other functionality
            return (FCFSSchedulingPolicy*) this->CreateFCFSPolicy(architectureConfig, this->log, 0, SimulateResourceUsageCur, numberOfReservations, malexpand, malshrink);
        }

        return policy;

    }
    
    SLURMSchedulingPolicy* SimulationConfiguration::extractSLURMFields(bool here, xmlNodePtr slurmFields, ArchitectureConfiguration* architectureConfig) {

        bool SimulateResourceUsageCur = false;
        SLURMSchedulingPolicy* policy = NULL;
        xmlDocPtr slurmDoc = (xmlDocPtr) slurmFields;

//        //SimulateResourceUsage
//        string SimulateResourceUsage_s = getStringFromXPathExpression("/SimulateResourceUsage", slurmDoc);
//        log->debugConfig("SimulateResourceUsage = " + SimulateResourceUsage_s);
//
//        if (SimulateResourceUsage_s.compare("YES") == 0 || SimulateResourceUsage_s.compare("1") == 0)
//            SimulateResourceUsageCur = true;
//
        //reservationdepth
        string ReservationDepth_s = getStringFromXPathExpression("/ReservationDepth", slurmDoc);
        int ReservationDepthCur = atoi(ReservationDepth_s.c_str());
        if (ReservationDepthCur < 0)
            ReservationDepthCur = -1;
        
        string WQScheduleDepth_s = getStringFromXPathExpression("/WQScheduleDepth", slurmDoc);
        int WQScheduleDepth = atoi(WQScheduleDepth_s.c_str());
        
        string SchedulerSkipTime_s = getStringFromXPathExpression("/SchedulerSkipTime", slurmDoc);
        int SchedulerSkipTime = atoi(SchedulerSkipTime_s.c_str());
        
        string PersistentMemory_s = getStringFromXPathExpression("/ModelPersistentMemory", slurmDoc);
        int PersistentMemory = atoi(PersistentMemory_s.c_str());

        //malleablility
        bool malexpand = false;
        bool malshrink = false;
//        string malexpand_s = getStringFromXPathExpression("/MalleabilityExpand", slurmDoc);
//        if (malexpand_s.compare("YES") == 0 || malexpand_s.compare("1") == 0)
//            malexpand = true;
//        string malshrink_s = getStringFromXPathExpression("/MalleabilityShrink", slurmDoc);
//        if (malshrink_s.compare("YES") == 0 || malshrink_s.compare("1") == 0)
//            malshrink = true;

        if (here) {
//            SimulateResourceUsage = SimulateResourceUsageCur;
            numberOfReservations = ReservationDepthCur;
//            malleableExpand = malexpand;
//            malleableShrink = malshrink;
            SLURMWQsize = WQScheduleDepth;
            SLURMSkipTime = SchedulerSkipTime;
            SLURMPmem = PersistentMemory;
        } else {
            //the architecture, the log and the startime will have to be set for other functionality
            return (SLURMSchedulingPolicy*) this->CreateSLURMPolicy(architectureConfig, this->log, 0, SimulateResourceUsageCur, numberOfReservations, malexpand, malshrink);
        }

        return policy;

    }




//    /**
//     * Creates a predictor
//     * @param type The type of predictor to be instantiated
//     * @param configPred The configuration information for the predictor
//     * @return A reference to the predictor created
//     */
//
//    Predictor* SimulationConfiguration::CreatePredictor(PredictorConfigurationFile* configPred) {
//        Predictor* ret = NULL;
//
//        switch (configPred->predictorType) {
//            case USER_RUNTIME_ESTIMATE:
//            {
//                ret = new UserRuntimeEstimatePred(this->log);
//                break;
//            }
//            case HISTORICAL_CLASSIFIER:
//            {
//                //we add the prediction variable to the back of the response variable
//                vector<native_t>* ResponseJobAttributesType = this->GetAttributeTypes(configPred->ResponseJobAttributes);
//
//                //we get thre prediction variable type
//                map<string, native_t>::iterator itAtType = this->AttributeJobType.find(configPred->PredictionVariable);
//                assert(itAtType != this->AttributeJobType.end());
//                native_t predT = itAtType->second;
//
//
//                ret = new HistoricalClassifier(configPred->classifierType, configPred->discretizer, configPred->numberOfBins, log, configPred->ResponseJobAttributes, ResponseJobAttributesType, configPred->ModelDirectoryFile, configPred->NewFilePerModel, configPred->ModelIntervalGeneration, configPred->PredictionVariable, predT);
//
//                //now we set the weka envs to the classifier
//                ((HistoricalClassifier*) ret)->setWekaEnv(this->simulatorConfiguration->JavaBinaryPath,
//                        this->simulatorConfiguration->JavaClassPath,
//                        this->simulatorConfiguration->WekaJarPath,
//                        this->simulatorConfiguration->TemporaryDirectory
//                        );
//
//                break;
//            }
//            default:
//                assert(false);
//        }
//
//        ret->setModelIntervalGeneration(configPred->ModelIntervalGeneration);
//
//        return ret;
//    }

//    /**
//     * Create a deadlinemiss manager
//     * @param type The file type for the manager that has to be created
//     * @return A reference to the deadlinemiss manager created
//     */
//    PredDeadlineMissManager* SimulationConfiguration::CreateDeadlineManager(DeadlineMissManager_t type) {
//        PredDeadlineMissManager* ret = NULL;
//
//        switch (type) {
//            case GRADUAL:
//            {
//                ret = new PredDeadlineMissManagerGradual();
//                break;
//            }
//            case FACTOR:
//            {
//                ret = new PredDeadlineMissManagerExp();
//                break;
//            }
//            default:
//                assert(false);
//        }
//
//        return ret;
//    }

    /**
     * Creates the architecture specified in the configuration file
     * @param log The logging engine
     * @return A reference to the created architecture
     */
    ArchitectureConfiguration* SimulationConfiguration::CreateArchitecture(Log* log, double startime) {
        assert(this->architectureFile != "");
        assert(architectureLoaded);
        ArchitectureConfiguration *arch = new ArchitectureConfiguration(this->architectureFile, log, startime, architectureType);
        arch->loadConfigurationFromFile();

        return arch;
    }

    /**
     * Creates the workload specified in the configuration file
     * @param log The logging engine
     * @return A reference to the created workload
     */
    TraceFile* SimulationConfiguration::CreateWorkload(Log* log) {
        if (this->workload == SWFExtCosts)
            return CreateWorkload(log, this->workload, this->workloadPath, this->jobMonetaryRequirementsFile);
        else
            return CreateWorkload(log, this->workload, this->workloadPath, this->jobRequirementsFile);
    }

    /**
     * Creates the workload specified in the parameters
     * @param log The logging engine
     * @param workloadReq The workload type required
     * @param workloadPathReq The file path for the workload
     * @param jobRequirementsFileReq The job requirements for the workload path (may be null)
     * @return A reference to the created workload
     */
    TraceFile* SimulationConfiguration::CreateWorkload(Log* log, workload_type_t workloadReq, string workloadPathReq, string jobRequirementsFileReq) {

        TraceFile* workloadRet = NULL;

        string TraceType;
        
        char name[256];
        workload_type_t automatic_wltype;

        if (!fileExists(workloadPathReq)) {
            log->error("File " + workloadPathReq + " does not exists please check the configuration file");
            exit(1); // This is a bit abdrupt, error and leave nicely...
        }

        // Open the input file, read the identifier line and switch on that.
        std::ifstream trace(workloadPathReq);
        if (trace.is_open()) {
            trace.getline(name, 256, ' ');
            trace.close();
        } else {
            log->error("Unable to open workload file for filetype analysis");
            exit(1); // This is a bit abdrupt, error and leave nicely...
        }
        string line = name;
        log->debug("Header line for auto id = " + line, 6);
        
        
        // Chop off initial #. Match against using the switch. Fail off if no matches.
        int s_len = line.length();
        size_t found = line.find_first_of('#');
        if (found == 0){
            TraceType = line.substr(1, s_len-1);
        } else {
            TraceType = line;
        }
            
        log->debug("TraceType = " + TraceType, 2);
        
        if (TraceType == "SWF") {
            automatic_wltype = SWF;
        } else if (TraceType == "PBSPro") {
            automatic_wltype = PBSPro;
        } else if (TraceType == "NNTF") {
            automatic_wltype = NNTF;
        } else {
            automatic_wltype = OTHER_WORKLOAD;
            cout << "Invalid workload type";
        }

        switch (automatic_wltype) {
            case SWF:
                workloadRet = (TraceFile*) new SWFTraceFile(workloadPathReq, log);
                break;
            case PBSPro:
                workloadRet = (TraceFile*) new pbsInput(workloadPathReq, log);
                break;
            case NNTF:
                workloadRet = (TraceFile*) new nntfInput(workloadPathReq, log);
                break;
            default:
                assert(0);
        }

        if (this->JobsToLoad == 0) {
            std::cout << " ERROR: At least one job has to be loaded " << endl;
            exit(-1);
        }

        workloadRet->setJobsToLoad(this->JobsToLoad);
        workloadRet->setLoadFromJob(this->LoadFromJob);
        workloadRet->setLoadTillJob(this->LoadTillJob);

        log->debug("Loading " + ftos(workloadRet->getJobsToLoad()) + " jobs (-1 means all the jobs )", 1);

        if (workloadRet->loadTrace())
            log->debug("The requirements trace has been load correctly.", 1);
        else
            printf("Some errors has occurred when loading the resource trace file .. please check the error log.\n");

        return workloadRet;
    }

    /**
     * Creates a FCFS policy based on the global variables
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the FCFS policy created
     */
    SchedulingPolicy* SimulationConfiguration::CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime, bool SimulateResourceUsage, int numberOfReservations, bool MalleableExpand, bool MalleableShrink) {
        log->debug("Creating a FCFS policy simulator.", 1);
        SchedulingPolicy* policyRet = new FCFSSchedulingPolicy(architectureConfig, log, startime, this->SimulateResourceUsage, this->numberOfReservations, this->malleableExpand, this->malleableShrink);
        policyRet->simulateResourceUsage = SimulateResourceUsage;
        policyRet->numberOfReservations = numberOfReservations;
        policyRet->malleableExpand = MalleableExpand;
        policyRet->malleableShrink = MalleableShrink;

        return policyRet;

    }

    /**
     * Creates a FCFS policy based on the global variables
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the FCFS policy created
     */
    SchedulingPolicy* SimulationConfiguration::CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime) {
        log->debug("Creating a FCFS policy simulator.", 1);

        return this->CreateFCFSPolicy(architectureConfig, log, startime, this->SimulateResourceUsage, this->numberOfReservations, this->malleableExpand, this->malleableShrink);
    }
    
    
    /**
     * Creates a SLURM policy based on the global variables
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the FCFS policy created
     */
    SchedulingPolicy* SimulationConfiguration::CreateSLURMPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime, bool SimulateResourceUsage, int numberOfReservations, bool MalleableExpand, bool MalleableShrink) {
        log->debug("Creating a SLURM policy simulator.", 1);
        SchedulingPolicy* policyRet = new SLURMSchedulingPolicy(architectureConfig, log, startime, this->SimulateResourceUsage, this->numberOfReservations, this->malleableExpand, this->malleableShrink);
        policyRet->simulateResourceUsage = SimulateResourceUsage;
        policyRet->numberOfReservations = numberOfReservations;
        policyRet->malleableExpand = MalleableExpand;
        policyRet->malleableShrink = MalleableShrink;

        return policyRet;

    }

    /**
     * Creates a FCFS policy based on the global variables
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the FCFS policy created
     */
    SchedulingPolicy* SimulationConfiguration::CreateSLURMPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime) {
        log->debug("SUPERSCOPE Creating a SLURM policy simulator.", 1);

        return this->CreateSLURMPolicy(architectureConfig, log, startime, this->SimulateResourceUsage, this->numberOfReservations, this->malleableExpand, this->malleableShrink);
    }


    /**
     * Creates a EASY policy based in the global variables
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return The reference to the EASY policy
     */
    /*
    SchedulingPolicy* SimulationConfiguration::CreateEASYPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime)
    {
            return CreateEASYPolicy(architectureConfig,log,startime,this->PredictorConfigFile,this->numberOfReservations,this->backfillingVariant,this->AllocateAllJobsInTheRT,SimulateResourceUsage,LessConsumeAlternativeThreshold,LessConsumeThreshold,tryToBackfillAllways, this->powerAwareBackfilling);
    }
     */
    /**
     * Creates a EASY policy based in the provided parameters
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @param PredictorConfigFileReq The predictor configuration file, in case that is required
     * @param numberOfReservationsReq The number of reservations to be used
     * @param variantReq The backfilling variant
     * @param AllocateAllJobsInTheRT Indicates is the allocations must be done in all the queued jobs
     * @param SimulateResourceUsage If the resource usage and job collisions have to be simulated
     * @param LessConsumeAlternativeThreshold Indicates the alternative threshold that can be used in the LessConsumeThreshold, when the basic once is not specified
     * @param LessConsumeThreshold Indicates the maximum allowed threshold that can be used in the LessConsumeThreshold, if -1 means that the LessConsume has to be used.
     * @param tryToBackfillAllways Indicates if all the jobs have tried to be backfilled
     * @param powerAware Indicates while this is a power aware variant
     * @return The reference to the EASY policy
     */
    /*
    SchedulingPolicy* SimulationConfiguration::CreateEASYPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime,string PredictorConfigFileReq, int numberOfReservationsReq,Backfilling_variant_t variantReq, bool AllocateAllJobsInTheRT,bool SimulateResourceUsage,double LessConsumeAlternativeThreshold, double LessConsumeThreshold, bool tryToBackfillAllways, bool powerAwareBackFilling)
    {
            SchedulingPolicy* policyRet = NULL;

            printf("Creating a EASY policy simulator.. \n");

            if(SimulateResourceUsage)
            {
                    printf("Simulate Resources enabled\n");
                    if(backfillingVariant == RUA_BACKFILLING)
                    {
                            cout << "Creating RUABackfilling " << endl;
                            policyRet = new RUABackfilling(architectureConfig,log,startime,backfillingVariant);
                    }
                    else if(backfillingVariant == POWER_AWARE_BACKFILLING)
                    {
                            cout << "Creating Power Aware backfilling " << endl;
                            policyRet = new PowerAwareBackfilling(architectureConfig,log,startime,backfillingVariant);
                    }
                    else
                    {
                            cout << "Creating EASY with resource model " << endl;
                            policyRet = new EASYWithCollisionSchedulingPolicy(architectureConfig,log,startime,backfillingVariant);
                    }

                    if(LessConsumeThreshold > 0)
                    {
                            ((EASYWithCollisionSchedulingPolicy*) policyRet)->setThresshold(LessConsumeThreshold);

                            if(LessConsumeAlternativeThreshold > 0)
                                    ((EASYWithCollisionSchedulingPolicy*) policyRet)->setAlternativeThreshold(LessConsumeAlternativeThreshold);
                    }
            }
            else if(PredictorConfigFileReq.compare("") != 0 || this->GlobalPredictorConfigFile.compare("") != 0)
            {
                    assert(backfillingVariant != RUA_BACKFILLING); // this policy requires to use the resource usage modelling

                    printf("%s", ("Creating a EASY policy simulator with prediction and reservations = "+to_string(numberOfReservationsReq)+"\n").c_str());

                    Predictor* predictor;
                    PredDeadlineMissManager* pre;
                    PredDeadlineMissManager* post;
                    PredictorConfigurationFile* configPred;

                    //we give more priority the local predictor
                    if(PredictorConfigFileReq.compare("") != 0 )
                    {
                            configPred = new PredictorConfigurationFile(PredictorConfigFileReq);
                            predictor = CreatePredictor(configPred);
                            pre = CreateDeadlineManager(configPred->preDeadlineManager);
                            post = CreateDeadlineManager(configPred->postDeadlineManager);
                    }
                    else
                    {
                            predictor = GlobalPredictionService;
                            pre = GlobalPreDeadlineManager;
                            post = GlobalPostDeadlineManager;

                    }

                    policyRet = new EASYPredSchedulingPolicy(architectureConfig,log,startime,variantReq);
                    ((EASYPredSchedulingPolicy*)policyRet)->setPredictor(predictor);
                    predictor->setPreManager(pre);
                    predictor->setPostManager(post);

            }
            else
            {
                    printf("%s", ("Creating the EASY policy with no prediction and reservations = "+to_string(numberOfReservationsReq)+" \n").c_str());
                    policyRet = new EASYSchedulingPolicy(architectureConfig,log,startime,variantReq);
            }

            ((EASYSchedulingPolicy*)policyRet)->setPowerAware(powerAwareBackFilling);
            ((EASYSchedulingPolicy*)policyRet)->setNumberOfReservations(numberOfReservationsReq);
            ((EASYSchedulingPolicy*)policyRet)->setAllocateAllJobs(AllocateAllJobsInTheRT);
            ((EASYSchedulingPolicy*)policyRet)->setTryToBackfillAllways(tryToBackfillAllways);

            policyRet->simulateResourceUsage = SimulateResourceUsage;

            return policyRet;
    }
     */
    /**
     * Creates a deep policy based on the global parameters
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the DEEP created policy
     */
    /*
    SchedulingPolicy* SimulationConfiguration::CreateDEEPPolicy(ArchitectureConfiguration* architectureConfig, Log* log,double startime)
    {
            return CreateDEEPPolicy(architectureConfig,log,startime,this->DSCPmetricType,this->DSCPstatisticType,this->DSCPgenerateAllParaversRTables,this->DSCPparaverOutPutDir,this->DSCPparaverTrace);
    }
     */

    /**
     * Creates a ISIS Dispactcher policy based on the global variables
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the isis dispatcher policy
     */
    /*
    SchedulingPolicy* SimulationConfiguration::CreateISISDispatcherPolicy(Log* log,double startime)
    {
            ISISDispatcherPolicy* policy = new ISISDispatcherPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);

            //must be done in this order, due to setting the policies, requires to know previously the centers name
            policy->setISISDispatcherArchitectures(ISISDispatcherArchitectures);
            policy->setISISDispatcherCentersName(ISISDispatcherCentersName);
            policy->setISISDispatcherPolicies( ISISDispatcherPolicies);
            policy->setISISDispatcherParaverTracesName(ISISDispatcherParaverTracesName);  policy->setISISDispatcherMetric2Optimize(ISISDispatcherMetric2Optimize);
            policy->setISISDispatcherStatistic2Optimize(ISISDispatcherStatistic2Optimize);
            policy->setISISDispatcherEmulateCPUFactor(ISISDispatcherEmulateCPUFactor);
            policy->setAmountOfKnownCenters(amountOfKnownCenters);

            policy->setGlobalTime(startime);

            //we also set the log and
            policy->setLog(log);

            for(vector<SchedulingPolicy*>::iterator itpol = ISISDispatcherPolicies->begin();
                            itpol != ISISDispatcherPolicies->end();
                            ++itpol)
            {
                    SchedulingPolicy* centerPolicy = *itpol;
                    centerPolicy->setLog(log);
                    centerPolicy->setGlobalTime(startime);
            }

            return policy;

    }
     */
    /**
     * Creates a BRANKpolicy based on the global variables
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the Grid backfilling policy
     */
    /*
    SchedulingPolicy* SimulationConfiguration::CreateBRANKPolicy(Log* log,double startime)
    {
            BRANKPolicy* policy = new BRANKPolicy(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);

            //must be done in this order, due to setting the policies, requires to know previously the centers name
            policy->setBRANKPolicyArchitectures(BRANKPolicyArchitectures);
            policy->setBRANKPolicyCentersName(BRANKPolicyCentersName);
            policy->setBRANKPolicyPolicies( BRANKPolicyPolicies);
            policy->setBRANKPolicyParaverTracesName(BRANKPolicyParaverTracesName);
            policy->setBRANKPolicyEmulateCpuFactor(BRANKPolicyEmulateCpuFactor);

            policy->setGlobalTime(startime);

            //we also set the log and
            policy->setLog(log);

            for(vector<SchedulingPolicy*>::iterator itpol = BRANKPolicyPolicies->begin();
                            itpol != BRANKPolicyPolicies->end();
                            ++itpol)
            {
                    SchedulingPolicy* centerPolicy = *itpol;
                    centerPolicy->setLog(log);
                    centerPolicy->setGlobalTime(startime);
            }

            return policy;

    }
     */
    /**
     * Creates a BRANKpolicy based on the global variables
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the Grid backfilling policy
     */
    /*
    SchedulingPolicy* SimulationConfiguration::CreateMetaBrokeringSystem(Log* log,double startime)
    {
            SchedulingPolicy* policy = new MetaBrokeringSystem(log,this->GlobalPredictionService != NULL, this->GlobalPredictionService);
            ((MetaBrokeringSystem*)policy)->setVirtualOrganitzations(MBVirtualOrganitzations);
            ((MetaBrokeringSystem*)policy)->setVONames(VONames);

            return policy;
    }
     */
    /**
     * Creates a DeepSearch based on the global variables
     * @param log The logging engine
     * @param startime The global startime
     * @return A reference to the DeepSearch policy
     */
    /*
     SchedulingPolicy* SimulationConfiguration::CreateDEEPPolicy(ArchitectureConfiguration* architectureConfig, Log* log,double startime,metric_t DSCPmetricTypeReq,statistic_t DSCPstatisticTypeReq,bool DSCPgenerateAllParaversRTablesReq,string paraverOutPutDirReq,string paraverTraceReq)
    {
            SchedulingPolicy* policyRet  = NULL;

            printf("Creating a DEEP SEARCH policy simulator.. \n");
            DeepSearchCollisionPolicy * policyDeep = new DeepSearchCollisionPolicy(architectureConfig,log,startime,DSCPmetricTypeReq,DSCPstatisticTypeReq);

            if(DSCPgenerateAllParaversRTablesReq)
            {
                    policyDeep->setGenerateAllParaversRTables(true);
                    policyDeep->setParaverOutPutDir(paraverOutPutDirReq);
                    policyDeep->setParaverTrace(paraverTraceReq);
            }

            policyRet = (SchedulingPolicy*) policyDeep;

            return policyRet;

    }
     */

    /**
     * Creates the policy based on the global parameters
     * @param architectureConfig The architecture that will be used in the simulation
     * @param log The logging engine
     * @param startime The global startime
     * @return
     */
    SchedulingPolicy* SimulationConfiguration::CreatePolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime) {

        SchedulingPolicy* policyRet = NULL;

        switch (this->policy) {
            case FCFS:
                policyRet = CreateFCFSPolicy(architectureConfig, log, startime);
                break;
            case EASY:
                //policyRet = CreateEASYPolicy(architectureConfig,log,startime);
                break;
            case DEEP:
                //policyRet = CreateDEEPPolicy(architectureConfig,log,startime);
                break;
            case ISIS_DISPATCHER:
                //OK- Gridbackfilling implementation got lost in the hyperspace. It's very likely that version we had accidentally merged or remove gridbackfilling class.
                //TODO: Figure out where the code is. Luckily as of now we do have git repo.
            case GRID_BACKFILLING:
                //policyRet = CreateISISDispatcherPolicy(log,startime);
                break;
            case BRANK_POLICY:
                //policyRet = CreateBRANKPolicy(log,startime);
                break;
            case META_BROKERING_SYSTEM:
                //policyRet = CreateMetaBrokeringSystem(log,startime);
                break;
            case SLURM:
                // This doesn't actually matter as it will never get used. We don't use the Policy in the sysOrch, only the one in the partitions.
                // BUT, the partitions copy the type of this policy and create theirs based upon it.
                policyRet = CreateSLURMPolicy(architectureConfig, log, startime);
                break; 
            default:
                assert(false);
                break;
        }

        //here we set all the common fields for the policy that are already known by the simulator
        policyRet->setEmulateCPUFactor(this->EmulateCPUFactor);
        policyRet->setRS_policyUsed(this->ResourceSelectionPolicy);
        policyRet->setMaxAllowedRuntime(this->maxAllowedRuntime);
        policyRet->setCostAllocationUnit(this->CostAllocationUnit);
        policyRet->setAllocationUnitHour(this->AllocationUnitHour);


        // take account compute Enery, for all scheduling jobs disciplines
        if (this->computeEnergy) {
            // TODO: architecture config file has a freq and voltage description ?
            assert(architectureConfig->getFrequencies().size() > 0 && architectureConfig->getFrequencies().size() == architectureConfig->getVoltages().size());

            PowerConsumption * powerinf = new PowerConsumption(architectureConfig->getActivity(), architectureConfig->getPortion());
            powerinf->setGears(architectureConfig->getFrequencies(), architectureConfig->getVoltages());
            policyRet->initializationPower(powerinf);
        }


        return policyRet;
    }

    /**
     * Creates the cvs job dumper class if it was indicated in the configuration file
     * @return A reference to the class
     */
    CSVJobsInfoConverter* SimulationConfiguration::createJobCSVDumper() {
        CSVJobsInfoConverter* JobCSVDumper = NULL;
        //here we also create the cvs dump file for the jobs in case it was specified
        if (this->JobsSimPerformanceCSVFile.compare("") != 0) {
            JobCSVDumper = new CSVJobsInfoConverter(this->JobsSimPerformanceCSVFile, log);
        }

        return JobCSVDumper;
    }

    /**
     * Creates the cvs job dumper class if it was indicated in the configuration file
     * @return A reference to the class
     */
    CSVPolicyInfoConverter* SimulationConfiguration::createPolicyCSVDumper() {
        CSVPolicyInfoConverter* PolicyCSVDumper = NULL;
        //here we also create the cvs dump file for the jobs in case it was specified
        if (this->JobsSimPerformanceCSVFile.compare("") != 0) {
            PolicyCSVDumper = new CSVPolicyInfoConverter(this->PolicySimPerformanceCSVFile, log);
        }

        return PolicyCSVDumper;
    }

    /**
     * Given a set of strings containing the name for the job attributes the function will return the native type associated to each of the types
     * @param AttributeTypes The vector containing the string with the attributes names
     * @return A reference to the vector containing the vector types for the attributes
     */
    vector<native_t>* SimulationConfiguration::GetAttributeTypes(vector<string>* AttributeTypes) {
        vector<native_t>* types = new vector<native_t>();

        for (vector<string>::iterator it = AttributeTypes->begin(); it != AttributeTypes->end(); ++it) {
            string att = *it;
            map<string, native_t>::iterator itAtType = this->AttributeJobType.find(att);
            assert(itAtType != this->AttributeJobType.end());
            types->push_back(itAtType->second);
        }

        return types;
    }

    /**
     * Function that will create all the CSV  R analyzers that analyze the CSVJobs files
     * @param csvfile The csv file that have to be analyzed
     * @return The set of analyzers for the CSVJobs files
     */
    vector<RScript*>* SimulationConfiguration::CreateJobCSVRAnalyzers(CSVJobsInfoConverter* csvfile, ArchitectureConfiguration* arch) {
        vector<RScript*> * analyzers = new vector<RScript*>();

        for (vector<r_scripts_type>::iterator it = this->RAnalyzers.begin(); it != this->RAnalyzers.end(); ++it) {
            r_scripts_type type = *it;

            if (type == JOB_CSV_R_ANALYZER_BASIC) {
                map<r_scripts_type, string>::iterator has = this->simulatorConfiguration->RScripts.find(type);

                assert(has != this->simulatorConfiguration->RScripts.end());

                string scriptPath = has->second;

                JobCSVRScriptAnaliser* analyzer = new JobCSVRScriptAnaliser(csvfile->getFilePath(), scriptPath, this->outPutDir, arch, this->log, this->simulatorConfiguration->TemporaryDirectory, this->simulatorConfiguration->RBinaryPath, this->simulatorConfiguration->RScriptsBaseDir);

                analyzers->push_back((RScript*) analyzer);
            }
        }

        return analyzers;

    }

}
