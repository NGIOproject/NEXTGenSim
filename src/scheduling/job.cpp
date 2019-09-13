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
#include <scheduling/job.h>
#include <utils/utilities.h>

#include <math.h>

namespace Simulator {

    /***************************
      Constructor and destructors
     ***************************/

    /**
     * The default constructor for the class
     */
    Job::Job() {
        jobNumber = -1;
        submitTime = -1;
        waitTime = -1;
        runTime = -1;
        numberProcessors = -1;
        avgCPUTimeUsed = -1;
        usedMemory = -1;
        requestedProcessors = -1;
        requestedTime = -1;
        originalRequestedTime = -1;
        requestedMemory = -1;
        jobSimStatus = LOCAL_QUEUED;

        status = NO_STATUS;

        userId = -1;
        groupId = -1;
        executable = -1;
        queueNumber = -1;
        partitionNumber = -1;
        precedingJobNumber = -1;
        thinkTimePrecedingJob = -1;
        numSimNodes = -1;

        bwMemoryUsed = -1;
        memoryUsed = -1;
        bwEthernedUsed = -1;
        bwNetworkUsed = -1;
        diskUsed = -1;

        cpusPerNode = -1;
        memPerNode = -1;
        diskPerNode = -1;

        jobSimStartTime = -1;
        jobSimFinishTime = -1;
        jobSimWaitTime = -1;
        jobSimSubmitTime = -1;
        jobSimisKilled = false;
        jobSimCompleted = true;
        jobSimisBackfilled = false;
        jobSimBackfillingTime = -1;
        jobSimBSLD = -1;
        jobSimEstimateFinishTime = -1;
        jobSimSLD = -1;
        jobSimCenter = "";

        penalizedRunTime = 0;
        oldPenalizedRunTime = 0;
        penalizedRunTimeReal = 0;

        userRuntimeEstimate = -1;
        jobSimEstimatedShadow = -1;
        runtimePrediction = -1;
        waitTimePrediction = -1;

        predictedMonetaryCost = -1;
        monetaryCost = -1;
        maximumAllowedCost = -1;
        jobSimEstimatedSTWithFF = -1;

        runsAtReducedFrequency = false;
        runEqualsEstimate = false;

        malleable = 0;
        maxNumberOfProcessors = 0;
        
        slurmprio = 1000; // SLURM starts things at 1000, so do we.
        
        nmetric_delaysec = 0.0;
        nmetric_archerratio = -1;
        
        workflow_id = 0;
        workflow_status = 0;

    }

    /**
     * The default destructor for the class
     */
    Job::~Job() {

        // delete all associated penalty times for all buckets
        for (jobMapBucketPenalizedTimes_t::iterator it = jobPenalties.begin(); it != jobPenalties.end(); it++)
            delete it->second;
        jobPenalties.clear();
    }

    /**
     * Reset all penalized times associated to all buckets or one bucket
     */
    void Job::resetBucketPenalizedTime(int bucketId) {
        jobListBucketPenalizedTime_t * currentBucket;
        if (bucketId == -1) {
            // delete all associated penalty times for all buckets
            for (jobMapBucketPenalizedTimes_t::iterator it = jobPenalties.begin(); it != jobPenalties.end(); it++)
                delete it->second;
            jobPenalties.clear();
        } else if (jobPenalties.count(bucketId) != 0) {
            currentBucket = jobPenalties[bucketId];
            jobPenalties.erase(bucketId);
            delete currentBucket;
        }
    }

    /**
     * Add new penalized time related to bucket one bucket
     */
    void Job::addBucketPenalizedTime(int bucketId, double startTime, double endTime, double penalizedTime) {
        jobListBucketPenalizedTime_t * currentBucket;


        if (jobPenalties.count(bucketId) == 0) {
            currentBucket = new jobListBucketPenalizedTime_t();
            jobPenalties[bucketId] = currentBucket;
        } else
            currentBucket = jobPenalties[bucketId];

        // add new penalty time for this bucket
        typedef pair<double, double> pairDouble;
        if (currentBucket->find(startTime) != currentBucket->end())
            currentBucket->erase(currentBucket->find(startTime));

        currentBucket->insert(pairDouble(startTime, penalizedTime));
    }

    /**
     * Get penalized times related to one bucket
     */
    const jobListBucketPenalizedTime_t * Job::getBucketPenalizedTimes(int bucketId) {
        return ( jobPenalties.count(bucketId) != 0 ? jobPenalties[bucketId] : NULL);
    }

