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
#include <scheduling/schedulingpolicy.h>
#include <utils/utilities.h>
#include <scheduling/simulation.h>
#include <utils/architectureconfiguration.h>
#include <utils/configurationfile.h>
#include <utils/log.h>
#include <utils/paravertrace.h>

#include <utils/jobqueue.h>
#include <scheduling/job.h>
#include <scheduling/reservationtable.h>
#include <scheduling/powerconsumption.h>
#include <math.h>

namespace Simulator {

    /**
     * The default constructor for the class
     */
    SchedulingPolicy::SchedulingPolicy() {
        //  useParaver = false;
        submittedJobs = 0;
        //  this->prvTrace = NULL;
        RS_policyUsed = OTHER_RS;
        schedulingPolicySimulated = OTHER_POLICY;
        maxAllowedRuntime = -1;
        lastTimeResourceUsageUpdate = -1;
        AccumulatedWT = 0;
        AccumulatedSld = 0;
        AccumulatedRespT = 0;
        AccumulatedRT = 0;
        FinishedJobs = 0;
        log = NULL;
        powerAware = false;
        simulateResourceUsage = false;
        power = NULL;
        backfilledReduced = 0;
        backfilledReducedDemand = 0;
        allowUserEstimatedRuntimeViolation = false; //by default we kill jobs
        EmulateCPUFactor = 1;
        numberOfReservations = 0;
    }

    /**
     * The default destructor for the class 
     */
    SchedulingPolicy::~SchedulingPolicy() {
        log->debug("Destroying the policy simulation");

        if (power)
            delete power;
    }

    /**
     * Returns the metric that contains the estimation of which would be the performance of a given job if it would be allocated now
     * @param MetricTpye The metric that indicates which performance variable has to be computed 
     * @param job The job that will be checked (note that it can be null)
     * @return The metric containing the performance that the given job would experiment if it would be submitted right now
     */
    Metric* SchedulingPolicy::EstimatePerformanceResponse(metric_t MetricType, Job* job) {
        Metric* metric = new Metric();

        if (this->globalTime != lastTimeResourceUsageUpdate)
            this->updateResourceUsage();

        switch (MetricType) {
            case MEMORY_AVAILABLE:
                metric->setNativeDouble(this->architecture->getTotalMemory() - currentResourceUsage.getMemoryUsed());
                break;
            case DISK_AVAILABLE:
                metric->setNativeDouble(this->architecture->getTotalDisk() - currentResourceUsage.getDiskSizeUsed());
                break;
            case FREE_CPUS:
                metric->setNativeDouble(this->architecture->getNumberCPUs() - currentResourceUsage.getNumberCPUsUsed());
                break;
            case AVG_SLD_FINISHED_JOBS:
                if (this->FinishedJobs == 0)
                    metric->setNativeDouble(0);
                else
                    metric->setNativeDouble(this->AccumulatedSld / this->FinishedJobs);
                break;
            case AVG_RT_FINISHED_JOBS:
                if (this->FinishedJobs == 0)
                    metric->setNativeDouble(0);
                else
                    metric->setNativeDouble(this->AccumulatedRT / this->FinishedJobs);
                break;
            case AVG_WT_FINISHED_JOBS:
                if (this->FinishedJobs == 0)
                    metric->setNativeDouble(0);
                else
                    metric->setNativeDouble(this->AccumulatedWT / this->FinishedJobs);
                break;
            case AVG_RESPT_FINISHED_JOBS:
                if (this->FinishedJobs == 0)
                    metric->setNativeDouble(0);
                else
                    metric->setNativeDouble(this->AccumulatedRespT / this->FinishedJobs);
                break;

            case OTHER_VAR:
                assert(false);
                break;
            default:
                //no metric known by a generic system, perhaps the specialized class knows more about this ..
                delete metric;
                return NULL;
        }

        return metric;
    }

