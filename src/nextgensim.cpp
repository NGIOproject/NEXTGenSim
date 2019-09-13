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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <ctime>


#include <utils/simulationconfiguration.h>
#include <scheduling/systemorchestrator.h>
#include <utils/architectureconfiguration.h>
#include <scheduling/schedulingpolicy.h>
#include <scheduling/resourceschedulingpolicy.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "scheduling/partition.h"
#include <utils/otf2trace.h>
#include <utils/trace.h>

#include <getopt.h>

const char * invocation_name = 0;
const char * const program_name = "NEXTGenSim";
const char * const program_version = "2.0";
const char * const program_year = "2019";

void show_help() throw () {
    cout << program_name << " - the NEXTGenIO Workload Simulator." << endl;
    cout << "See the file README.md for more information.\n" << endl;
    cout << "Usage: " << invocation_name << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  -h, --help                              display this help and exit" << endl;
    cout << "  -s<file>, --simconfig=<file>            use <file> as simulation configuration file." << endl;
    cout << "  -d[debuglevel], --debug[=<debuglevel>]  enable debug mode and optionally specify <debuglevel>." << endl;
    //cout << "  -r<path>, --root=<path>                 specify <path> as location of root NEXTGenSim directory." << endl;
    cout << "" << endl;
    cout << "Environment Variables:" << endl;
    //cout << "NGSIM_ROOT : Alt method to specify root directory for NEXTGenSim.\n             The -r option takes preference over this." << endl;
}

//void show_version() throw () {
//    std::printf("%s version %s\n", program_name, program_version);
//    std::printf("Copyright (C) %s Francesc Guim Bernat .\n", program_year);
//    std::printf("This program is Barcelona Supercomputing Center and Francesc Guim Software software; you may redistribute it under the terms of\n");
//    std::printf("the GNU General Public License.  This program has absolutely no warranty.\n");
//}

void show_error(const char * msg, const int errcode = 0, const bool help = false) throw () {
    if (msg && msg[0] != 0) {
        std::fprintf(stderr, "%s: %s", program_name, msg);
        if (errcode > 0) std::fprintf(stderr, ": %s", strerror(errcode));
        std::fprintf(stderr, "\n");
    }
    if (help && invocation_name && invocation_name[0] != 0)
        std::fprintf(stderr, "Try `%s --help' for more information.\n", invocation_name);
}

void internal_error(const char * msg) throw () {
    char buf[80];
    std::snprintf(buf, sizeof ( buf), "internal error: %s.\n", msg);
    show_error(buf);
    exit(3);
}

/*
const char * optname( const int code, const Arg_parser::Option options[] ) throw()
{
  static char buf[2] = '?';
  if( code != 0 )
    for( int i = 0; options[i].code; ++i )
      if( code == options[i].code )
        if( options[i].name ) return options[i].name;
            else break;
  if( code > 0 && code < 256 ) buf[0] = code;
  else buf[0] = '?';
  return buf;
}
 */

//void show_simulation_policies() throw () {
//    std::printf("The available scheduling policies that are being simulated are:\n");
//    std::printf("First Come First Serve - ID : FCFS\n");
//    std::printf("EASY Backfilling - ID: EASY");
//    std::printf("DeepSearch Policy - ID DEEP");
//}
//
//void show_simulation_workloads() throw () {
//    std::printf("The workloads formats that are currently recognized by the simulator are:\n");
//    std::printf("Standard Workload Format - ID : SWF\n");
//    std::printf("Standard Workload Format EXtended (SWF+job requirements) ID: SWFExt ");
//}
//
//void show_simulation_architectures() throw () {
//    std::printf("The architecture models that are currently recognized by the simulator are:\n");
//    std::printf("HpcSystem I (Blades/Nodes/Cpus)- ID : MN1\n");
//}





/* WE KEEP THIS AS A GLOBAL FOR THE SIGNAL TREATMENT */
Simulator::Simulation * simulator = NULL;