    /**
     * This function returns the content of an attribute in string format
     * @param attribute The string of the attribute to be returned
     * @return A string containing the value of the attribute
     */
    string Job::getAttributeValue(string attribute) {
        string result = "";

        if (attribute == "jobNumber") result = to_string(jobNumber);
        else if (attribute == "submitTime") result = ftos(submitTime);
        else if (attribute == "waitTime") result = ftos(waitTime);
        else if (attribute == "runTime") result = ftos(runTime);
        else if (attribute == "numberProcessors") result = to_string(numberProcessors);
        else if (attribute == "avgCPUTimeUsed") result = ftos(avgCPUTimeUsed);
        else if (attribute == "usedMemory") result = ftos(usedMemory);
        else if (attribute == "requestedProcessors") result = to_string(requestedProcessors);
        else if (attribute == "requestedTime") result = ftos(requestedTime);
        else if (attribute == "originalRequestedTime") result = ftos(originalRequestedTime);
        else if (attribute == "requestedMemory") result = ftos(requestedMemory);
        else if (attribute == "jobSimStatus") result = to_string(jobSimStatus);
        else if (attribute == "status") result = to_string(status);
        else if (attribute == "userId") result = to_string(userId);
        else if (attribute == "groupId") result = to_string(groupId);
        else if (attribute == "executable") result = to_string(executable);
        else if (attribute == "queueNumber") result = to_string(queueNumber);
        else if (attribute == "partitionNumber") result = to_string(partitionNumber);
        else if (attribute == "precedingJobNumber") result = to_string(precedingJobNumber);
        else if (attribute == "thinkTimePrecedingJob") result = to_string(thinkTimePrecedingJob);
        else if (attribute == "bwMemoryUsed") result = ftos(bwMemoryUsed);
        else if (attribute == "memoryUsed") result = ftos(memoryUsed);
        else if (attribute == "bwEthernedUsed") result = ftos(bwEthernedUsed);
        else if (attribute == "bwNetworkUsed") result = ftos(bwNetworkUsed);
        else if (attribute == "jobSimStartTime") result = ftos(jobSimStartTime);
        else if (attribute == "jobSimFinishTime") result = ftos(jobSimFinishTime);
        else if (attribute == "jobSimWaitTime") result = ftos(jobSimWaitTime);
        else if (attribute == "jobSimSubmitTime") result = ftos(jobSimSubmitTime);
        else if (attribute == "jobSimisKilled") result = to_string(jobSimisKilled);
        else if (attribute == "jobSimisBackfilled") result = to_string(jobSimisBackfilled);
        else if (attribute == "jobSimBackfillingTime") result = ftos(jobSimBackfillingTime);
        else if (attribute == "jobSimBSLD") result = ftos(jobSimBSLD);
        else if (attribute == "jobSimEstimateFinishTime") result = ftos(jobSimEstimateFinishTime);
        else if (attribute == "jobSimSLD") result = ftos(jobSimSLD);
        else if (attribute == "jobSimCenter") result = jobSimCenter;
        else if (attribute == "penalizedRunTime") result = ftos(penalizedRunTime);
        else if (attribute == "oldPenalizedRunTime") result = ftos(oldPenalizedRunTime);
        else if (attribute == "penalizedRunTimeReal") result = ftos(penalizedRunTimeReal);
        else if (attribute == "userRuntimeEstimate") result = ftos(userRuntimeEstimate);
        else
            assert(false);

        return result;
    }


    /***************************
       Sets and gets functions
     ***************************/

    /**
     * Returns the avgCPUTimeUsed
     * @return A double containing the avgCPUTimeUsed
     */
    double Job::getAvgCPUTimeUsed() {
        return avgCPUTimeUsed;
    }

    /**
     * Sets the avgCPUTimeUsed to the job
     * @param theValue The avgCPUTimeUsed
     */
    void Job::setAvgCPUTimeUsed(double theValue) {
        avgCPUTimeUsed = theValue;
    }

    /**
     * Returns the bwEthernedUsed
     * @return A double containing the bwEthernedUsed
     */
    double Job::getBWEthernedUsed() {
        return bwEthernedUsed;
    }

    /**
     * Sets the bwEthernedUsed to the job
     * @param theValue The bwEthernedUsed
     */
    void Job::setBWEthernedUsed(double theValue) {
        bwEthernedUsed = theValue;
    }

    /**
     * Returns the bwMemoryUsed
     * @return A double containing the bwMemoryUsed
     */
    double Job::getBWMemoryUsed() {
        return bwMemoryUsed;
    }

