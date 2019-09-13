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
#ifndef SIMULATIONPOWERCONSUMPTION_H
#define SIMULATIONPOWERCONSUMPTION_H

#include <map>
#include <vector>


#include <scheduling/job.h>

using std::map; 
using namespace std;


typedef pair<double, int> pairConfs;
typedef vector<pairConfs> rconf;

namespace Simulator {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class PowerConsumption{
public:
    PowerConsumption();
    PowerConsumption(double act, double por);

    ~PowerConsumption();

     void updateEnergyConsumedSimple(int active, int total, uint64_t globalTime, Job* job);
     void updateEnergyConsumed(int active, int total, uint64_t globalTime);
     double computeSaving();  
     double gettopFrequency();
     double getpreviousTime();
     void setPreviousTime(double time);
     double getenergyConsumed();
     void setEnergyConsumed(double energy);
     double getScalingRatio(int jobNumber, double freq);
     double getBeta(int jobNumber);
     void increaseNumberOfReducedModeCpus(int number, double freq);
     void decreaseNumberOfReducedModeCpus(int number, double freq);
     void setGears(vector<double> freq, vector<double> volt); 

     vector<rconf> confs; 


private:
     double Activity; /* it presents the ratio of the running and idle activities */
     double Portion; /* the portion of static in the total CPU power      */
     double alpha; /* a parameter from the the power model, Pstatic=alpha*V  */
     double topFrequency;  /* we need to know which is the top frequency */
     double previousTime;   
     double energyConsumed;
      map<int, double> betas;   /* beta for one job is randomly determined, so it has to be stored in order to have always same beta for one job     */
      map<double,double> gears; 
      map<double,int> numberCpus; /* the first member presents a frequency and the second is the number of processors that run at that frequency  */


};

}

#endif