    /**
     * Returns the metric that contains the static information regarding the system, such os, vendor etc
     * @param MetricTpye The metric that indicates which performance variable has to be computed 
     * @return The metric containing the static system information that has been required
     */
    Metric* SchedulingPolicy::getStaticSystemInformation(metric_t MetricType) {
        Metric* metric = new Metric();
        switch (MetricType) {
            case VENDOR:
                metric->setNativeString(this->architecture->getVendor());
                break;
            case OS_NAME:
                metric->setNativeString(this->architecture->getOSName());
                break;
            case TOTAL_CPUS:
                metric->setNativeDouble(this->architecture->getNumberCPUs());
                break;
            case CLOCK_SPEED:
                metric->setNativeDouble(this->architecture->getClockSpeed());
                break;
            case DISK_SIZE:
                metric->setNativeDouble(this->architecture->getTotalDisk());
                break;
            case MEMORY_SIZE:
                metric->setNativeDouble(this->architecture->getTotalMemory());
                break;
            case OTHER_VAR:
                assert(false);
                break;
            default:
                //no metric known by a generic system, perhaps the specialized class knows more about this ..
                delete metric;
                return NULL;
        }

        return metric;

    }

    /**
     * Removes a job from the RT. At this global level, we only update the derived information regarding the status of the system. Note that the subclasses probably extend this function however they should call this upper class method.
     * @param job The job that has to be removed from the reservationtable
     */
    void SchedulingPolicy::jobFinish(Job* job) {

        //this function is mainly called for update the deriverd information conerning the job that have been finished.. The idea is that his information will be used for compute the averages of the different job variables, such as sld, wt etc.. that will be queried for the brokers.
        double wt = job->getJobSimStartTime() - job->getJobSimSubmitTime();

        this->AccumulatedWT += wt;
        this->AccumulatedSld += (wt + job->getRunTime()) / job->getRunTime();
        this->AccumulatedRespT += job->getRunTime() + wt;
        this->AccumulatedRT += job->getRunTime();

        this->FinishedJobs++;


    }

    /** This function basically updates the structre currentResourceUsage that contains all the derived information about the current status of the system that can be used by the brokering system */
    void SchedulingPolicy::updateResourceUsage() {
        double diskDemand = 0;
        int cpus = 0;
        double memoryDemand = 0;

        //iterated over the running jobs and update it demand
        for (Job* rjob = this->RunningQueue.begin();
                rjob != NULL; rjob = this->RunningQueue.next()) {
            //extract the job info and update the update resources
            diskDemand += rjob->getDiskUsed();
            cpus += rjob->getNumberProcessors();
            memoryDemand += rjob->getMemoryUsed();
        }

        this->currentResourceUsage.setDiskSizeUsed(diskDemand);
        this->currentResourceUsage.setNumberCpusUsed(cpus);
        this->currentResourceUsage.setMemoryUsed(memoryDemand);

    }

    /**
     * Return the number of jobs in the wait queue 
     * @return A integer containing the number of queued jobs 
     */
//    double SchedulingPolicy::getJobsIntheWQ() {
//        // waitQueue is now a private member of the schedulingpolicy implementation.
//        //    This just pulls from an empty pointer.
//        //  return this->waitQueue->getJobs();
//    }

    /**
     * Adds a given job to the wait queue, usually when it arrives to the system.
     * @param job the job to be added to the wait queue
     */
    void SchedulingPolicy::jobArrive(Job* job) {

        map < Job*, bool>::iterator it = JobProcessed.find(job);

        //if it's the first time that the job is being processed we do all the process stuff
        //otherwise we keep the actions 
        if (it == JobProcessed.end()) {
            job->setRunTime(ceil(job->getRunTime() * this->getEmulateCPUFactor()));

            this->submittedJobs++;

            this->JobProcessed.insert(pair < Job*, bool>(job, true));
        }
    }


    //void SchedulingPolicy::schedule()
    //{
    //	
    //}

