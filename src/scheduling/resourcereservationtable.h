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
#ifndef RESOURCERESERVATIONTABLE_H
#define RESOURCERESERVATIONTABLE_H

#include <scheduling/reservationtable.h>
#include <scheduling/metric.h>
#include <scheduling/resourcebucket.h>

#include <set>
#include <vector>
#include <list>
#include <assert.h>

#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval.hpp>


using std::vector;
using std::set;
using std::list;

//Forward declarations
namespace Utils {
    class ArchitectureConfiguration;
    class Log;
}

namespace ArchDataModel {
    class Node;
}

namespace Simulator {

    class SchedulingPolicy;
    class ResourceJobAllocation;
    class Job;

    /** This struct implements the order function for the set of buckets ordered by the startime of the buckets.
     * The buckets with the lowest startTime goest first.
     * In case of same startime the buckets are compared with their cpu,
     * and in case of the same cpu the buckets are compared with their id.*/
    struct ABucket_StartTime_lt_t {

        bool operator()(ResourceBucket* b1, ResourceBucket* b2) const {

            double x = b1->getStartTime();
            double y = b2->getStartTime();

            assert(x >= 0 && y >= 0);

            if (x != y)
                return x < y; //Lower startTime goes first
            else
                return b1->getID() < b2->getID();
        }
    };


    /** Defines a BucketSetOrdered that is a set of ResourceBuckets ordered with the startime */
    typedef set<ResourceBucket*, ABucket_StartTime_lt_t> BucketSetOrdered;
    /** Defines a pair of job and job allocation */
    typedef pair<Job*, JobAllocation*> pairJobAlloc;
    /** Defines a pair of job and ResourceBucket */
    typedef pair<ResourceBucket*, bool> pairBucketBool;
    /** Defines a matrix */
    typedef std::vector< std::vector<double> > matrix;

    /** Defines a mapping from Job's to Resources they are allocated to.*/
    typedef map<int, ResourceJobAllocation*> mapResJobAlloc;


    /**
     * @author Daniel Rivas, daniel.rivas@bsc.es
     */

    /**  
     * This function implements a Resource Reservation Table. This will be used by the scheduler for map jobs in the cpus along the time. 
     * IMPORTANT ! ALWAYS THAT THE STARTTIME FOR A GIVEN BUCKET CHANGES IT MUST BE ERASED AND REINSERTED IN THE LIST 
     * IF NOT , THE STRIC ORDER OF THE FOLLOWING FUNCTION MAY LOSE ITS FUNCTIONALITY 
     *  
     * USE void updateStartTimeFreeBucket(ResourceBucket* bucket,double startTime) for update safely the bucket startime for an already inserted bucket
     */
    class ResourceReservationTable : public ReservationTable {
    public:

        /* Constructors and desctructors*/
        ResourceReservationTable(std::vector<ArchDataModel::Node*> nodes, Utils::Log* log, uint64_t globalTime);
        ResourceReservationTable(Utils::Log* log, uint64_t globalTime);
        ResourceReservationTable();
        ~ResourceReservationTable();

        void addNode(ArchDataModel::Node* node);
        void removeNode(ArchDataModel::Node* node);

        /* auxiliar functions mainly used by the deepsearch policy*/

        void allocateBucket(double startTime, double endTime);

        /* Main and auxiliar methods */
        bool allocateJob(Job* job, ResourceJobAllocation* allocation); /* Inherited from reservation table - see there */
        bool deallocateJob(Job* job); /* Inherited from reservation table - see there */
        bool killJob(Job* job);
        bool extendRuntime(Job* job, double length);
        bool reduceRuntime(Job* job, double length);
        uint32_t getNumberCPUsUsed();

        /* Malleability */
        ResourceJobAllocation* findPossibleExtraAllocation(Job* job, double time, double length);
        bool expandJobAllocation(Job* job, JobAllocation* jobAlloc);
        bool shrinkJobAllocation(Job* job, int scpus);

        BucketSetOrdered::iterator findLowerBound(BucketSetOrdered* bucketSet, ResourceBucket* current);
        BucketSetOrdered::iterator findUpperBound(BucketSetOrdered* bucketSet, ResourceBucket* current);


        /* auxuliar methods */
        BucketSetOrdered* findFirstBucketCpus(double time, double length);
        ResourceBucket* findFirstBucketProcessor(double time, double length, int processor);

        /* The following methods are the once we used in the past  
           The first once is based on an iterative method that may be faster but do not explore deep in to the search space (it discarts some possible allocations). It may be more costly if this function it's called many times in order to find the solution. (Two different variants for this method).*/
        ResourceJobAllocation* findFastConsecutiveMethodAllocation(BucketSetOrdered* suitableBuckets, double length, int numCpus, double initialShadow);
        ResourceJobAllocation* findFastMethodAllocation(BucketSetOrdered* suitableBuckets, double length, int numCpus, double initialShadow);
        /* - The second on it's a litle bit more smart. In the sense it construct a matrixs with meta data and explores all the possible results in a smarter way. It creates a meta data. */
        ResourceJobAllocation* findSmartMethodAllocation(BucketSetOrdered* suitableBuckets, double length, int numCpus, double initialShadow);


        bool shareEnoughTime(ResourceBucket* bucket1, ResourceBucket* bucket2, double length);
        void freeAllocation(ResourceJobAllocation* allocationForCheckingReq);

        vector<BucketSetOrdered*>* getBuckets();

        ResourceJobAllocation* findPossibleAllocation(Job* job, double time, double length, std::vector<uint32_t> pnodes = {});
        ResourceBucket* findFirstBucket(double time, double runtime, int bId);


        void dumpAllBucketSetsAndCheck(BucketSetOrdered* allocations, BucketSetOrdered* globalView, BucketSetOrdered* buckets);



    protected:

        void dumpBucketSet(BucketSetOrdered* setBuckets);
        void checkBucketSet(BucketSetOrdered* setBuckets);
        void printBucketSetOrdered(BucketSetOrdered* bs, uint32_t id, uint32_t setid);

        void updateStartTimeFreeBucket(ResourceBucket* bucket, double startTime); /**< function that updates safely a bucket already inserted in the local sets - avoids violate the strict order of the set*/

        vector<BucketSetOrdered*> buckets; /**< the resource reservation table. A BucketSetOrdered per Resource. */

    };

}

#endif
