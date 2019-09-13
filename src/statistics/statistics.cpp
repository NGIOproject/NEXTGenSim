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
#include <statistics/statistics.h>
#include <statistics/average.h>
#include <statistics/min.h>
#include <statistics/max.h>
#include <statistics/median.h>
#include <statistics/percentile95.h>
#include <statistics/percentiles.h>
#include <statistics/interquartiledifference.h>
#include <statistics/standarddeviation.h>
#include <statistics/accumulate.h>
#include <statistics/countliterals.h>
#include <statistics/accumulateliterals.h>

#include <utils/utilities.h>

#include <cstdlib>
#include <string>
#include <math.h>

#include <fstream>
#include <assert.h>


using namespace Statistics;

namespace Simulator {

    /**
     * The default constructor for the class
     */

    SimStatistics::SimStatistics(Log* log) {
        this->log = log;
        this->showStatsStdOut = false;
    }

    /**
     * The default destructor for the class
     */

    SimStatistics::~SimStatistics() {
    }

    /**
     * Sets the metrics that will be computed for the job
     * @param jobMetrics sets the metrics that will computed for jobs
     */
    void SimStatistics::setJobMetricsDefinition(Metrics* jobMetrics) {
        this->jobMetrics = jobMetrics;
    }

    /**
     * Sets the metrics that will be computed for the policy
     * @param policyMetrics The metrics that will be computed for the policy.
     */
    void SimStatistics::setPolicyMetricsDefinition(Metrics* policyMetrics) {
        this->policyMetrics = policyMetrics;
    }

    /**
     * Compute the set of metrics for all the jobs that have been simulated
     * @param jobs A reference to the hashmap containing the jobs that will be used for compute the metrics
     * @return The set of computed metrics
     */
    Metrics* SimStatistics::computeJobMetrics(map<int, Job*>* jobs) {
        log->debug("Collecting jobs fields for each metric and computing the required values ..");

        for (Metrics::iterator it = this->jobMetrics->begin(); it != this->jobMetrics->end(); ++it) {
            Metric* currentMetric = *it;

            vector<double> dvalue;
            vector<int> ivalue;
            vector<string> svalue;
            map<string, vector<double>*> csvalue;

            switch (currentMetric->getnativeType()) {
                case DOUBLE:
                    getJobVariableValues(&dvalue, jobs, currentMetric);
                    computeMetricValue(&dvalue, currentMetric);
                    break;
                case INTEGER:
                    getJobVariableValues(&ivalue, jobs, currentMetric);
                    computeMetricValue(&ivalue, currentMetric);
                    break;
                case STRING:
                    getJobVariableValues(&svalue, jobs, currentMetric);
                    computeMetricValue(&svalue, currentMetric);
                    break;
                case COMPOSED_DOUBLE:
                    getJobVariableValues(&csvalue, jobs, currentMetric);
                    computeMetricValue(&csvalue, currentMetric);
                    break;
                default:
                    //shouldn't happend !
                    assert(false);
            }
        }
        return this->jobMetrics;
    }

    /**
     * Computes all the metrics that have been collected for the policy using the entity collector
     * @param collector A reference to the entity collector
     * @return The set of computed metrics
     */
    Metrics* SimStatistics::computePolicyMetrics(PolicyEntityCollector* collector) {
        log->debug("Collecting policy fields for each metric and computing the required values ..");

        for (Metrics::iterator it = this->policyMetrics->begin(); it != this->policyMetrics->end(); ++it) {
            Metric* currentMetric = *it;

            vector<double> * dvalue;
            vector<int> * ivalue;

            switch (currentMetric->getnativeType()) {
                case DOUBLE:
                    dvalue = getPolicydVariableMetrics(collector, currentMetric);
                    computeMetricValue(dvalue, currentMetric);
                    break;
                case INTEGER:
                    ivalue = getPolicyiVariableMetrics(collector, currentMetric);
                    computeMetricValue(ivalue, currentMetric);
                    break;
                default:
                    //shouldn't happend !
                    assert(false);
            }

        }
        return this->policyMetrics;
    }