    /**
     * Sets the bwMemoryUsed to the job
     * @param theValue The bwMemoryUsed
     */
    void Job::setBWMemoryUsed(double theValue) {
        bwMemoryUsed = theValue;
    }

    /**
     * Returns the bwNetworkUsed
     * @return A double containing the bwNetworkUsed
     */
    double Job::getBWNetworkUsed() {
        return bwNetworkUsed;
    }

    /**
     * Sets the bwNetworkUsed to the job
     * @param theValue The bwNetworkUsed
     */
    void Job::setBWNetworkUsed(double theValue) {
        bwNetworkUsed = theValue;
    }

    /**
     * Returns the executable
     * @return A integer containing the executable
     */
    int Job::getExecutable() {
        return executable;
    }

    /**
     * Sets the executable to the job
     * @param theValue The executable
     */
    void Job::setExecutable(int theValue) {
        executable = theValue;
    }

    /**
     * Returns the groupId
     * @return A integer containing the groupId
     */
    int Job::getGroupID() {
        return groupId;
    }

    /**
     * Sets the groupId to the job
     * @param theValue The groupId
     */
    void Job::setGroupID(int theValue) {
        groupId = theValue;
    }

    /**
     * Returns the jobNumber
     * @return A integer containing the jobNumber
     */
    int Job::getJobNumber() {

        return jobNumber;
    }

    /**
     * Sets the jobNumber to the job
     * @param theValue The jobNumber
     */
    void Job::setJobNumber(int theValue) {
        jobNumber = theValue;
    }

    /**
     * Returns the memoryUsed
     * @return A double containing the memoryUsed
     */
    double Job::getMemoryUsed() {
        return memoryUsed;
    }

    /**
     * Sets the memoryUsed to the job
     * @param theValue The memoryUsed
     */
    void Job::setMemoryUsed(double theValue) {
        memoryUsed = theValue;
    }

    /**
     * Returns the numberProcessors
     * @return A integer containing the numberProcessors
     */
    int Job::getNumberProcessors() {
        return numberProcessors;
    }

    /**
     * Sets the numberProcessors to the job
     * @param theValue The numberProcessors
     */
    void Job::setNumberProcessors(int theValue) {
        numberProcessors = theValue;
    }

    /**
     * Sets the partitionNumber to the job
     * @param theValue The partitionNumber
     */
    void Job::setPartitionNumber(int theValue) {
        partitionNumber = theValue;
    }

    /**
     * Returns the partitionNumber
     * @return A integer containing the partitionNumber
     */
    int Job::getPartitionNumber() {
        return partitionNumber;
    }

    /**
     * Returns the precedingJobNumber
     * @return A integer containing the precedingJobNumber
     */
    int Job::getPrecedingJobNumber() {
        return precedingJobNumber;
    }

    /**
     * Sets the precedingJobNumber to the job
     * @param theValue The precedingJobNumber
     */
    void Job::setPrecedingJobNumber(int theValue) {
        precedingJobNumber = theValue;
    }

    /**
     * Returns the queueNumber
     * @return A integer containing the queueNumber
     */
    int Job::getQueueNumber() {
        return queueNumber;
    }

    /**
     * Sets the queueNumber to the job
     * @param theValue The queueNumber
     */
    void Job::setQueueNumber(int theValue) {
        queueNumber = theValue;
    }

    /**
     * Returns the numSimNodes
     * @return A integer containing the queueNumber
     */
    int Job::getNumSimNodes() {
        return numSimNodes;
    }

    /**
     * Sets the numSimNodes to the job
     * @param theValue The numSimNodes
     */
    void Job::setNumSimNodes(int theValue) {
        numSimNodes = theValue;
    }

    /**
     * Returns the  requestedMemory
     * @return A double containing the  requestedMemory
     */
    double Job::getRequestedMemory() {
        return requestedMemory;
    }

    /**
     * Sets the requestedMemory to the job
     * @param theValue The requestedMemory
     */
    void Job::setRequestedMemory(double theValue) {
        //TODO: Do this differently
        if (theValue == -1) theValue = 0;
        requestedMemory = theValue;
    }

    /**
     * Returns the requestedProcessors
     * @return A integer containing the requestedProcessors
     */
    int Job::getRequestedProcessors() {
        return requestedProcessors;
    }

    /**
     * Sets the  requestedProcessors to the job
     * @param theValue The requestedProcessors
     */
    void Job::setRequestedProcessors(int theValue) {
        requestedProcessors = theValue;
    }

