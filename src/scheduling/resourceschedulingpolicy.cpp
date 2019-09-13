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
#include <scheduling/resourceschedulingpolicy.h>
#include <scheduling/resourcereservationtable.h>
#include <scheduling/resourcejoballocation.h>
//#include <utils/paravertrace.h>
#include <utils/trace.h>

namespace Simulator {

/**
 * The default constructor for the class
 */
ResourceSchedulingPolicy::ResourceSchedulingPolicy(): SchedulingPolicy()
{

}


/**
 * The default destructor for the class 
 * @return 
 */
ResourceSchedulingPolicy::~ResourceSchedulingPolicy()
{
}

/**
 * Release the resources used by the job. 
 * @param job The job that has to be realeased 
 * 
 * Seems to never be called anywhere
 */
void ResourceSchedulingPolicy::releaseJobResources(Job* job)
{
	ResourceJobAllocation* allocation = (ResourceJobAllocation*)
		((ResourceReservationTable*)this->reservationTable)->getJobAllocation(job);

	deque<ResourceBucket*> forParaver;
        
	/*
    carrying out the deallocation to the processors 
    it must be done externally to the deallocate policy due to this
    function may be called while the jobs is still not running !
	 */

	 //TODO: Is there something to do here? There's no CPUMapping anymore.
//	for(deque< ResourceBucket * >::iterator iter = allocation->getAllocations().begin();
//			iter != allocation->getAllocations().end(); ++iter)
//	{
//		ResourceBucket* bucket = *iter;
////		if(this->prvTrace != 0)
////			forParaver.push_back((ResourceBucket*) bucket);
//	}

//	if(this->useParaver)
//		this->prvTrace->ParaverJobExit(job->getJobNumber(),forParaver);

	/* we free the corresponding buckets*/
	this->reservationTable->deallocateJob(job);

	/* the job won't be for anymore in the system */
	map<Job*,bool>::iterator jobSysIt = this->JobInTheSystem.find(job);
	assert(jobSysIt != this->JobInTheSystem.end());
	this->JobInTheSystem.erase(jobSysIt);

}

/**
 * Find an an allocation based on the resource sellection polcy set in the simulation and based on the job requirements 
 * ALSO USED IN THE FCFS (AND WITH EASY COLLISION POLICY) SCHEDULING .. IN CASE OF MODIFICATION PLEASE UPDTATE THE fcfs STUFF
 * @param job The job that would like to be allocated 
 * @param startTime The required start time for the allocation 
 * @param runtime The required time for the allocation 
 * @see The class SchedulingPolicy 
 * @return The allocation that mathces the requirements (if there is no allocation it will be detailed in the allocation problem filed )
 */
ResourceJobAllocation* ResourceSchedulingPolicy::findAllocation(Job* job, double startTime,double runtime)
{

	//ALSO USED IN THE FCFS (AND WITH EASY COLLISION POLICY) SCHEDULING .. IN CASE OF MODIFICATION PLEASE UPDTATE THE fcfs STUFF

	//while there are problems of finding resources in a given startTime, we will iterate and incrementing the startime for the search.

	bool allocated = false;	

	while(!allocated)
	{
		ResourceJobAllocation* possibleAllocation =
				((ResourceReservationTable*)this->reservationTable)->findPossibleAllocation(job, startTime,runtime);
		
		//TODO: This has been oversimplified. Check how to go back to add different policies and act accordingly.
		if(possibleAllocation->getAllocationProblem()) {
			startTime = possibleAllocation->getNextStartTimeToExplore();
			delete possibleAllocation;
			continue;
		}
		else {
			/* If there was no problem, we found a superset with feasible allocations.
			** We now need to find an actual allocation according to the scheduling policy. */
			bool useFirstFit = this->getRS_policyUsed() == FIRST_FIT or this->getRS_policyUsed() == FF_AND_FCF;
			if(not useFirstFit) {
				std::cout << "RS policy Used: " << this->getRS_policyUsed() << std::endl;
			}
			assert(useFirstFit);
			
			allocated = true;
			return possibleAllocation;
		}
	}
	
	return NULL;

}
}