    /**
     * Extracts the set of double values from the policy entity collector based on the provided metric
     * @param collector A reference to the entity collector
     * @param metric The metric to be computed
     * @return The double values that are required for compute the provided metric
     */
    vector<double> * SimStatistics::getPolicydVariableMetrics(PolicyEntityCollector* collector, Metric* metric) {

        //the values must be copied.. may be the statistical stimator will modify some values ..
        vector<double>* values = new vector<double>();

        int samples = collector->gettimeStamp()->size();

        for (int i = 0; i < samples; i++)
            switch (metric->getType()) {
                case JOBS_IN_WQUEUE:
                {
                    values->push_back(collector->getnumberJobsInTheQueue()->operator[](i));
                    break;
                }
                case CPUS_USED:
                {
                    values->push_back(collector->getNumberCPUsUsed()->operator[](i));
                    break;
                }
                case BACKFILLED_JOBS:
                {
                    values->push_back(collector->getbackfilledJobs()->operator[](i));
                    break;
                }
                case LEAST_WORK_LEFT:
                {
                    values->push_back(collector->getleftWork()->operator[](i));
                    break;
                }
                case RUNNING_JOBS:
                {
                    values->push_back(collector->getrunningJobs()->operator[](i));
                    break;
                }
                default:
                    assert(false);

            }

        return values;
    }

    /**
     * Extracts the set of integer values from the policy entity collector based on the provided metric
     * @param collector A reference to the entity collector
     * @param metric The metric to be computed
     * @return The double values that are required for compute the provided metric
     */
    vector<int> * SimStatistics::getPolicyiVariableMetrics(PolicyEntityCollector* collector, Metric* metric) {
        //TODO: no integer values right now
        return (vector<int>*) NULL; //Dummy return
    }

    /**
     * Shows the statistics that have been specified in the configuration file at the current moment. The results are shown in the standard output.
     */
    void SimStatistics::ShowCurrentStatisticsStd() {
        //we get the metrics realted to the job entity
        vector<string> jobEntityMetrics;

        this->MetricsValues(this->jobMetrics, &jobEntityMetrics, true);
        this->MetricsValues(this->policyMetrics, &jobEntityMetrics, true);
    }

    /**
     * Save the statistics specified in the configuration file in the specified file in case it would be specified. If the output file is not present the data will be shown in the standard output.
     */
    void SimStatistics::saveGlobalStatistics() {

        //we get the metrics realted to the job entity
        vector<string> jobEntityMetrics;

        this->MetricsValues(this->jobMetrics, &jobEntityMetrics, this->GlobalStatisticsOutputFile == "" || showStatsStdOut);
        this->MetricsValues(this->policyMetrics, &jobEntityMetrics, this->GlobalStatisticsOutputFile == "" || showStatsStdOut);

        if (this->GlobalStatisticsOutputFile == "")
            return;

        //if output file specified
        cout << "Creating statistics file " << this->GlobalStatisticsOutputFile << endl;
        string file = this->GlobalStatisticsOutputFile;
        std::ofstream fout(file.c_str(), ios::trunc);


        //we dump it to the file

        fout << "Metrics entity:job" << endl;
        for (vector<string>::iterator it = jobEntityMetrics.begin(); it != jobEntityMetrics.end(); ++it) {
            fout << *it << endl;
        }
    }

    /**
     * Generate a string from the metrics that have been previously computed. In the case that the dumpStdOut is set it will the statistics will be shown in the standard output
     * @param metrics The metrics that have been computed
     * @param descriptions The output vector where the metrics will be stored as string
     * @param dumpStdOut A boolean that indicates if the jobs have to be shown in the standard output
     */
    void SimStatistics::MetricsValues(Metrics* metrics, vector<string>* descriptions, bool dumpStdOut) {
        for (Metrics::iterator it = metrics->begin(); it != metrics->end(); ++it) {
            Metric* metric = *it;
            string StringValue = GetStringValue(metric);

            if (dumpStdOut)
                std::cout << StringValue << endl;

            descriptions->push_back(StringValue);
        }
    }

    /**
     * Returns the value in string format of the provided metric (the returned string is in a human readable format)
     * @param metric The metric that have to be humanreable converted
     * @return The content of the metric in string format
     */
    string SimStatistics::GetStringValue(Metric* metric) {
        switch (metric->getnativeType()) {
            case DOUBLE:
                return metric->getname() + ":" + metric->getestimatorName() + ":" + ftos(metric->getnativeDouble());
                break;
            case INTEGER:
                return metric->getname() + ":" + metric->getestimatorName() + ":" + ftos(metric->getnativeDouble());
                break;
            case STRING:
                return metric->getname() + ":" + metric->getestimatorName() + ":" + metric->getnativeString();
                break;
            default:
                //shouldn't happend !
                assert(false);
        }


    }