    /**
     * Returns the requestedTime
     * @return A double containing the requestedTime
     */
    double Job::getRequestedTime() {
        if (runEqualsEstimate) return runTime;
        else return requestedTime;
    }

    /**
     * Sets the requestedTime to the job
     * @param theValue The requestedTime
     */
    void Job::setRequestedTime(double theValue) {
        /* If first time we save the original*/
        if (this->originalRequestedTime == -1)
            this->originalRequestedTime = theValue;

        requestedTime = theValue;
        userRuntimeEstimate = theValue;
    }

    /**
     * Returns the runTime
     * @return A double containing the runTime
     */
    double Job::getRunTime() {
        if (runTime == -1)
            return this->requestedTime;
        else
            return runTime;
    }

    /**
     * Sets the runTime to the job
     * @param theValue The runTime
     */
    void Job::setRunTime(double theValue) {
        //if no requestime is provided by the workload we estimate it as the same runtime ..
        if (requestedTime == -1)
            this->setRequestedTime(theValue);
        runTime = theValue;

    }

    /**
     * Returns the submitTime
     * @return A double containing the
     */
    double Job::getSubmitTime() {
        return submitTime;
    }

    /**
     * Sets the jobSimSubmitTime to the job
     * @param theValue The jobSimSubmitTime
     */
    void Job::setSubmitTime(double theValue) {
        submitTime = theValue;
        //by default the sim submit time is exactly the same as this one .. but maybe for the load simulation this may change ..
        this->jobSimSubmitTime = submitTime;
    }

    /**
     * Returns the thinkTimePrecedingJob
     * @return A integer containing the thinkTimePrecedingJob
     */
    int Job::getThinkTimePrecedingJob() {
        return thinkTimePrecedingJob;
    }

    /**
     * Sets the  thinkTimePrecedingJob to the job
     * @param theValue The thinkTimePrecedingJob
     */
    void Job::setThinkTimePrecedingJob(int theValue) {
        thinkTimePrecedingJob = theValue;
    }

    /**
     * Returns the usedMemory
     * @return A double containing the usedMemory
     */
    double Job::getUsedMemory() {
        return usedMemory;
    }

    /**
     * Sets the usedMemory to the job
     * @param theValue The usedMemory
     */
    void Job::setUsedMemory(double theValue) {
        usedMemory = theValue;
    }

    /**
     * Returns the userId
     * @return A integer containing the userId
     */
    int Job::getUserID() {
        return userId;
    }

    /**
     * Sets the userId to the job
     * @param theValue The userId
     */
    void Job::setUserID(int theValue) {
        userId = theValue;
    }

    /**
     * Returns the waitTime
     * @return A double containing the waitTime
     */
    double Job::getWaitTime() {
        return waitTime;
    }

    job_status_t Job::getStatus() {
        return status;
    }

    /**
     * Sets the to the job
     * @param theValue The
     */
    void Job::setStatus(job_status_t theValue) {
        status = theValue;
    }

    /**
     * Sets the status to the job
     * @param theValue The status
     */
    void Job::setStatus(int theValue) {
        switch (theValue) {
            case 0:
                status = FAILED;
                break;
            case 1:
                status = COMPLETED;
                break;
            case 5:
                status = CANCELLED;
                break;
            default:
                status = COMPLETED;
        }
    }

    /**
     * Sets the waitTime to the job
     * @param theValue The waitTime
     */
    void Job::setWaitTime(double theValue) {
        waitTime = theValue;
    }

    /**
     * Returns the jobSimFinishTime
     * @return A double containing the jobSimFinishTime
     */
    double Job::getJobSimFinishTime() {
        return jobSimFinishTime;
    }

    /**
     * Sets the jobSimFinishTime to the job
     * @param theValue The jobSimFinishTime
     */
    void Job::setJobSimFinishTime(double theValue) {
        jobSimFinishTime = theValue;
    }

    /**
     * Returns the jobSimStartTime
     * @return A double containing the jobSimStartTime
     */
    double Job::getJobSimStartTime() {
        return jobSimStartTime;
    }

    /**
     * Sets the  jobSimStartTime to the job
     * @param theValue The jobSimStartTime
     */
    void Job::setJobSimStartTime(double theValue) {
        //we save the previous start time only if the startime is different than -1
        if (jobSimStartTime != -1)
            this->jobSimLastSimStartTime = jobSimStartTime;
        jobSimStartTime = theValue;
        this->jobSimWaitTime = theValue - this->jobSimStartTime;
    }

    vector< int > Job::getUsedCpus() const {
        return UsedCpus;
    }