    /**
     * Sets the globalTime to the SchedulingPolicy
     * @param theValue The globalTime
     */
    void SchedulingPolicy::setGlobalTime(uint64_t globalTime) {
        this->log->debug("Setting the global time of the schdeuling policy and reservationTable to " + ftos(globalTime), 6);
        this->globalTime = globalTime;

        /* we set also the global time for the reservationTable */
        this->reservationTable->setGlobalTime(globalTime);
    }

    /**
     * Returns the reservationTable
     * @return A reference reservationTable
     */
    ReservationTable* SchedulingPolicy::getReservationTable() const {
        return reservationTable;
    }

    /**
     * Sets the reservationTable to the SchedulingPolicy
     * @param theValue The reservationTable
     */
    void SchedulingPolicy::setReservationTable(ReservationTable* theValue) {
        reservationTable = theValue;
    }

    /**
     * Returns the prvTrace
     * @return A reference to the prvTrace
     */
    trace* SchedulingPolicy::getOutputTrace() const {
        return opTrace;
    }

    /**
     * Sets the prvTrace to the SchedulingPolicy
     * @param theValue The prvTrace
     */
    void SchedulingPolicy::setOutputTrace(trace* theValue) {
        opTrace = theValue;
        outputTrace = true;
    }

    /**
     * Returns the number of backilled jobs 
     * @return A double containing the number of backfilled jobs 
     */
    double SchedulingPolicy::getBackfilledJobs() {
        return 0; //by default we do not assume that the policy uses backfill optimization 
    }

    /**
     * Returns the architecture that is being used by the schedulingpolicy
     * @return A reference to the scheduling
     */
    ArchitectureConfiguration* SchedulingPolicy::getArchitecture() const {
        return architecture;
    }

    /**
     * Sets the architecture to the SchedulingPolicy 
     * @param theValue The sim
     */
    void SchedulingPolicy::setArchitecture(ArchitectureConfiguration* theValue) {
        currentResourceUsage.setArchitecture(theValue);
        architecture = theValue;

    }

    /**
     * Returns the reference to the logging engine 
     * @return A reference to the log 
     */

    Log* SchedulingPolicy::getlog() const {
        return log;
    }

    /**
     * Sets the log  to the SchedulingPolicy 
     * @param theValue The log
     */
    void SchedulingPolicy::setLog(Log* theValue) {
        log = theValue;
    }

    /**
     * Returns the EmulateCPUFactor
     * @return A double containing the EmulateCPUFactor
     */
    double SchedulingPolicy::getEmulateCPUFactor() const {
        return EmulateCPUFactor;
    }

    /**
     * Sets the EmulateCPUFactor  to the SchedulingPolicy 
     * @param theValue The EmulateCPUFactor
     */
    void SchedulingPolicy::setEmulateCPUFactor(const double& theValue) {
        EmulateCPUFactor = theValue;
    }

    /**
     * This function is called after the simulation finishes , the scheduler may decide to do some postprocessing stuff.
     */
    void SchedulingPolicy::PostProcessScheduling() {

    }

    /**
     * This function is called before start the simulation , the scheduler may decide to do some init stuff.
     */
    void SchedulingPolicy::PreProcessScheduling() {

    }

    /**
     * Returns the resource selection policy used 
     * @return A RS_policy_type_t indicating the resource allocation policy
     */
    RS_policy_type_t SchedulingPolicy::getRS_policyUsed() const {
        return RS_policyUsed;
    }

    /**
     * Sets the useParaver  to the SchedulingPolicy 
     * @param theValue The useParaver
     */
    void SchedulingPolicy::setGenOuputTrace(bool theValue) {
        outputTrace = theValue;
    }


    /**
     * Returns the true if the paraver traces have to be generated 
     * @return A bool indicating if the parver traces have to be generated
     */#include <scheduling/job.h>

bool SchedulingPolicy::getGenOutputTrace() const {
        return outputTrace;
    }

    /**
     * Sets the allowUserEstimatedRuntimeViolation to the SchedulingPolicy indicating if jobs must be killed on user runtime violation or not
     * @param theValue Kill or not the jobs 
     */
    void SchedulingPolicy::setAllowUserEstimatedRuntimeViolation(bool theValue) {
        allowUserEstimatedRuntimeViolation = theValue;
    }

