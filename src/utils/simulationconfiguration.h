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
#ifndef SIMULATORSIMULATIONCONFIGURATION_H
#define SIMULATORSIMULATIONCONFIGURATION_H

/* Auxiliary Stuff includes */
#include <scheduling/swftracefile.h>
#include <scheduling/swftraceextended.h>
#include <utils/pbsInput.h>
#include <utils/nntfInput.h>
//#include <scheduling/swfgridtrace.h>
#include <utils/log.h>
#include <utils/paravertrace.h>
#include <utils/otf2trace.h>
#include <utils/utilities.h>

/* Simulation stuff includes */
#include <scheduling/simulation.h>
#include <scheduling/simulatorcharacteristics.h>

/* simulation statistics and output files stuff */
#include <statistics/statistics.h>
#include <utils/cvsjobsinfoconverter.h>
#include <utils/jobcsvrscriptanaliser.h>
#include <utils/csvpolicyinfoconverter.h>
#include <utils/rscript.h>

/*Architecture includes */


/*Parsing and configuration includes */

//#include <utils/arg_parser.h>
#include <utils/configurationfile.h>
#include <statistics/statisticsconfigurationfile.h>
//#include <utils/predictorconfigurationfile.h>

/*The policies includes */
#include <scheduling/fcfsschedulingpolicy.h>
#include <scheduling/slurmschedulingpolicy.h>


/* prediction stuff */
//#include <predictors/predictor.h>
//#include <predictors/userruntimeestimatepred.h>
//#include <predictors/preddeadlinemissmanager.h>
//#include <predictors/preddeadlinemissmanagerexp.h>
//#include <predictors/preddeadlinemissmanagergradual.h>
#include <utils/configurationfile.h>
//#include <predictors/historicalifier.h>

/* libxml2 stuff */
#include <libxml/tree.h>
#include <libxml/parser.h>

/* include utils stuff */
#include <utils/utilities.h>
#include <sstream>
#include <cassert>


using namespace Simulator;
using namespace Utils;
//using namespace Datamining;

namespace Utils {

    /**
     * @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
     */

    /** Class that provides the functionalities for accessing to the information of a simulation configuration file*/
    class SimulationConfiguration {
    public:
        SimulationConfiguration(string ConfigFile, Log* log, ConfigurationFile* simulatorConfiguration);
        ~SimulationConfiguration();
        SimulationConfiguration();
        void import(SimulationConfiguration* ConfigFile);
        void import(ConfigurationFile* config);

        /* auxiliar function for creating complex required objects for the simulation */
//        PredDeadlineMissManager* CreateDeadlineManager(DeadlineMissManager_t type);
//        Predictor* CreatePredictor(PredictorConfigurationFile* configPred);

        ArchitectureConfiguration* CreateArchitecture(Log*, double startime);

        TraceFile* CreateWorkload(Log* log);
        TraceFile* CreateWorkload(Log* log, workload_type_t workloadReq, string workloadPathReq, string jobRequirementsFileReq);

        CSVJobsInfoConverter* createJobCSVDumper();
        CSVPolicyInfoConverter* createPolicyCSVDumper();