    /**
     * Sets the UsedCpus to the job
     * @param theValue The UsedCpus
     */
    void Job::setUsedCpus(const vector< int >& theValue) {
        UsedCpus = theValue;
    }

    /**
     * Returns the penalizedRunTime
     * @return A double containing the penalizedRunTime
     */
    double Job::getPenalizedRunTime() {
        return penalizedRunTime;
    }

    /**
     * Sets the penalizedRunTime to the job
     * @param theValue The penalizedRunTime
     */
    void Job::setPenalizedRunTime(double theValue) {
        this->oldPenalizedRunTime = penalizedRunTime;
        penalizedRunTime = theValue;
    }

    /**
     * Returns the lastBucket
     * @return A integer containing the lastBucket
     */
    int Job::getLastBucket() {
        return lastBucket;
    }

    /**
     * Sets the lastBucket to the job
     * @param theValue The lastBucket
     */
    void Job::setLastBucket(int theValue) {
        lastBucket = theValue;
    }

    /**
     * Returns the startBucket
     * @return A integer containing the startBucket
     */
    int Job::getStartBucket() {
        return startBucket;
    }

    /**
     * Sets the startBucket to the job
     * @param theValue The startBucket
     */
    void Job::setStartBucket(int theValue) {
        startBucket = theValue;
    }

    /**
     * Returns the jobSimEstimateFinishTime
     * @return A double containing the jobSimEstimateFinishTime
     */
    double Job::getJobSimEstimateFinishTime() const {
        return jobSimEstimateFinishTime;
    }

    /**
     * Sets the jobSimEstimateFinishTime to the job
     * @param theValue The jobSimEstimateFinishTime
     */
    void Job::setJobSimEstimateFinishTime(double theValue) {

        jobSimEstimateFinishTime = theValue;
    }

    /**
     * Returns the jobSimisKilled
     * @return A bool containing the jobSimisKilled
     */
    bool Job::getJobSimisKilled() {
        return this->jobSimisKilled;
    }

    /**
     * Sets the jobSimisKilled to the job
     * @param theValue The jobSimisKilled
     */
    void Job::setJobSimisKilled(bool isKilled) {
        this->jobSimisKilled = isKilled;
    }

    /**
     * Returns the jobSimSubmitTime
     * @return A double containing the jobSimSubmitTime
     */
    double Job::getJobSimSubmitTime() const {
        return jobSimSubmitTime;
    }

    /**
     * Sets the jobSimSubmitTime to the job
     * @param theValue The jobSimSubmitTime
     */
    void Job::setJobSimSubmitTime(double theValue) {
        jobSimSubmitTime = theValue;
    }

    /**
     * Returns the jobSimisBackfilled
     * @return A bool containing the jobSimisBackfilled
     */
    bool Job::getJobSimisBackfilled() const {
        return jobSimisBackfilled;
    }

    /**
     * Sets the jobSimisBackfilled to the job
     * @param theValue The jobSimisBackfilled
     */
    void Job::setJobSimisBackfilled(bool theValue) {
        jobSimisBackfilled = theValue;

        if (!jobSimisBackfilled)
            jobSimBackfillingTime = -1;
    }

    /**
     * Returns the jobSimBackfillingTime
     * @return A double containing the jobSimBackfillingTime
     */
    double Job::getJobSimBackfillingTime() const {
        return jobSimBackfillingTime;
    }

    /**
     * Sets the  jobSimBackfillingTime to the job
     * @param theValue The jobSimBackfillingTime
     */
    void Job::setJobSimBackfillingTime(double theValue) {
        jobSimBackfillingTime = theValue;

        if (theValue != -1)
            jobSimisBackfilled = true;
    }

    /**
     * Returns the jobSimWaitTime
     * @return A double containing the jobSimWaitTime
     */
    double Job::getJobSimWaitTime() const {
        return jobSimWaitTime;
    }

    /**
     * Sets the jobSimWaitTime to the job
     * @param theValue The jobSimWaitTime
     */
    void Job::setJobSimWaitTime(double theValue) {
        assert(theValue >= 0);

        jobSimWaitTime = theValue;

        double runtime;

        if (this->getRequestedTime() != -1)
            runtime = min(this->getRunTime(), this->getRequestedTime());
        else
            runtime = this->getRunTime();


        this->setJobSimSLD((this->getJobSimWaitTime() + runtime) / runtime);
        this->setJobSimBSLD((this->getJobSimWaitTime() + runtime) / max(runtime, (double) BSLD_THRESHOLD));

        this->setJobSimBSLDPRT((this->getJobSimWaitTime() + runtime + this->getPenalizedRunTime()) / max(runtime + this->getPenalizedRunTime(), (double) BSLD_THRESHOLD));



    }