    /**
     * Returns if the jobs will be killed on job runtime violation (assuming job provided by user) 
     * @return Boolean indicating if they will get killed or not. 
     */
    bool SchedulingPolicy::getAllowUserEstimatedRuntimeViolation() const {
        return allowUserEstimatedRuntimeViolation;
    }

    /**
     * Sets the RS_policyUsed  used by the policy 
     * @param theValue The RS_policyUsed
     */
    void SchedulingPolicy::setRS_policyUsed(const RS_policy_type_t& theValue) {
        RS_policyUsed = theValue;
    }

    /**
     * Returns the scheduling selection policy used 
     * @return A SchedulingPolicy_t indicating the scheduling policy
     */
    policy_type_t SchedulingPolicy::getschedulingPolicySimulated() const {
        return schedulingPolicySimulated;
    }

    /**
     * Sets the schedulingPolicySimulated implemented by the class
     * @param theValue The SchedulingPolicy_t
     */
    void SchedulingPolicy::setSchedulingPolicySimulated(const policy_type_t& theValue) {
        schedulingPolicySimulated = theValue;
    }

    /**
     * Returns the maximmum allowed runtime for the jobs that can be submitted to the center (or queue)
     * @return A double containing the maximum 
     */
    double SchedulingPolicy::getmaxAllowedRuntime() const {
        return maxAllowedRuntime;
    }

    /**
     * Sets the maximum allowed runtime for the jobs that are submitted to the center queue 
     * @param theValue The maximum allowed runtime for the job (emulating the queue limit)
     */
    void SchedulingPolicy::setMaxAllowedRuntime(double theValue) {
        maxAllowedRuntime = theValue;
    }

    /**
     * Returns the montery cost that an allocation unit has 
     * @return An integer conatining the cost of allocation unit
     */
    double SchedulingPolicy::getCostAllocationUnit() const {
        return CostAllocationUnit;
    }

    /**
     * Sets the cost of an allocation unit 
     * @param theValue The cost of the allocation unit.
     */
    void SchedulingPolicy::setCostAllocationUnit(double theValue) {
        CostAllocationUnit = theValue;
    }

    /**
     * Returns the allocations per our that the center charges
     * @return An integer allocations unit per our
     */
    double SchedulingPolicy::getAllocationUnitHour() const {
        return AllocationUnitHour;
    }

    /**
     * Sets the allocations per our that the center charges
     * @param theValue An integer allocations unit per our
     */
    void SchedulingPolicy::setAllocationUnitHour(double theValue) {
        AllocationUnitHour = theValue;
    }

    /**
     * Function that returns the number of cpus that are currently used by the system. It has been defined as a virtual method (note that not abstract) due to some policy may redefine its source
     * @return The number of processors that are used in the current architecture.
     */
    int SchedulingPolicy::getNumberCPUUsed() {
        return this->reservationTable->getNumberCPUsUsed();
    }

    /**
     * Returns the PowerConsumption regarding the power usage
     * @return A reference to the PowerConsumption 
     */
    PowerConsumption* SchedulingPolicy::getpowerConsumption() const {
        return power;
    }

    /**
     * Sets the PowerConsumption model to the simulation
     * @param theValue The reference to the PowerConsumption model
     */
    void SchedulingPolicy::setPowerConsumption(PowerConsumption* theValue) {
        power = theValue;
    }


    //NEXT FUNCTIONS CONCERN THE POWER CONSUMPTION !