        SchedulingPolicy* CreatePolicy(ArchitectureConfiguration*, Log* log, double startime);
        SchedulingPolicy* CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime);
        SchedulingPolicy* CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime, bool SimulateResourceUsage, int numberOfReservations);
        SchedulingPolicy* CreateFCFSPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime, bool SimulateResourceUsage, int numberOfReservations, bool MalleableExpand, bool MalleableShrink);
        SchedulingPolicy* CreateSLURMPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime);
        SchedulingPolicy* CreateSLURMPolicy(ArchitectureConfiguration* architectureConfig, Log* log, double startime, bool SimulateResourceUsage, int numberOfReservations, bool MalleableExpand, bool MalleableShrink);
   
        vector<RScript*>* CreateJobCSVRAnalyzers(CSVJobsInfoConverter* csvfile, ArchitectureConfiguration* arch);

        //get policy info
        policy_type_t getPolicyType();
        RS_policy_type_t getResourceSelectionPolicyType();
        architecture_type_t getArchitectureType();

    private:
        //xml specific stuff 
        xmlDocPtr doc; /**<  XML Document root pointing to the simulatorConfiguration.xml file */

        //stuff for getting information concerning the jobs
        //EASYSchedulingPolicy* extractEASYFields(bool here,xmlNodePtr easyFields,ArchitectureConfiguration* architectureConfig);
        //ISISDispatcherPolicy* extractISISDispatcherFields(bool here,xmlNodePtr isisFields, ArchitectureConfiguration* architectureConfig);
        //BRANKPolicy* extractBRANKPolicyFields(bool here,xmlNodePtr brankFields,ArchitectureConfiguration* architectureConfig);
        FCFSSchedulingPolicy* extractFCFSFields(bool here, xmlNodePtr fcfsFields, ArchitectureConfiguration* architectureConfig);
        SLURMSchedulingPolicy* extractSLURMFields(bool here, xmlNodePtr fcfsFields, ArchitectureConfiguration* architectureConfig);
        //MetaBrokeringSystem* extractMetaBrokeringSystemFields(bool here,xmlNodePtr MetaBrokeringSystemFields,ArchitectureConfiguration* architectureConfig);
        //DeepSearchCollisionPolicy* extractDEEPFields(bool here,xmlNodePtr, ArchitectureConfiguration* architectureConfig);

        //stuff for parsing types
        policy_type_t getPolicyType(string spolicy);
        RS_policy_type_t getResourceSelectionPolicyType(string spolicy);
        architecture_type_t getArchitectureType(string arch);
        statistic_t getStatisticType(string estimator);
        metric_t getMetricType(string variable);
        vector<native_t>* GetAttributeTypes(vector<string>* AttributeTypes);

        ConfigurationFile* simulatorConfiguration; /**< A reference to the simulator configuration */

    public:
        /*we make them public since it won't be common to acces or modify it .. only in the inital phase*/
        string ConfigFile; /**< The file path for the simulation configuration file */
        string StatisticsConfigFile; /**< */

        /* general parameters common to all scheduling policies ..*/
        policy_type_t policy; /**< The policy that will be simulated */
        workload_type_t workload; /**< The worload type that will used as an input */
        string workloadPath; /**< The path where the workload is stored*/
        string defworkloadPath; /**< The path where the workload is stored*/
        // string paraverOutPutDir; /**< The directory path where the parver output files will be stored*/ // No, just put in output dir, no need to be overcomplicated.
        string outputTraceFilename; /**< The paraver trace file name */
        string schedCoeffDataFilename; // Where we write the data to
        string schedCoeffCountFilename; // Where we write the counts to
        int startJobTrace; /**<indicates the initial number of job where the paravertrace trace has to be generated */
        int endJobTrace; /**<indicates the last number of job where the paravertrace trace has to be generated */
        string architectureFile; /**< The file containing the architecture definition */
        architecture_type_t architectureType; /**< The type of the architecture used in the simulation. */
        string outPutDir; /**< The directory path where the output files will be stored*/
        string ErrorFilePath; /**< The pathfile for the error file */
        string DebugFilePath; /**< The pathfile for the debug file */
        int DebugLevel; /**< The debug level used in the simulation */
        int DebugFrom; /**< From what line we will start dumping files */
        string GlobalStatisticsOutputFile; /**< The path for the output simulation statistics file */
        string JobsSimPerformanceCSVFile; /**< The path for the output cvs file that will contain the information for the simulated jobs  */
        string PolicySimPerformanceCSVFile; /**< The path for the output cvs file that will contain the ifnroamtion for the policies*/
        string SWFOutputFile; /**< The path for the output trace in SWF format.*/

        double collectStatisticsInterval; /**< The interval between to statistical collection events  */
        string jobRequirementsFile; /**< The filepath for the job requirements files */
        double EmulateCPUFactor; /**<see scheduling policy for its definition.*/
        double ArrivalFactor; /**<indicates if the submitt time has to be multiplied by a factor*/
        double JobsToLoad; /**<by default, -1, indicates that all the jobs must be loaded in the simulation, otherwise indicates the number of jobs to be loaded*/
        double LoadFromJob; /**<by default 1 indicates from which job it has to  be loaded                     */
        double LoadTillJob; /**<by default -1 indicates that there is no limit */
        double LoadFactor; /**<indicates the load (obtained due to the jobs arrival) have to be obtained, usefull for test how a given configuration response on a given load. By default is the once configured in the original trace.*/
        double maxAllowedRuntime; /**< Indicates the maximum amount of runtime allowed runtime for the submitted jobs, it mainly models the job queue of the local resource, -1 means no limit*/
        double CostAllocationUnit; /**< Indicates the cost per allocation unit */
        double AllocationUnitHour; /**< Indicates the number of allocations unit per hour that the execution of one single processor requires */

        double SlowdownBound; /**< Indicated the value of the bound to calculate the bounded slowdown.*/

        bool showSimulationProgress; /**<if true the the simulation progress will be shown in the STD OUT by defaul false*/

        RS_policy_type_t ResourceSelectionPolicy; /**<the resource selection policy that the policy will use for select where to allocate the jobs */

        //indicate for some fields if loaded or not
        bool architectureLoaded; /**< Indicates if the architecture file has been loaded correctly*/
        bool policyLoaded; /**< Indicates if the policy  has been loaded correctly*/
        bool workloadLoaded; /**< Indicates if the workload has been loaded correctly*/
        bool DSCPmetricTypeLoaded; /**< Indicates if the deepsearchcollisionpolicy metric has been loaded correctly*/
        bool DSCPstatisticTypeLoaded; /**< Indicates if the deepsearchcollisionpolicy statistics has has been loaded correctly*/
        bool fileLoaded; /**< Indicates if the file has been loaded correctly*/
        bool generateOutputTrace; /**<indicates if an output trace should be generated*/

        bool generateSWF; /**<Indicates if the swf trace of the execution must be generated.*/
        bool simPartitions; /**<Indicates if partitions must be simulated. */
        bool reserveFullNode; /**<Indicates if a job reserves only full nodes or not. If true, a job can only reserve multiples of a node. If false, a job can reserve a portion of a node, i.e., jobs can share resources within a node.*/

        /*specific parameters for each policy .. if this field list grows a lot we may create subclasses */
        metric_t DSCPmetricType; /**< The metric to optmize in case that the deepsearchcollisionpolicy is used (@see deepsearchcollisionpolicy)*/
        statistic_t DSCPstatisticType; /**< The statistics to use in the metric computation in case that the deepsearchcollisionpolicy is used (@see deepsearchcollisionpolicy)*/
        bool DSCPgenerateAllParaversRTables; //indicates if all the intermediate paraver trace files must be generated .. this is basically for debug purposes .. be aware becasue for each job allocation a paraver trace file will be generated !
        string DSCPparaverOutPutDir; /**< The output dir where all the intermediate dumps of the reservation tables will be dump in case that the deepsearchcollisionpolicy is used, and the dummping is required*/
        string DSCPparaverTrace; /**< The output dir where all the intermediate paraver traces of the reservation tables will be dump in case that the deepsearchcollisionpolicy is used, and the dummping is required */

        //regarding the EASY policy
        int numberOfReservations; /**< The number of reservations for the policy, in case EASY is specified*/
        bool SimulateResourceUsage; /**< Indicates if the job collisions and resource usage have to be simulation  */

        //regarding malleability
        bool malleableExpand; /**< If malleability expand should be considered */
        bool malleableShrink; /**< If malleability shrink should be considered */


        map<string, native_t> AttributeJobType; /**< Cotains the mapping of each of the job attribute with its type, to be used for the datamining models creation */

        string GlobalPredictorConfigFile; /**< Contains the global predictor configuration file that can be used by all the simulation entities. In the case of local policies, if a specific configuration is specified it will be used rather than the global */

        vector<r_scripts_type> RAnalyzers; /**< The set of analyzers that have be used once the simulation will finish */

        string jobMonetaryRequirementsFile; /**< The file that contains the monetary requirements for the jobs */

        //regarding a meta-brokering system
        vector<SchedulingPolicy*>* MBVirtualOrganitzations; /**< The definition of the different virtual organitzations */
        vector<string> VONames; /**< The name of the different virtual organitzations */


        //initialize all the variables  
        void init();

        Log* log;
        bool computeEnergy;
        bool powerAwareBackfilling;

        int SLURMWQsize;
        int SLURMSkipTime;
        int SLURMPmem;
        
    };

}

#endif