    /**
     * Returns the jobSimSLD
     * @return A double containing the jobSimSLD
     */
    double Job::getJobSimSLD() const {
        return jobSimSLD;
    }

    /**
     * Sets the jobSimSLD to the job
     * @param theValue The jobSimSLD
     */
    void Job::setJobSimSLD(double theValue) {
        assert(theValue >= 1);
        jobSimSLD = theValue;
    }

    /**
     * Returns the jobSimBSLD
     * @return A double containing the jobSimBSLD
     */
    double Job::getJobSimBSLD() const {
        return jobSimBSLD;
    }

    /**
     * Sets the jobSimBSLD to the job
     * @param theValue The jobSimBSLD
     */
    void Job::setJobSimBSLD(double theValue) {
        if (theValue > 1)
            jobSimBSLD = theValue;
        else
            jobSimBSLD = 1;
    }

    /**
     * Returns the jobSimDeadlineMissed
     * @return A bool containing the jobSimDeadlineMissed
     */
    bool Job::getJobSimDeadlineMissed() const {
        return jobSimDeadlineMissed;
    }

    /**
     * Sets the jobSimDeadlineMissed to the job
     * @param theValue The jobSimDeadlineMissed
     */
    void Job::setJobSimDeadlineMissed(bool theValue) {
        jobSimDeadlineMissed = theValue;
    }

    /**
     * Returns the userRuntimeEstimate
     * @return A double containing the userRuntimeEstimate
     */
    double Job::getUserRuntimeEstimate() const {
        return userRuntimeEstimate;
    }

    /**
     * Sets the userRuntimeEstimate to the job
     * @param theValue The userRuntimeEstimate
     */
    void Job::setUserRuntimeEstimate(double theValue) {
        userRuntimeEstimate = theValue;
    }

    /**
     * Returns the jobSimCompleted
     * @return A bool containing the jobSimCompleted
     */
    bool Job::getJobSimCompleted() const {
        return jobSimCompleted;
    }

    /**
     * Sets the jobSimCompleted to the job
     * @param theValue The jobSimCompleted
     */
    void Job::setJobSimCompleted(bool theValue) {
        jobSimCompleted = theValue;
    }

    /**
     * Returns the oldPenalizedRunTime
     * @return A double containing the oldPenalizedRunTime
     */
    double Job::getOldPenalizedRunTime() const {
        return oldPenalizedRunTime;
    }

    /**
     * Sets the oldPenalizedRunTime to the job
     * @param theValue The oldPenalizedRunTime
     */
    void Job::setOldPenalizedRunTime(const double& theValue) {
        oldPenalizedRunTime = theValue;
    }

    /**
     * Returns the center where the job was submitted
     * @return A string containing the center
     */
    string Job::getJobSimCenter() const {
        return jobSimCenter;
    }

    /**
     * Sets the jobSimCenter to the job
     * @param theValue The jobSimCenter
     */
    void Job::setJobSimCenter(const string& theValue) {
        jobSimCenter = theValue;
    }

    /**
     * Returns the penalizedRunTimeReal
     * @return A double containing the penalizedRunTimeReal
     */
    double Job::getPenalizedRunTimeReal() const {
        return penalizedRunTimeReal;
    }

    /**
     * Sets the penalizedRunTimeReal to the job
     * @param theValue The penalizedRunTimeReal
     */
    void Job::setPenalizedRunTimeReal(double theValue) {
        this->oldPenalizedRunTimeReal = this->penalizedRunTimeReal;
        penalizedRunTimeReal = theValue;
    }

    /**
     * Returns the oldPenalizedRunTimeReal
     * @return A double containing the oldPenalizedRunTimeReal
     */
    double Job::getOldPenalizedRunTimeReal() const {
        return oldPenalizedRunTimeReal;
    }

    /**
     * Sets the oldPenalizedRunTimeReal to the job
     * @param theValue The oldPenalizedRunTimeReal
     */
    void Job::setOldPenalizedRunTimeReal(double theValue) {
        oldPenalizedRunTimeReal = theValue;
    }

    /**
     * Returns the Status for the job
     * @return A job_status_t containing the status for the job
     */
    job_status_t Job::getJobSimStatus() const {
        return jobSimStatus;
    }