void terminate(int param) {
    std::printf("Terminating simulation a sigkill has been received...\n");
    std::printf("All the statistic files and traces are being generated..\n");
    simulator->setStopSimulation(true);
}

void showStats(int param) {
    simulator->showCurrentStatistics();
    signal(SIGINT, showStats);
}

int main(int argc, char *argv[]) {


    /********************************************************************************************************************
     ***************************************** PARSING THE COMMAND LINE **************************************************
     *********************************************************************************************************************/
    //Variables for the command line parsing


    bool debugEnabled = false;
    int debugLevel = -1;
    invocation_name = argv[0];
    bool generateOutputTrace = false;
    double startime = 0;
    std::string ngsim_root;
    std::string simulationconfigurationfile;


    cout << "      _   _  _______   _______ _____            _____ _           " << endl;
    cout << "     | \\ | ||  ___\\ \\ / /_   _|  __ \\          /  ___(_)          " << endl;
    cout << "     |  \\| || |__  \\ V /  | | | |  \\/ ___ _ __ \\ `--. _ _ __ ___  " << endl;
    cout << "     | . ` ||  __| /   \\  | | | | __ / _ \\ '_ \\ `--. \\ | '_ ` _ \\ " << endl;
    cout << "     | |\\  || |___/ /^\\ \\ | | | |_\\ \\  __/ | | /\\__/ / | | | | | |" << endl;
    cout << "     \\_| \\_/\\____/\\/   \\/ \\_/  \\____/\\___|_| |_\\____/|_|_| |_| |_|" << endl;
    cout << " " << endl;

    /*
     * Changing to getopt_long for less code
     */
    while (1) {


        static struct option long_options[] = {
            {"debug", optional_argument, NULL, 'd'},
            {"help", no_argument, NULL, 'h'},
            {"simconfig", required_argument, NULL, 's'},
//            {"root", required_argument, NULL, 'r'},
            {0, 0, 0}
        };

        int option_index = 0;
        int c = 0;
        c = getopt_long(argc, argv, "hs:d::", long_options, &option_index);

        if (c == -1) {
            break;
        }


        /*
         * Things we really do need options for...
         * h (--help)  - a help with options info
         * d (--debug) - debug PLUS setting the debug level
         * s (--simconfig) - location of a simulation configuration file
         * r (--root) - NGSIM_ROOT rather than define by an env var
         * 
         * Things we might want options for in the future...
         * Type of output file to generate (paraver/otf2)
         * Whether or not to do an output file
         * Logfile directory (old parser did this, but too many options)
         * Output file location
         * 
         */

        switch (c) {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                break;

            case 'd':
                debugEnabled = true;
                //                cout << "Enabling debug at level ";
                if (optarg != NULL) {
                    debugLevel = atoi(optarg);
                    //                    cout << debugLevel << endl;
                }
                //                else
                //                    cout << "1 (default)." << endl;
                break;

            case 'h':
                show_help();
                return EXIT_SUCCESS;
                break;

//            case 'r':
//                if (optarg == NULL) {
//                    cout << "FAILURE IN NGSIM_ROOT parsing" << endl;
//                } else {
//                    ngsim_root = optarg;
//                }
//                break;

            case 's':
                /* Simulation configuration to run */
                if (optarg == NULL) {
                    cout << "Failure to specify a config." << endl;
                } else {
                    simulationconfigurationfile = optarg;
                }
                break;

            default:
                cout << "Unrecognised Options" << endl;
                show_help();
                return EXIT_FAILURE;

        }
    }


    if (simulationconfigurationfile.empty()) {
        cout << "You must specify a simConfig file for this program. Cannot proceed." << endl;
        return EXIT_FAILURE;
    }



    /********************************************************************************************************************
     ***************************************** END PARSING COMMAND LINE **************************************************
     *********************************************************************************************************************/

    // This is a fake(!) time to pass to the log constructors. It will always be 0 as all events before do_simulation, happen at t=0 from a logging point of view
    // In these pre-events, the log index is more useful; once the simulation starts, the globalTime is more useful.
    uint64_t faketime = 0;

    Utils::Log * log = new Log("",
            "",
            (debugLevel != -1) ? debugLevel : 1,
            1,
            debugEnabled,
            &faketime,
            true);


    /*
     * This is mad, but kinda makes sense
     * The simulaTOR configuration contains default values which are overriden by the simulaTION configuration
     * 
     * Also, the nomenclature is confusing:
     * simulation configuration should be called scenario
     * simulator configuration should be called configuration
     */

    //stuff will be overloaded by this one 
    Utils::SimulationConfiguration* simConfig = NULL;

    //    if (configuration->simulationconfigFile != ""){
    if (simulationconfigurationfile != "") {
        simConfig = new SimulationConfiguration(simulationconfigurationfile, log, NULL);
        cout << "Scenario Config:\t" + simulationconfigurationfile << endl;
    } else {
        simConfig = new SimulationConfiguration();
    }

   log->beginFile(simConfig->ErrorFilePath, simConfig->DebugFilePath);


    simConfig->log = log;

    //Now that the we have parsed all the parameters and the configuration file has been loaded we 
    //carry out all the simulation stuff related to the simulation
    //also the parameters provided by command line have more priority to the once provided by xml


    if (generateOutputTrace)
        //in case has pushed it from command line .. we enforce it independent of the simulationconfiguration file
        simConfig->generateOutputTrace = true;


    log->debug("Loading the simConfig file of the architecture.", 1);



    /* now the architecture */
    Utils::ArchitectureConfiguration* architectureConfig = simConfig->CreateArchitecture(log, startime);
    log->debug("Architecture created.", 1);
    Simulator::SystemOrchestrator *sysOrch = architectureConfig->getSystemOrchestrator();
    sysOrch->setSimulatePartitions(simConfig->simPartitions);
    sysOrch->setGlobalTime(startime);
    sysOrch->setOutputTrace(simConfig->generateOutputTrace);
    sysOrch->setsimuConfig(simConfig);

    //    architecture_type_t archType = architectureConfig->getArchitectureType();
    uint32_t numPartitions = sysOrch->getNumberPartitions();

    log->debug("Creating policy.", 1);
    Simulator::SchedulingPolicy *policy = simConfig->CreatePolicy(architectureConfig, log, startime);
    sysOrch->setPolicy((ResourceSchedulingPolicy*) policy);
    sysOrch->setReserveFullNode(simConfig->reserveFullNode);

    log->debug("The sysOrch has paraver set to " + itos(sysOrch->getOutputTrace()), 1);
    //if 
    log->debug("The architecture file has been loaded correctly !!! The simConfig file is " + simConfig->architectureFile);

    //First the workload 
    log->debug("Using the workload " + simConfig->workloadPath, 1);

    TraceFile* workload = simConfig->CreateWorkload(log);

    log->debug("Creating the simulation simConfig container..");

    log->debug("Loading the statistic configuration file " + simConfig->StatisticsConfigFile);

    Utils::StatisticsConfigurationFile* statisticsConfig = new StatisticsConfigurationFile(simConfig->StatisticsConfigFile, log);

    log->debug("Creating the statistical analysis instance for the simulation");

    Simulator::SimStatistics* statistics = new SimStatistics(log);
    statistics->setJobMetricsDefinition(statisticsConfig->getJobMetrics());
    statistics->setPolicyMetricsDefinition(statisticsConfig->getPolicyMetrics());

    Utils::CSVJobsInfoConverter* jobsSimulationCSV = simConfig->createJobCSVDumper();

    if (jobsSimulationCSV != NULL) {
        log->debug("Creating the CSV file " + simConfig->JobsSimPerformanceCSVFile + " for dumping the jobs simulation performance variables", 2);
    }

    Utils::CSVPolicyInfoConverter* policySimulationCSV = simConfig->createPolicyCSVDumper();

    if (policySimulationCSV != NULL)
        log->debug("Creating the CSV file " + simConfig->JobsSimPerformanceCSVFile + " for dumping the policy simulation performance variables", 2);

    statistics->setGlobalStatisticsOutputFile(simConfig->GlobalStatisticsOutputFile);
    statistics->setsimOutputPath(simConfig->outPutDir);
    statistics->setSchedDataFile(simConfig->schedCoeffDataFilename);
    statistics->setSchedCountFile(simConfig->schedCoeffCountFilename);

    log->debug("Creating the container for the simulator characteristics", 1);

    Simulator::SimulatorCharacteristics * chars = new SimulatorCharacteristics();
    //Load container with objects created
    chars->setWorkload(workload);
    chars->setArchConfiguration(architectureConfig);
    chars->setSimStatistics(statistics);


    log->debug("Creating the simulator...");

    simulator = new Simulation(chars, log, sysOrch);
    log->setglobalTimeref(simulator->getGlobalTimeRef());


    simulator->setCollectStatisticsInterval(simConfig->collectStatisticsInterval);
    simulator->setArrivalFactor(simConfig->ArrivalFactor);
    simulator->setJobsSimulationPerformance(jobsSimulationCSV);
    simulator->setPolicySimulationPerformance(policySimulationCSV);
    simulator->setShowSimulationProgress(simConfig->showSimulationProgress);
    simulator->setSlowdownBound(simConfig->SlowdownBound);

    sysOrch->setSim(simulator);

    if (jobsSimulationCSV != NULL)
        jobsSimulationCSV->setPolicy(NULL);


    Utils::trace* outputTrace = 0;
    if (simConfig->generateOutputTrace) {
        /*
         * Invocation test
         */
        outputTrace = new otf2trace();
        outputTrace->setLog(log);

        /* For the test case, this should only be called once. */
        for (uint32_t ii = 0; ii < numPartitions; ii++) {
            Partition* p = sysOrch->getPartition(ii);
            ResourceSchedulingPolicy * rsp = p->getSchedulingPolicy();
            rsp->setOutputTrace(outputTrace);
            log->debug("Prior to calling the init method.", 2);
            log->debug(simConfig->outputTraceFilename, 1);
            outputTrace->init(simConfig->outputTraceFilename, simConfig->outPutDir, p, workload->getNumberOfJobs());
        }

        log->debug("Correctly created a new otf2trace object");


        //        trace* t = new trace();
        //        delete t;
    } else {
        log->debug("Paraver trace generation is disabled..");
    }


    log->debug("Setting the SIGNAL function");
    signal(SIGUSR1, terminate);
    signal(SIGINT, showStats);

    pid_t pidsimulation = getpid();
    std::cout << "The pid for the current simulation is " + itos(pidsimulation) << endl;

    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    std::cout << "Starting Simulation run @ " << std::ctime(&start_time) << endl;

    log->debug("Initializing the simulation", 1);
    simulator->initSimulation();

    log->debug("Doing the simulation", 1);
    simulator->doSimulation();

    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "Ending Simulation run @ " << std::ctime(&end_time) << endl;
    log->debug("Simulation finished.", 1);


    if (simConfig->generateOutputTrace) {
        outputTrace->simFinished((uint64_t) simulator->getglobalTime());
    }

    log->debug("Generating the R Analyzer scripts and launching the analysis");
    log->debug("The Job CSV Analyzers ...");

    if (jobsSimulationCSV != NULL) {
        vector<RScript*>* analyzers = simConfig->CreateJobCSVRAnalyzers(simulator->getJobsSimulationPerformance(), architectureConfig);

        for (vector<RScript*>::iterator it = analyzers->begin(); it != analyzers->end(); ++it) {
            RScript* analyzer = *it;
            analyzer->analyze();
        }
    }

    return EXIT_SUCCESS;
}