    /**
     * This function computes the required metric using the integer values provided in the vector
     * @param input The vector conatining the values
     * @param metric The metric that has to be fullfiled
     */
    void SimStatistics::computeMetricValue(vector<int>* input, Metric* metric) {
        /* only min and max can be treated as integer estimators*/

        StatisticalEstimator * estimator = NULL;
        //statistic_t N = metric->getstatisticUsed();
        switch (metric->getstatisticUsed()) {
            case MIN:
            {
                estimator = (StatisticalEstimator*) new Min();
                ((Min*) estimator)->setIvalues(input);
                break;
            }
            case MAX:
            {
                estimator = (StatisticalEstimator*) new Max();
                ((Max*) estimator)->setIvalues(input);
                break;
            }
            case ACCUMULATE:
            {
                estimator = (StatisticalEstimator*) new Accumulate();
                ((Accumulate*) estimator)->setiValues(input);
                break;
            }
            default:
                assert(false);
        }
        Metric* result = estimator->computeValue();
        metric->setNativeDouble(result->getnativeDouble());
        delete result;

    }

    /**
     * This function computes the required metric using the string values provided in the vector
     * @param input The vector conatining the values
     * @param metric The metric that has to be fullfiled
     */

    void SimStatistics::computeMetricValue(vector<string>* input, Metric* metric) {
        /* only min and max can be treated as integer estimators*/

        StatisticalEstimator * estimator = NULL;

        switch (metric->getstatisticUsed()) {
            case COUNT_LITERALS:
            {
                estimator = (StatisticalEstimator*) new CountLiterals();
                ((CountLiterals*) estimator)->setSValues(input);
                break;
            }
            default:
                assert(false);
        }

        Metric* result = estimator->computeValue();
        metric->setNativeString(result->getnativeString());
        delete result;

    }

    /**
     * This function computes the required metric using values provided in the hashmap of vector of doubles
     * @param input The vector conatining the values
     * @param metric The metric that has to be fullfiled
     */

    void SimStatistics::computeMetricValue(map<string, vector<double>*>* input, Metric* metric) {
        /* only min and max can be treated as integer estimators*/

        StatisticalEstimator * estimator = NULL;

        switch (metric->getstatisticUsed()) {
            case ACCUMULATE_LITERALS:
            {
                estimator = (StatisticalEstimator*) new AccumulateLiterals();
                ((AccumulateLiterals*) estimator)->setSPValues(input);
                break;
            }
            default:
                assert(false);
        }

        Metric* result = estimator->computeValue();
        metric->setNativeString(result->getnativeString());
        metric->setNativeType(STRING);
        delete result;

    }

    /**
     * This function computes the required metric using the double values provided in the vector
     * @param input The vector conatining the values
     * @param metric The metric that has to be fullfiled
     */
    void SimStatistics::computeMetricValue(vector<double>* input, Metric* metric) {
        StatisticalEstimator * estimator = NULL;

        switch (metric->getstatisticUsed()) {
            case AVG:
            {
                estimator = (StatisticalEstimator*) new Average();
                ((Average*) estimator)->setValues(input);
                break;
            }
            case MIN:
            {
                estimator = (StatisticalEstimator*) new Min();
                ((Min*) estimator)->setDvalues(input);
                break;
            }
            case MAX:
            {
                estimator = (StatisticalEstimator*) new Max();
                ((Max*) estimator)->setDvalues(input);
                break;
            }
            case MEDIAN:
            {
                estimator = (StatisticalEstimator*) new Median();
                ((Median*) estimator)->setValues(input);
                break;
            }
            case PERCENTILE95:
            {
                estimator = (StatisticalEstimator*) new Percentile95();
                ((Percentile95*) estimator)->setValues(input);
                break;
            }
            case PERCENTILES:
            {
                estimator = (StatisticalEstimator*) new Percentiles();
                ((Percentiles*) estimator)->setValues(input);
                break;
            }
            case IQR:
            {
                estimator = (StatisticalEstimator*) new InterquartileDifference();
                ((InterquartileDifference*) estimator)->setValues(input);
                break;
            }
            case STDEV:
            {
                estimator = (StatisticalEstimator*) new StandardDeviation();
                ((StandardDeviation*) estimator)->setValues(input);
                break;
            }
            case ACCUMULATE:
            {
                estimator = (StatisticalEstimator*) new Accumulate();
                ((Accumulate*) estimator)->setValues(input);
                break;
            }
            default:
                assert(false);
        }

        Metric* result = estimator->computeValue();
        if (result->getnativeType() == DOUBLE) {
            metric->setNativeDouble(result->getnativeDouble());
            metric->setNativeType(DOUBLE);
        }
        if (result->getnativeType() == STRING) {
            metric->setNativeString(result->getnativeString());
            metric->setNativeType(STRING);
        }
        if (result->getnativeType() == INTEGER) {
            metric->setNativeBool(result->getnativeDouble());
            metric->setNativeType(INTEGER);
        }
        delete result;
        delete estimator;

    }