    /**
     * Sets the  jobSimStatus to the job
     * @param theValue The jobSimStatus
     */
    void Job::setJobSimStatus(const job_status_t& theValue) {
        jobSimStatus = theValue;
    }

    /**
     * Returns the jobSimLastSimStartTime
     * @return A double containing the jobSimLastSimStartTime
     */
    double Job::getJobSimLastSimStartTime() const {
        return jobSimLastSimStartTime;
    }

    /**
     * Sets the jobSimLastSimStartTime to the job
     * @param theValue The jobSimLastSimStartTime
     */
    void Job::setJobSimLastSimStartTime(double theValue) {
        jobSimLastSimStartTime = theValue;
    }

    /**
     * Returns the originalRequestedTime
     * @return A double containing the originalRequestedTime
     */
    double Job::getOriginalRequestedTime() const {
        return originalRequestedTime;
    }

        /**
     * Sets the originalRequestedTime to the job
     * @param theValue The originalRequestedTime
     */
    void Job::setOriginalRequestedTime(double theValue) {
        originalRequestedTime = theValue;
    }

    /**
     * Returns the jobSimEstimatedShadow
     * @return A double containing the jobSimEstimatedShadow
     */
    double Job::getJobSimEstimatedShadow() const {
        return jobSimEstimatedShadow;
    }

    /**
     * Sets the jobSimEstimatedShadow to the job
     * @param theValue The jobSimEstimatedShadow
     */
    void Job::setJobSimEstimatedShadow(double theValue) {
        jobSimEstimatedShadow = theValue;
    }

    /**
     * Returns the resource management method that has been used for allocate the job
     * @return A RS_policy_type_t containing the resource selection policy used for alocate the job
     */
    RS_policy_type_t Job::getallocatedWith() const {
        return allocatedWith;
    }

    /**
     * Sets the resource management method that has been used for allocate the job
     * @param theValue The resource management policy used
     */
    void Job::setAllocatedWith(const RS_policy_type_t& theValue) {
        allocatedWith = theValue;
    }

    /**
     * Returns the predicted runtime for the job estimated by the prediction system (in case it is used, -1 otherwise)
     * @return An integer containing the prediction time
     */
    double Job::getRuntimePrediction() const {
        return runtimePrediction;
    }

    /**
     * Sets the prediction time for the job, in case it is set by the prediction system
     * @param theValue The runtime prediction
     */
    void Job::setRuntimePrediction(double theValue) {
        runtimePrediction = theValue;
    }

    /**
     * Returns the predicted runtime for the job estimated by the prediction system or by the scheduling system (in case it is used, -1 otherwise)
     * @return An integer containing the wt time
     */
    double Job::getWaitTimePrediction() const {
        return waitTimePrediction;
    }

    /**
     * Sets the wait prediction time for the job, in case it is set by the prediction system or by the scheduling system
     * @param theValue The wt prediction
     */
    void Job::setWaitTimePrediction(double theValue) {
        waitTimePrediction = theValue;
    }

    /**
     * Returns the predicted runtime for the job estimated by the prediction system or by the scheduling system (in case it is used, -1 otherwise)
     * @return An integer containing the wt time
     */
    double Job::getPredictedMonetaryCost() const {
        return predictedMonetaryCost;
    }

    /**
     * Sets the cost prediction time for the job, in case it is set by the prediction system or by the scheduling system
     * @param theValue The cost prediction
     */
    void Job::setPredictedMonetaryCost(double theValue) {
        predictedMonetaryCost = theValue;
    }

    /**
     * Returns the cost for the job returned by the scheduling system (in case it is used, -1 otherwise)
     * @return An integer containing the wt time
     */
    double Job::getMonetaryCost() const {
        return monetaryCost;
    }

    /**
     * Sets the cost for the job, in case it is set by the prediction system or by the scheduling system
     * @param theValue The cost prediction
     */
    void Job::setMonetaryCost(double theValue) {
        monetaryCost = theValue;
    }

    /**
     * Returns the maximum required cost that the user provided as an input of the scheduling
     * @return An integer containing the maximum allowd cost
     */
    double Job::getMaximumAllowedCost() const {
        return maximumAllowedCost;
    }

    /**
     * Sets the maxiumum allowed cost for the job
     * @param theValue The cost in units allocations that the job wants to spend
     */
    void Job::setMaximumAllowedCost(double theValue) {
        maximumAllowedCost = theValue;
    }

    /**
     * Returns the EstimatedSTWithFF
     * @return An integer containing the estimated start time with the FirstFit policy
     */
    double Simulator::Job::getJobSimEstimatedSTWithFF() const {
        return jobSimEstimatedSTWithFF;
    }