    /**
     * This function updates the power consumption model. This method is defined as virtual (note that note pure virtual) to provide the basic power model. Perhaps some sublcasses may use a more sophisticated model.
     */
    void SchedulingPolicy::updateEnergyConsumedInPower(uint64_t globalTime, Job* job) {
        /*
          if (powerAware) 
          power->updateEnergyConsumed(this->getCPUUsage(), this->getArchitecture()->getNumberCPUs(), globalTime);
              else
              power->updateEnergyConsumedSimple(this->getCPUUsage(),this->getArchitecture()->getNumberCPUs(), globalTime);
         */
        //  if(powerAware){
        power->updateEnergyConsumedSimple(this->getCPUUsage(), this->getArchitecture()->getNumberCPUs(), globalTime, job);
        //  }

    }

    int SchedulingPolicy::getCPUUsage() {
        updateResourceUsage();
        return currentResourceUsage.getNumberCPUsUsed();
    }

    bool SchedulingPolicy::computePower() {
        return (power != NULL);
    }

    void SchedulingPolicy::initializationPower(PowerConsumption* powerinf) {
        power = powerinf;


        //RUTGERS


        /*
         Create table with one row for each NODE with pairs (timestamp, configuration_type)
         */
        assert(false);
        // TODO: No idea what this is doing. However, Node&HPCSystem do not exist anymore. 
        /*
  HPCSystem* mn = ((HPCSystemConfiguration*) this->architecture)->gethpcsystem();
  int nodes = mn->getTotalNodes();
  //int blades = mn->getTotalBlades(); //UNUSED



  for (int i=0; i<nodes; i++){

        rconf tmp;
        Node *node = mn->getNodeWithGlobalId(i);
        int type = (int) node->getNetworkBW();
        tmp.push_back(pairConfs(0, type));
        power->confs.push_back(tmp);

  }
         */

        /*
          for( int i=0; i<nodes; i++){
                rconf tmp = power->confs[i];
                pairConfs pc = tmp[0];
                int aa = 44;
          }
         */
        /*
         rconf tmp1;
         tmp1.push_back(pairConfs(1,55));
         tmp1.push_back(pairConfs(2,66));

         rconf tmp2;
         tmp2.push_back(pairConfs(11,5555));
         tmp2.push_back(pairConfs(22,6666));

         power->confs.push_back(tmp1);
         power->confs.push_back(tmp2);

         rconf tmp = power->confs[1];

         pairConfs rr = tmp[0];
         pairConfs rr2 = tmp[1];

         tmp[0] = pairConfs(7,1212);
         power->confs[0] = tmp;

         rconf tmp77 = power->confs[0];
         pairConfs tt = tmp77[0];
         pairConfs tt2 = tmp77[1];

         int a = power->confs.size();
         */



        // POSSIBLE OTHER POLICY (DISABLING SUBSYSTEMS PER BLADECENTER)

        // TO BE DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    }

    double SchedulingPolicy::getenergyConsumedFromPower() {
        return power->getenergyConsumed();
    }

    void SchedulingPolicy::setPowerAware(bool aware) {
        powerAware = aware;
    }

    bool SchedulingPolicy::getPowerAware() {
        return powerAware;
    }

    void SchedulingPolicy::increaseNumberOfReducedModeCpus(Job * job) {
        power->increaseNumberOfReducedModeCpus(job->getNumberProcessors(), job->getRunsAtFrequency());

        if (job->getJobSimisBackfilled()) {
            // save other information if job has been backfilled
            backfilledReduced++;
            backfilledReducedDemand += job->getNumberProcessors() * job->getRunTime();
        }
    }

    void SchedulingPolicy::decreaseNumberOfReducedModeCpus(Job * job) {
        power->decreaseNumberOfReducedModeCpus(job->getNumberProcessors(), job->getRunsAtFrequency());
    }

    double SchedulingPolicy::getRatio(int jobNumber, double freq) {
        if (power != NULL) return power->getScalingRatio(jobNumber, freq);
        else return 1;
    }

    void SchedulingPolicy::setPreviousTimeToPower(double time) {
        if (power != NULL)
            power->setPreviousTime(time);
    }

    void SchedulingPolicy::setFirstStartTime(double time) {
        startTime = time;
        setPreviousTimeToPower(time);
    }

    void SchedulingPolicy::dumpGlobalPowerInformation(double end_time, int usefull) {
        double energy = getenergyConsumedFromPower();
        double total_workload_time = (end_time - startTime);
        double total_workload_time_days = (end_time - startTime) / (24 * 60 * 60);
        double utilization = usefull / ((end_time - startTime) * getArchitecture()->getNumberCPUs());
        //RUTGERS
        /*
                for (int i=0; i<nodes; i++){
                        rconf tmp;
                        Node *node = mn->getNodeWithGlobalId(i);
                        int type = (int) node->getNetworkBW();
                        tmp.push_back(pairConfs(0, type));
                        power->confs.push_back(tmp);
                }
                for( int i=0; i<nodes; i++){
                        rconf tmp = power->confs[i];
                        pairConfs pc = tmp[0];
                        int aa = 44;
                }
         */

        assert(false);
        //TODO: Check why this is using HPCSystem & Node classes and substitute them with the new architecture.
        /*
        HPCSystem* mn = ((HPCSystemConfiguration*) this->architecture)->gethpcsystem();
        //int nodes = mn->getTotalNodes(); //UNUSED
        //int blades = mn->getTotalBlades(); //UNUSED
        //int oo = power->confs.size(); //UNUSED
        for( int i=0; i<(int)power->confs.size(); i++) {
                rconf tmp = power->confs[i];
                Node *node = mn->getNodeWithGlobalId(i);
                int cpus = node->getNumberCPUs();
                //int ss = tmp.size(); //UNUSED
                // Search for all entries (analyze per pairs)
                for( int j=0; j<(int)tmp.size(); j++) {
                        //pairConfs pc = tmp[j]; //UNUSED
                        double begin = tmp[j].first, end;
                        if(begin==0) begin=startTime;
                        int type = tmp[j].second;
                        if(j ==(int)tmp.size()-1) end = end_time;
                        else end = tmp[j+1].first;
                        // Common energy consumption (power=50W)
                        energy += (end-begin) * 50;
                        // Energy for each node at idle state
                        switch(type) {
                                case 1: // ALL
                                        energy += 40 * (end-begin) * cpus;
                                        energy += 80 * (end-begin);
                                        break;
                                case 2: // CPU LOW
                                        energy += 20 * (end-begin) * cpus;
                                        energy += 80 * (end-begin);
                                        break;
                                case 3: // CPU LOW, NO DISK
                                        energy += 20 * (end-begin) * cpus;
                                        energy += 40 * (end-begin);
                                        break;
                                case 4: // CPU LOW, NO DISK, NO NIC
                                        energy += 20 * (end-begin) * cpus;
                                        energy += 20 * (end-begin);
                                        break;
                                case -1: // regular case, without types...
                                        energy += 40 * (end-begin) * cpus;
                                        energy += 80 * (end-begin);
                                        break;
                                default:
                                        break;
                        }
                }
        }
         */
        std::cout << endl << "The energy consumed is:  " + ftos(energy) << endl;
        // std::cout << endl <<"The number of backfilled jobs is  "+ ftos(getBackfilledJobs())  << endl;
        std::cout << endl << "Total workolad execution time is  " + ftos(total_workload_time_days) + " days and " + ftos(total_workload_time) + " seconds" << endl;
        std::cout << endl << "Utilization is " + ftos(utilization) << endl;
        //  std::cout << endl <<"The number of jobs backfilled at lower frequency is "+ ftos(backfilledReduced)  << endl;
        //  std::cout << endl <<"Their demand is  "+ ftos(backfilledReducedDemand/((end_time-startTime)*getArchitecture()->getNumberCPUs()))  << endl;
    }

    string SchedulingPolicy::getCenterName() const {
        return CenterName;
    }

    void SchedulingPolicy::setCenterName(const string& theValue) {
        CenterName = theValue;
    }

    /**
     * Sets the sim to the SchedulingPolicy
     * @param theValue The sim
     */
    void SchedulingPolicy::setSim(Simulation* theValue) {
        sim = theValue;
    }

}