    /**
     * Extracts the set of integer values from the list of jobs based on the provided metric
     * @param collector A reference to the list of jobs
     * @param metric The metric to be computed
     * @return The integer values that are required for compute the provided metric
     */
    void SimStatistics::getJobVariableValues(vector<int>* output, map<int, Job*>* jobs, Metric* metric) {
        for (map<int, Job*>::iterator it = jobs->begin(); it != jobs->end(); ++it) {
            Job * job = it->second;

            if (job->getJobSimStatus() != COMPLETED && metric->getType() != kILLED_JOB)
                continue;

            switch (metric->getType()) {

                case DELAY_SECONDS:
                    //        sld_predicted = (job->getRuntimePrediction()+job->getWaitTimePrediction())/job->getRuntimePrediction();
                    // Really, these are integers, but this function has to move data around as double so we do some casting.
                    output->push_back((int) job->getDelaySec());

                default:
                    break;

            }
        }

    }

    /**
     * Extracts the set of string values from the list of jobs based on the provided metric
     * @param collector A reference to the list of jobs
     * @param metric The metric to be computed
     * @return The string values that are required for compute the provided metric
     */

    void SimStatistics::getJobVariableValues(vector<string>* output, map<int, Job*>* jobs, Metric* metric) {
        for (map<int, Job*>::iterator it = jobs->begin(); it != jobs->end(); ++it) {
            Job * job = it->second;

            if (job->getJobSimStatus() != COMPLETED)
                continue;

            switch (metric->getType()) {
                case SUBMITED_JOBS_CENTER:
                    output->push_back(job->getJobSimCenter());
                    break;
                case RSP_USED:
                    output->push_back(getStringOfRSP(job->getallocatedWith()));
                    break;
                case COMPUTATIONAL_COST_RESPECTED:
                    if (job->getMaximumAllowedCost() >= job->getMonetaryCost())
                        output->push_back(string("YES"));
                    else
                        output->push_back(string("NO"));
                    break;
                default:
                    assert(false);
            }

        }
    }

    /**
     * Given a resource selection policy it returns a string conatining the string representation
     * @param RSP The resource selection policy
     * @return The string representation of the rsp
     */
    string SimStatistics::getStringOfRSP(RS_policy_type_t RSP) {

        switch (RSP) {
            case FIRST_FIT:
                return "FIRST_FIT";
                break;
            case FIRST_CONTINUOUS_FIT:
                return "FIRST_CONTINUOUS_FIT";
                break;
            case FF_AND_FCF:
                return "FF_AND_FCF";
                break;
            case FIND_LESS_CONSUME:
                return "FIND_LESS_CONSUME";
                break;
            case FIND_LESS_CONSUME_THRESSHOLD:
                return "FIND_LESS_CONSUME_THRESSHOLD";
                break;
            case EQUI_DISTRIBUTE_CONSUME:
                return "EQUI_DISTRIBUTE_CONSUME";
                break;
            case DC_FIRST_VM:
                return "DC_FIRST_VM";
                break;
            case DEFAULT_RS_P:
                return "DEFAULT_RS_P";
                break;
            case OTHER_RS:
                return "OTHER_RS";
                break;
            default:
                std::cout << "ERROR on RSP: " << (uint32_t) RSP << " vs " << (uint32_t) FF_AND_FCF << std::endl;
                assert(false);
        }
    }