    /**
     * Sets the EstimatedSTWithFF for the job
     * @param theValue The estimated start time
     */
    void Simulator::Job::setJobSimEstimatedSTWithFF(double theValue) {
        jobSimEstimatedSTWithFF = theValue;
    }

    /**
     * Returns the BSLDRT
     * @return An integer containing the BSLD for the job considering the penalized runtime
     */
    double Simulator::Job::getJobSimBSLDPRT() const {
        return JobSimBSLDPRT;
    }

    /**
     * Sets the BSLD for the job considering the penalized runtime
     * @param theValue The BSLD considering the PRT
     */
    void Simulator::Job::setJobSimBSLDPRT(double theValue) {
        JobSimBSLDPRT = theValue;
    }

    /**
     * Returns the diskUsed
     * @return An integer containing diskUsed BSLD
     */
    double Job::getDiskUsed() const {
        return diskUsed;
    }

    /**
     * Sets the average size of diskUsed for the job
     * @param theValue The diskUsed used
     */
    void Job::setDiskUsed(double theValue) {
        diskUsed = theValue;
    }

    bool Job::getRunsAtReducedFrequency() {
        return runsAtReducedFrequency;
    }

    void Job::setRunsAtReducedFrequency(bool ind) {
        runsAtReducedFrequency = ind;
    }

    double Job::getRunsAtFrequency() {
        return frequency;
    }

    void Job::setRunsAtFrequency(double freq) {
        frequency = freq;
    }

    int Job::getCPUsPerNode() {
        return cpusPerNode;
    }

    void Job::setCPUsPerNode(int cpusPerNode) {
        this->cpusPerNode = cpusPerNode;
    }

    int Job::getMemPerNode() {
        return memPerNode;
    }

    void Job::setMemPerNode(int memPerNode) {
        this->memPerNode = memPerNode;
    }

    int Job::getDiskPerNode() {
        return diskPerNode;
    }

    void Job::setDiskPerNode(int diskPerNode) {
        this->diskPerNode = diskPerNode;
    }

    /**
     * Sets the max numberProcessors to the job
     * @param theValue The numberProcessors
     */
    void Job::setMaxProcessors(int theValue) {
        maxNumberOfProcessors = theValue;
    }

    /**
     * Gets the max numberProcessors to the job
     */
    int Job::getMaxProcessors() {
        return maxNumberOfProcessors;
    }

    /**
     * Sets the min numberProcessors to the job
     * @param theValue The numberProcessors
     */
    void Job::setMinProcessors(int theValue) {
        minNumberOfProcessors = theValue;
    }

    /**
     * Gets the min numberProcessors of the job
     */
    int Job::getMinProcessors() {
        return minNumberOfProcessors;
    }

    /**
     * Sets the malleability of a job
     * @param theValue The numberProcessors
     */
    void Job::setMalleable(int theValue) {
        malleable = theValue;
    }

    /**
     * Gets the malleability of a job
     */
    bool Job::getMalleable() {
        return malleable;
    }
    
    /*
     * Sets the SLURM priority of a job
     * @param theValue The priority as used by SLURM
     */
    void Job::setslurmprio(uint32_t theValue) {
        slurmprio = theValue;
    }

    /*
     * Gets the SLURM priority of a job
     */
    uint32_t Job::getslurmprio() {
        return slurmprio;
    }
    
    int Job::getDelaySec(){
        return nmetric_delaysec;
    }
    
    void Job::setDelaySec(int theValue){
        nmetric_delaysec = theValue;
    }
    
    double Job::getArcherRatio(){
        return nmetric_archerratio;
    }
    
    void Job::setArcherRatio(double theValue){
        nmetric_archerratio = theValue;
    }
    
    double Job::getIOCompRatio(){
        return io_to_runtime_ratio;
    }
    
    void Job::setIOCompRatio(double theValue){
        io_to_runtime_ratio = theValue;
    }

    int Job::getworkflowID(){
        return workflow_id;
    }
    
    void Job::setworkflowID(int theValue){
        workflow_id = theValue;
    }

    int Job::getworkflowStatus(){
        return workflow_status;
    }
    
    void Job::setworkflowStatus(int theValue){
        workflow_status = theValue;
    }

    vector<int> Job::getPreceedingJobs() {
        return preceeding_jobs;
    }

    void Job::setPreceedingJobs(vector<int> theValue) {
        preceeding_jobs = theValue;
    }
    
}
