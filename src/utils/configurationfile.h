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
#ifndef UTILSCONFIGURATIONFILE_H
#define UTILSCONFIGURATIONFILE_H

#include <utils/log.h>

/* libxml2 stuff */
#include <libxml/tree.h>
#include <libxml/parser.h>

/* include utils stuff */
#include <utils/utilities.h>
#include<sstream>
#include <cassert>
#include <map>

using namespace std;
using std::map;

namespace Utils {

    /** This enum contains all the available policies in the simulator */
    enum policy_type_t {
        FCFS = 0,
        EASY,
        DEEP,
        GRID_BACKFILLING,
        ISIS_DISPATCHER,
        BRANK_POLICY,
        META_BROKERING_SYSTEM,
        SLURM,
        OTHER_POLICY,
    };

/** This enum contains all the different workload formats available in the simulator */
    enum workload_type_t {
        SWF = 0,
        SWFExtReqs,
        SWFExtCosts,
        SWFGrid,
        PBSPro,
        NNTF,
        OTHER_WORKLOAD,
    };

    /** This enum contains all the different architecture configurations that are available in the simulator **/
    enum architecture_type_t {
        HPC = 0,
        OTHER_TYPE,
    };

    /** This enum indicates the different cvs converter files available in the simulator, currently only one*/
    enum csv_converter_t {
        CSV_JOBS_INFO_CONVERTER = 0,
        DEFAULT_CSV_CONVERTER,
    };

    /** This enum indicates the different r analyzer scripts available to the simulator , each r script is associated to a given output files of the simulation, for instance the JOB_CSV_R_ANALYZER_BASIC analises the csv job files providing a general description fro the job csv file */
    enum r_scripts_type {
        JOB_CSV_R_ANALYZER_BASIC = 0,
        DEFAULT_CSV_R_ANALYER,
    };

    /** This enums indicates the type of fields available in the cvs output  */
    enum file_field_t {
        DOUBLE_FIELD = 0,
        STRING_FIELD,
        INT_FIELD,
    };

    /** This enum indicates the available formats of the fields for the output cvs file */
    enum file_field_format_t {
        NODECIMAL = 0,
        CIENTIFIC,
        DEFAULT_FORMAT,
    };


    /**
     * @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
     */

    /**
     * This class implements the load of the configuration file for the simulator. In this case we make the variables public since it won't be common to acces or modify it .. only in the inital phase
     */
    class ConfigurationFile {
    public:
        ConfigurationFile(Utils::Log* log, string ConfigFile);
        ~ConfigurationFile();

        void import(ConfigurationFile* ConfigFile);

    public:

        string ConfigFile; /**< The filepath of the configuration file */
        string DefaultWorkloadPath; /**< The default workload path where the default files are stored */
        string DefaultArchitectureConfigurationFile; /**< The default architecutre configuration file  */
        string DefaultPolicy; /**< The default policy to be used */
        string DefaultParaverOutputDir; /**< The default paraver output file */
        string DefaultParaverTraceName; /**< The default paraver trace file name */
        string ErrorFilePath; /**< The default file where the loggin engine will store the error messages */
        string DebugFilePath; /**< The default file where the loggin engine will store the debug messages */
        string GlobalStatisticsOutputFile;
        int DebugFrom; /**< Indicates from what line the logger will start dumping stuff */
        int DebugLevel; /**< The default debug level for the simulation */
        string DefaultOutPutDir; /**< The default output dir for the simulations */

        /* be aware !! if changes on this class check that the class Simulat*/

        policy_type_t policy; /**<  The policy for the simulation  */
        workload_type_t workload; /**< The workload type for the simulation */
        string workloadPath; /**< The workload path for the simulation */
        string jobRequirementsFile; /**< The job requirements files for the simulation */
        string paraverOutPutDir; /**< The paraver output file */
        string paraverTrace; /* <old default trace filename> */
        string outputTraceFilename; /**< The default trace file name */
        string architectureFile; /**< The  architecutre configuration file  */
        architecture_type_t architectureType; /**< The architecture type used in the simulation. */
        string statisticsConfigFile; /**< The statistics configuration file */
        string simulationconfigFile; /**< The simulation configuration file */

        string JavaBinaryPath; /**< The filepath for the java file */
        string JavaClassPath; /**< The class path for the java file */
        string WekaJarPath; /**< The filepaht for the weka.jar file */

        string TemporaryDirectory; /**< The directory where all the temporary files will be dumped , for instance analyzer scripts etc . */

        string RBinaryPath; /**< The file path for the R binary file  */
        map<r_scripts_type, string> RScripts; /**< The scripts contained in this vector will be applied to the CVS files types  */
        string RScriptsBaseDir; /**< The directory where the R auxliar scripts are located */

        r_scripts_type getAnalyzerType(string r_analyzer);

        int JobsToSimulate; /**< This is an integer value that indicates the amount of jobs that the simulator has to load in the simulation stream, this option is desigend for those situations in which the user has a set of different simulation configuration files with different JobsToLoad and he/she wants to carry out all the punch of experiments using the same amount of loaded jobs  */

        double LessConsumeAlternativeThreshold; /**< Indicates the alternative threashold that has to be used in the find less consume threshold function  when the default one can not be satisfied .. in this situations it use to be more relaxed*/

    private:

        //xml specific stuff - 
        xmlDocPtr doc; /**<  XML Document root pointing to the simulatorConfiguration.xml file */
        Log* log;

    };

}

#endif