    /**
     * Extracts the set of string vector of double pair values from the list of jobs based on the provided metric
     * @param collector A reference to the list of jobs
     * @param metric The metric to be computed
     * @return The hashmap of vectors values that are required for compute the provided metric
     */
    void SimStatistics::getJobVariableValues(map<string, vector<double>*>* output, map<int, Job*>* jobs, Metric* metric) {
        for (map<int, Job*>::iterator it = jobs->begin(); it != jobs->end(); ++it) {
            Job * job = it->second;

            switch (metric->getType()) {
                case LOAD_JOBS_CENTERS:
                {
                    map<string, vector<double>*>::iterator inserted = output->find(job->getJobSimCenter());

                    if (inserted == output->end()) {
                        vector<double>* nvect = new vector<double>;
                        nvect->push_back(job->getRequestedTime() * job->getNumberProcessors());
                        output->insert(pair<string, vector<double>*>(job->getJobSimCenter(), nvect));
                    } else {
                        vector<double>* vect = (*output)[job->getJobSimCenter()];
                        vect->push_back(job->getRequestedTime() * job->getNumberProcessors());
                    }

                    break;
                }
                default:
                    assert(false);
            }

        }
    }

    /**
     * Extracts the set of double  values from the list of jobs based on the provided metric
     * @param collector A reference to the list of jobs
     * @param metric The metric to be computed
     * @return The double values that are required for compute the provided metric
     */

    void SimStatistics::getJobVariableValues(vector<double>* output, map<int, Job*>* jobs, Metric* metric) {
        for (map<int, Job*>::iterator it = jobs->begin(); it != jobs->end(); ++it) {
            Job * job = it->second;

            if (job->getJobSimStatus() != COMPLETED && metric->getType() != kILLED_JOB)
                continue;
            double sld_predicted;
            switch (metric->getType()) {
                case SLD:
                    if (job->getRunTime() <= 0) {
                        log->debug("The job " + to_string(job->getJobNumber()) + " has runtime less equal zero, we don't compute its metric", 4);
                        continue;
                    }
                    output->push_back(job->getJobSimSLD());
                    break;

                case BSLD:
                    if (job->getRunTime() <= 0) {
                        log->debug("The job " + to_string(job->getJobNumber()) + " has runtime less equal zero, we don't compute its metric", 4);
                        continue;
                    }
                    output->push_back(job->getJobSimBSLD());
                    break;

                case BSLD_PRT:
                    if (job->getRunTime() <= 0) {
                        log->debug("The job " + to_string(job->getJobNumber()) + " has runtime less equal zero, we don't compute its metric", 4);
                        continue;
                    }
                    output->push_back(job->getJobSimBSLDPRT());
                    break;

                case WAITTIME:
                    output->push_back(job->getJobSimWaitTime());
                    break;
                case BACKFILLED_JOB:
                    output->push_back((double) job->getJobSimisBackfilled());
                    break;
                case kILLED_JOB:
                    output->push_back((double) job->getJobSimisKilled());
                    break;
                case PENALIZED_RUNTIME:
                    output->push_back((double) job->getPenalizedRunTimeReal());
                    break;
                case PERCENTAGE_PENALIZED_RUNTIME:
                    output->push_back((double) ((job->getPenalizedRunTimeReal() / job->getRunTime())*100));
                    break;
                case COMPUTATIONAL_COST:
                    output->push_back((double) (job->getMonetaryCost()));
                    break;
                case ESTIMATED_COMPUTATIONAL_COST:
                    output->push_back((double) (job->getPredictedMonetaryCost()));
                    break;
                case ESTIMATED_START_TIME_FIRST_FIT:
                    output->push_back((double) (job->getJobSimEstimatedSTWithFF()));
                    break;
                case PREDICTED_WT:
                    output->push_back((double) (job->getWaitTimePrediction()));
                    break;
                case PREDICTED_RUNTIME:
                    output->push_back((double) (job->getRuntimePrediction()));
                    break;
                case SLD_PREDICTED:
                    if (job->getWaitTimePrediction() != 0)
                        output->push_back((double) (job->getRuntimePrediction() + job->getWaitTimePrediction()) / job->getWaitTimePrediction());
                    break;
                case ESTIMATED_COMPUTATIONAL_COST_ERROR:
                    if (job->getMonetaryCost() != 0)
                        output->push_back((double) (((job->getMonetaryCost() - job->getPredictedMonetaryCost()) / job->getMonetaryCost())*100));
                    break;
                case PREDICTED_WT_ERROR:
                    if (job->getJobSimWaitTime() != 0)
                        output->push_back((double) (((job->getJobSimWaitTime() - job->getWaitTimePrediction()) / job->getJobSimWaitTime())*100));
                    else
                        output->push_back(0);
                    break;
                case PREDICTED_RUNTIME_ERROR:
                    output->push_back((double) (((job->getRunTime() - job->getRuntimePrediction()) / job->getRunTime())*100));
                    break;

                case SLD_PREDICTED_ERROR:
                    sld_predicted = (job->getRuntimePrediction() + job->getWaitTimePrediction()) / job->getRuntimePrediction();
                    output->push_back((double) (((job->getJobSimSLD() - sld_predicted) / job->getJobSimSLD())*100));

                case ARCHER_RATIO:
                    output->push_back(job->getArcherRatio());


                default:
                    break;
            }
        }

    }

    /**
     * Function set the metrics that will be computed for the jobs
     * @param metrics The set of metrics
     */
    void SimStatistics::addJobMetrics(Metrics* metrics) {
        this->jobMetrics = metrics;
    }

    /**
     * Returns the file path of the file where the statistics will be stored
     * @return A string containing the file
     */
    string SimStatistics::getGlobalStatisticsOutputFile() const {
        return GlobalStatisticsOutputFile;
    }

    /**
     * Sets the file path where the statistics will be stored
     * @param theValue The file path
     */
    void SimStatistics::setGlobalStatisticsOutputFile(const string& theValue) {
        //expanding the file name (replacing variables by literals)
        GlobalStatisticsOutputFile = expandFileName(theValue);
    }

    void SimStatistics::setsimOutputPath(const string& theValue) {
        simOutputPath = path(expandFileName(theValue));
        log->debug("Stats output path root: " + simOutputPath.string(), 2);
    }

    path SimStatistics::getsimOutputPath(void){
        return simOutputPath;
    }

    void SimStatistics::setSchedDataFile(const string& theValue) {
        if (theValue != ""){
            schedDataFile = simOutputPath / expandFileName(theValue);
        }
        else {
            schedDataFile = simOutputPath / "/sched_data.txt";
        }
        log->debug("Sched output data file: " + schedDataFile.string(), 2);
    }

    path SimStatistics::getSchedDataFile(void){
        return schedDataFile;
    }

    void SimStatistics::setSchedCountFile(const string& theValue) {      
        if (theValue != "") {
            schedCountFile = simOutputPath / expandFileName(theValue);
        } else {
            schedCountFile = simOutputPath / "/sched_coeff.txt";
        }
        log->debug("Sched output count file: " + schedCountFile.string(), 2);
    }

    path SimStatistics::getSchedCountFile(void){
        return schedCountFile;
    }

    /**
     * Returns true if the statistics will be displayed also to the standard output
     * @return A bool indicating if the stats will be shown in the STDOUT
     */
    bool SimStatistics::getshowStatsStdOut() const {
        return showStatsStdOut;
    }

    /**
     * Sets in the statistics have to be shown in the standard out
     * @param theValue True if the statistics have to be displayed in the STDOUT
     */
    void SimStatistics::setShowStatsStdOut(bool theValue) {
        showStatsStdOut = theValue;
    }

    void SimStatistics::doHeatmap(map<int, Job*>* jobs) {

        // Vectors correspond to runtime in hours
        // Entries in vectors to node count in the ranges
        // 1, 2, 3-4, 5-8, 9-16, 17-32, 33-64, 65-128, 129-256, 257-512, 513-1024, 1025-2048, 2049-4096, 4097-8192
        vector<double> zero_one = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        vector<double> one_three = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        vector<double> three_six = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        vector<double> six_twelve = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        vector<double> twelve_twentyfour = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
        vector<double> twentyfour_plus = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

        vector<int> zero_one_c = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> one_three_c = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> three_six_c = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> six_twelve_c = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> twelve_twentyfour_c = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        vector<int> twentyfour_plus_c = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


        //stuff
        for (map<int, Job*>::iterator it = jobs->begin(); it != jobs->end(); ++it) {
            Job * job = it->second;
            double runtime = (job->getRunTime() / 3600);
            int index = int(ceil(log2(job->getNumSimNodes())));

            if (runtime > 0 && runtime <= 1 && job->getNumSimNodes() > 0) {
                if (zero_one[index] == -1) {
                    zero_one[index] = job->getArcherRatio();
                } else {
                    zero_one[index] += job->getArcherRatio();
                }
                zero_one_c[index] += 1;
            } else if (runtime > 1 && runtime <= 3 && job->getNumSimNodes() > 0) {
                if (one_three[index] == -1) {
                    one_three[index] = job->getArcherRatio();
                } else {
                    one_three[index] += job->getArcherRatio();
                }
                one_three_c[index] += 1;
            } else if (runtime > 3 && runtime <= 6 && job->getNumSimNodes() > 0) {
                if (three_six[index] == -1) {
                    three_six[index] = job->getArcherRatio();
                } else {
                    three_six[index] += job->getArcherRatio();
                }
                three_six_c[index] += 1;
            } else if (runtime > 6 && runtime <= 12 && job->getNumSimNodes() > 0) {
                if (six_twelve[index] == -1) {
                    six_twelve[index] = job->getArcherRatio();
                } else {
                    six_twelve[index] += job->getArcherRatio();
                }
                six_twelve_c[index] += 1;
            } else if (runtime > 12 && runtime <= 24 && job->getNumSimNodes() > 0) {
                if (twelve_twentyfour[index] == -1) {
                    twelve_twentyfour[index] = job->getArcherRatio();
                } else {
                    twelve_twentyfour[index] += job->getArcherRatio();
                }
                twelve_twentyfour_c[index] += 1;
            } else if (runtime > 24 && job->getNumSimNodes() > 0) {
                if (twentyfour_plus[index] == -1) {
                    twentyfour_plus[index] = job->getArcherRatio();
                } else {
                    twentyfour_plus[index] += job->getArcherRatio();
                }
                twentyfour_plus_c[index] += 1;
            } else {
                // Panic at the disco!
            }
            
        }


        cout << "Creating statistics file for heatmaps." << endl;

        std::ofstream fout(schedDataFile.c_str(), ios::trunc);

        fout.precision(10);
        for (int i = 0; i < (int)zero_one_c.size(); i++) {
            zero_one[i] = zero_one[i] / zero_one_c[i];
            one_three[i] = one_three[i] / one_three_c[i];
            three_six[i] = three_six[i] / three_six_c[i];
            six_twelve[i] = six_twelve[i] / six_twelve_c[i];
            twelve_twentyfour[i] = twelve_twentyfour[i] / twelve_twentyfour_c[i];
            twentyfour_plus[i] = twentyfour_plus[i] / twelve_twentyfour_c[i];
        }

        for (double z : zero_one) {
            fout << z << " ";
        }
        fout << endl;
        for (double z : one_three) {
            fout << z << " ";
        }
        fout << endl;
        for (double z : three_six) {
            fout << z << " ";
        }
        fout << endl;
        for (double z : six_twelve) {
            fout << z << " ";
        }
        fout << endl;
        for (double z : twelve_twentyfour) {
            fout << z << " ";
        }
        fout << endl;
        for (double z : twentyfour_plus) {
            fout << z << " ";
        }
        fout << endl;

        fout.close();


        std::ofstream ffout(schedCountFile.c_str(), ios::trunc);
        for (int z : zero_one_c) {
            ffout << z << " ";
        }
        ffout << endl;
        for (int z : one_three_c) {
            ffout << z << " ";
        }
        ffout << endl;
        for (int z : three_six_c) {
            ffout << z << " ";
        }
        ffout << endl;
        for (int z : six_twelve_c) {
            ffout << z << " ";
        }
        ffout << endl;
        for (int z : twelve_twentyfour_c) {
            ffout << z << " ";
        }
        ffout << endl;
        for (int z : twentyfour_plus_c) {
            ffout << z << " ";
        }
        ffout << endl;
        ffout.close();
        
        std::ofstream rvwout("rvw.out", ios::trunc);
        for (map<int, Job*>::iterator it = jobs->begin(); it != jobs->end(); ++it) {
            Job * job = it->second;
            rvwout << "Rat: " << int(job->getJobNumber()) << " " << int(job->getRunTime()) << " " << int(job->getJobSimWaitTime()) << endl;
        }
        rvwout.close();
        
        
    }
    
        

}
