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
#include <scheduling/resourcereservationtable.h>
#include <utils/architectureconfiguration.h>
#include <utils/log.h>
#include <utils/utilities.h>
#include <scheduling/resourcejoballocation.h>
#include <scheduling/schedulingpolicy.h>
#include <scheduling/simulation.h>
#include <archdatamodel/node.h>

#include <deque>
#include <algorithm>
#include <climits>

#include "systemorchestrator.h"

using namespace std;

extern Simulator::Simulation * simulator;

namespace Simulator {

    /**
     * The default constructor for the class
     */
    ResourceReservationTable::ResourceReservationTable() {
        cout << "DefaultRRTContructor" << endl;
    }

    /**
     * The default destructor for the class
     */
    ResourceReservationTable::~ResourceReservationTable() {



        //we free all the free buckets
        for (vector<BucketSetOrdered*>::iterator it = this->buckets.begin(); it != this->buckets.end(); ++it) {
            for (BucketSetOrdered::iterator deb = (*it)->begin(); deb != (*it)->end(); ++deb)
                delete *deb;
            delete *it;
        }

        //now we free all the job allocations if present
        for (map<Job*, ResourceJobAllocation*>::iterator it = JobAllocationsMapping.begin(); it != JobAllocationsMapping.end(); ++it)
            delete it->second;
    }

    /**
     *  This is the constructor for the class.
     *  BE AWARE ! IF SOMETHING CHANGES CHECK THAT THE operator= OF THE VIRTUAL RESERVATION TABLE IS NOT BEING AFECTED
     *  MAINLY REFERENCES
     *
     * @param configuration A reference to the configuration concerning the architecture
     * @param log A reference to the logging engine
     * @param globalTime The globalTime of the simulation
     */
    ResourceReservationTable::ResourceReservationTable(std::vector<ArchDataModel::Node*> nodes, Utils::Log* log, uint64_t globalTime) : ReservationTable(log, globalTime) {
        /* creating the sets  */
        for (uint32_t i = 0; i < nodes.size(); i++) {
            /* the free buckets */

            ResourceBucket* freeBucket = new ResourceBucket(nodes[i], globalTime, -1);
            freeBucket->setID(i);
            BucketSetOrdered* freeBuckets = new BucketSetOrdered();
            freeBuckets->insert(freeBucket);
            this->buckets.push_back(freeBuckets); //Add to buckets
            this->log->debug("The size of the freeBuckets is " + to_string(freeBuckets->size()) + " the buckets size is " + to_string(this->buckets.size()), 6);
        }
    }

    ResourceReservationTable::ResourceReservationTable(Log* log, uint64_t globalTime) : ReservationTable(log, globalTime) {
        // Does this ever get called ???
    }

    void ResourceReservationTable::addNode(ArchDataModel::Node *node) {
        //        cout << __PRETTY_FUNCTION__ << endl;
        ResourceBucket *freeBucket = new ResourceBucket(node, globalTime, -1);
        freeBucket->setID(buckets.size()); //TODO: Check if it'd be better to use node's ID.
        BucketSetOrdered* freeBuckets = new BucketSetOrdered();

        freeBuckets->insert(freeBucket);

        this->buckets.push_back(freeBuckets);
//        this->log->debug("Added a new bucket to the list of buckets.");

        //        cout << "FBSize: " << buckets.size() << endl;
    }

    //TODO: In order to remove nodes from the reservation table, we first need to change buckets vector->set/map and order by nodeid.

    void ResourceReservationTable::removeNode(ArchDataModel::Node *node) {

    }

    /**
     * Methodology: Find the bucket that goes >= and then go one step back
     * @param bucketSet A reference to the BucketSetOrdered with a set of buckets.
     * @param current A reference to the reference bucket
     * @return Returns an iterator to the last bucket (in the ordered set) that has a lower or equal startime that the reference bucket
     */
    BucketSetOrdered::iterator ResourceReservationTable::findLowerBound(BucketSetOrdered* bucketSet, ResourceBucket* current) {
        //    cout << __PRETTY_FUNCTION__ << endl;
        //    for (BucketSetOrdered::iterator bicket = bucketSet->begin();bicket != bucketSet->end(); ++bicket){
        //        cout << "BICKET start = " << (*bicket)->getStartTime() << "  end= " << (*bicket)->getEndTime() << endl;
        //    }

        BucketSetOrdered::iterator itfreeb = bucketSet->lower_bound(current); //Returns >= buckets
        if (itfreeb == bucketSet->end()) {
            itfreeb--;
            return itfreeb;
        }
        if ((*itfreeb)->getStartTime() == current->getStartTime())
            return itfreeb;
        else if (itfreeb == bucketSet->begin())
            assert(false); //shouldn't happen. There's alwayas a bucket with startTime 0.

        --itfreeb; // If it's current or not this works
        return itfreeb;
    }

    /**
     * This functions returns the bucket that goes > in the set compared with current
     * @param bucketSet A reference to the BucketSetOrdered with a set of buckets.
     * @param current A reference to the reference bucket
     * @return Returns an iterator to the first bucket (in the ordered set) that has the startTime bigger than the reference's bucket endTime.
     */
    BucketSetOrdered::iterator ResourceReservationTable::findUpperBound(BucketSetOrdered* bucketSet, ResourceBucket* current) {
        BucketSetOrdered::iterator itfreeb = bucketSet->upper_bound(current); //Returns > buckets
        if (itfreeb == bucketSet->end())
            assert(false); //shouldn't happen but could happen. Left here for debug in case it happens.

        while (current->getEndTime() > (*itfreeb)->getStartTime() and (*itfreeb)->getEndTime() != -1) {
            itfreeb++;
            assert(itfreeb != bucketSet->end()); //shouldn't happen because last bucket's endTime is always -1 yet left just in case.
        }
        return itfreeb;
    }

    /**
     * Inherited from reservation table
     * @see the reservation table class
     * BE AWARE !!! IF A BUG IS  IN THIS METHOD PLEASE FIX THE SAME BUG AT THE CUTBUCKETS METHOD OF THE
     * VIRTUAL RESERVATION TABLE !!!
     * @param job The job to be allocated to the reservation table
     * @param genericallocation The job allocation that has to be allocated
     * @return True is the job has been correctly allocated
     */
    bool ResourceReservationTable::allocateJob(Job* job, ResourceJobAllocation* jobAlloc) {
        /*
         * Setup:
         * Get a pointer to the allocations (ie resources on nodes) that need to be placed into the RRT
         * Note, it has been already determined (via the Scheduling Policy) that the Job should be placed
         * Also, what is passed in to this function when call from fsfc->allocate() is a ResourceJobAlloc, so the getAllocations() methods work
         * But, from reading this, you'd assume it doesn't as getAllocations would return an empty.
         */
        deque<ResourceBucket*> allocations = jobAlloc->getAllocations();
        
        /*
         * Place (allocate) the job to the reservation table
         */

        deque<ResourceBucket*>::iterator alloc_it;

        log->debug("[ResourceReservationTable] allocating job " + to_string(job->getJobNumber()) + " of size " + to_string(allocations.size()), 6);
        //        cout << "[ResourceReservationTable] allocating job " << to_string(job->getJobNumber()) << " of size " << allocations.size() << endl;



        assert(allocations.size() > 0);
        


        /*
         * Basic procedure:
         * 1. iterate over the allocation (the desired), tackling one nodes-worth at a time, call this "current"
         * 2. iterate over the buckets for the node indicated in current and split buckets as necessary to place the job on that node
         * 3. rinse and repeat
         */
        for (alloc_it = allocations.begin(); alloc_it != allocations.end(); ++alloc_it) {

            ResourceBucket* current = *alloc_it;
            //int bId = current->getID();
            uint32_t bId = current->getID();
            


            /* The buckets in the jobAllocation have all the same startTime and endTime.
             ** The buckets in the jobAllocation have to be inserted in the buckets list of the
             ** reservation table. The buckets in JobAllocation are not actually inserted. New objects are created instead.
             ** Some buckets might have to be broken down.
             ** POSSIBLE CASES:
             ** 1. current fits exactly in an existing bucket => Allocate job.
             ** 2. current starts between two buckets => Shorten the two existing buckets
             ** and add current in the middle. Allocate job in the other two to substract resources.
             */

            /* DEBUG*/
            // if(log->getLevel() >= 6)
            // dumpAllBucketSetsAndCheck(allocations[cpuCurrent],globalView[cpuCurrent],buckets[cpuCurrent]);

            /* before do any stuff with the buckets we will find the lower bucket */
            BucketSetOrdered::iterator bucket_it = this->findLowerBound(buckets[bId], current);
            ResourceBucket* prevBucket = *bucket_it;

            //When allocating a job only two buckets might need to be broken down: the first and the last. ONly if the new falls in the middle of those.
            /*
             * When allocating a job, you have to divide buckets which:
             * a. have current's starttime in them
             * b. have current's endtime in them
             * 
             * Further, if there are buckets in the middle, from some previous bucket alterations, the job needs allocated to those too
             */

            ReservationList rl = current->getResourceList();

            /*
             * Current (ie what I want to use to run my job, will take up the resource in prevBucket.
             * prevBucket is cloned to newBucket
             * prevBucket then has it's starttime set to be what I need
             * newBucket has it's endTime set to be prevBucket's starttime - 1
             * thus:
             * | prevbucket               | (before)
             * | newBucket  | prevBucket  | (after)
             *              | current     | (reference, current is never actually used, just copied)
             */
            if (prevBucket->getStartTime() < current->getStartTime()) {
                ResourceBucket *newBucket = new ResourceBucket(*prevBucket);
                prevBucket->setStartTime(current->getStartTime());
                //prevBucket->allocateJob(job, rl);
                // Why don't we allocate the job in prevbucket too?
                // That seems to be the logical thing.
                newBucket->setEndTime(current->getStartTime() - 1);
                //newBucket->setID(prevBucket->getID());
                buckets[bId]->insert(newBucket);
            }


            /*
             * keep_alloc is true, iff prevBucket has an endtime leq when I need my job (as defined in current) to finish AND prevBucket wasn't the last in the BucketSetOrdered
             * ie, wasn't the final bucket with infinite end time
             * Using the above picture, keep_alloc would be True if something came after prevBucket in the BucketSetOrdered
             */
            bool keep_alloc = (prevBucket->getEndTime() <= current->getEndTime() and (prevBucket->getEndTime() != -1));
            //            if (keep_alloc) {
            //                cout << "KEEP_ALLOC" << endl;
            //                printBucketSetOrdered(buckets[bId], 0, bId);
            //            }


            /*
             * What points where - a guide for confused people...
             * current --> a badly named iterator to a bucketsetordered which represents the _desired_ resources for "job"
             * bucket_it --> an iterator which initially points to the bucket in a bucketsetordered which has starttime prior the first bucket of current
             * prevBucket --> the initial bucket pointed to by "bucket_it"
             */

            /*
             * The first 3 lines are almost repeats
             */
            while (keep_alloc) {
                if ((*bucket_it)->getEndTime() == -1) break; // break if it points to a bucket which has an infinite endtime, ie is the last in the bucketsetordered
                assert(bucket_it != buckets[bId]->end()); // check the bucket itself is not (one past) the last bucket in the BucketSetOrdered (it's the usual C++ one past the end iterator check)
                assert((*bucket_it)->getEndTime() != -1); // check that the endtime of the bucket is not "infinite"
                /*
                 * The above seems a repeat, but, the first time, we just break from the while loop, ie
                 * end the loop. Here we are asserting we have broken free if we had an infinite endtime.
                 */


                (*bucket_it)->allocateJob(job, rl); // Map the desired resources of the job onto the bucket
                /*
                 * This fills up the resources of this bucket with that of the job
                 */

                if ((*bucket_it)->getEndTime() == current->getEndTime()) break; //We know we reached the last bucket to allocate in the horizontal sense. Ie, working our way along the timeline.
                bucket_it++; // Step further along the timeline
                /*
                 * It's incrementing along the time axis, such that we make sure we allocate the resources we need (given by current) to all buckets between
                 * the one which had to get changed to accommodate the startime of current and the one which will need changed to accommodate the endtime of current.
                 * This is a "horizontal" traversal
                 */

                //                keep_alloc = ((*bucket_it)->getEndTime() <= current->getEndTime() and (prevBucket->getEndTime() != -1));
                keep_alloc = ((*bucket_it)->getEndTime() <= current->getEndTime() and ((*bucket_it)->getEndTime() != -1));
                /*
                 * Terminator condition, it we've not yet reached a bucket which has an endtime which is further
                 * into the future than current requires.
                 */
            }

            /*
             * What points where, an updated guide for confused idiots like nickj
             * current still points to the allocation, ie resource that we want to reserve for our job(s)
             * it now points to a bucket which exceeds the endtime of current, ie has a endtime gt current.endtime
             * that might be infinite.
             *
             */

            /*
             * If the bucket we are looking at (assume prevBucket for simplicity) has an end time after what we need or extends to infinity
             * Assume we have dealt correctly with the reshuffling at the start of "current" to make that work out
             * AND assume we have dealt with any buckets in the middle
             * clone (it) into newBucket and set the endtime of newBucket to match current.endtime
             * modify starttime of "it" to be current.endtime+1
             *
             * thus:
             * | (*it)                    | (before)
             * | newBucket   | (*it)      | (after)
             * | current     | (reference, current is never actually used, just copied)                 *
             */
            if ((*bucket_it)->getEndTime() > current->getEndTime() or (*bucket_it)->getEndTime() == -1) { //need to break the bucket by the end
                ResourceBucket *newBucket = new ResourceBucket(*(*bucket_it));
                newBucket->setEndTime(current->getEndTime());
                //newBucket->setID((*bucket_it)->getID());
                assert(newBucket->getEndTime() != -1);
                newBucket->allocateJob(job, rl);
                (*bucket_it)->setStartTime(current->getEndTime() + 1);

                buckets[bId]->insert(bucket_it, newBucket);
            }
            // printBucketSetOrdered(buckets[bId], 0, bId);


            /*
             * Question - how do we iterate over allocations?
             * We remapped it to iterate over something else
             * Can we dump it to cout and examine?
             * The answer you've all been searching for
             * ... scoping rules. Scoping flippin' rules.
             * After the next line, we jump to the top of the loop and it takes on the next value in it's
             * sequence. Ie, it points to the next item in allocations, it the next node's worth of resource you need to deal with
             * For the sake of a bit of naming, this would have been much easier.
             * And never abuse scoping for a quick win.
             */

            //            cout << "ALLOC_IT: end of loop value   " << *alloc_it << endl;
        }
        //for sanity
        //        cout << "IT:: out of loop value   " << *alloc_it << endl;
        map<Job*, ResourceJobAllocation*>::iterator itcheck = this->JobAllocationsMapping.find(job);
        assert(itcheck == this->JobAllocationsMapping.end()); //check that the job is not yet allocated.

        //adding the mapping for the job allocation .. then we will be able to kill or finish it
        this->JobAllocationsMapping.insert(std::pair<Job *, ResourceJobAllocation*>(job, jobAlloc));
        

        return true;
    }

    void ResourceReservationTable::printBucketSetOrdered(BucketSetOrdered *bs, uint32_t id, uint32_t setid) {

        BucketSetOrdered::iterator it = bs->begin();
        std::cout << "----------- [" << setid << "] Print id: " << id << " - " << (void*) bs << " -------------" << std::endl;
        for (; it != bs->end(); it++) {
            std::cout << (void*) *it << ": " << (*it)->getStartTime() << " - " << (*it)->getEndTime() << "( ";
            std::map<Job*, ReservationList> reservations = (*it)->getJobReservations();
            std::map<Job*, ReservationList>::iterator it_res = reservations.begin();
            for (; it_res != reservations.end(); it_res++) {
                std::cout << it_res->first->getJobNumber() << " ";
            }
            std::cout << ")" << std::endl;
        }
        std::cout << "--------------- end printing ----------------" << std::endl;
    }

    /**
     * function created for debugging, dumps the current buckets in a set of buckets
     * @param setBuckets The set of buckets to be dump
     */
    void ResourceReservationTable::dumpBucketSet(BucketSetOrdered* setBuckets) {
        if (log->getLevel() >= 6) {
            log->debug("The size of the buckets for the cpu is " + to_string(setBuckets->size()), 6);
            for (BucketSetOrdered::iterator deb = setBuckets->begin(); deb != setBuckets->end(); ++deb) {
                ResourceBucket* debBuck = *deb;
                log->debug("Bucked id " + to_string(debBuck->getID()) + " statTime " + ftos(debBuck->getStartTime()) + " endTime " + ftos(debBuck->getEndTime()), 6);
            }
        }
    }

    /**
     * for asserting that everything is fine in the reservation table. it basically checks that no free buckets are continuous (due to in this case the buckets should be joint) and that two continuous buckets are really continuous (what means that the endTime for the previous+1 is the same as the startime for the next)
     * @param setBuckets  The set of buckets to check.
     */

    void ResourceReservationTable::checkBucketSet(BucketSetOrdered* setBuckets) {
        //TODO: Redo with new buckets.
    }

    /**
     * This function reduces the runtime of a given job that has been allocated in the reservation table, this is used in situations where a given job was suppoed to run X and the scheduler has realized that it has run X-alpha, and due to the scheduler algorithm the reservationtable has to be updated
     * @param job The job whom allocation has to be updated
     * @param length The lenght that has to be reduced
     * @return True if no problems have raised
     */

    bool ResourceReservationTable::reduceRuntime(Job* job, double length) {
        assert(false);
        //TODO: Not being used but will be needed for malleability. Update with the new architecture.

        // map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);
        // ResourceJobAllocation* allocation = (ResourceJobAllocation*) allocIter->second;

        // assert(allocation->getEndTime()-length > 0);
        // allocation->setEndTime(allocation->getEndTime()-length);

        // assert(allocation != 0);
        // assert(length > 0);

        // for(deque<ResourceBucket*>::iterator it = allocation->getAllocations().begin();
        // it != allocation->getAllocations().end();++it)
        // {
        // ResourceBucket* current = *it;


        // /* for sanity*/
        // assert(current != 0);
        // assert(current->getJob()->getJobNumber() == job->getJobNumber());
        // assert(current->getEndTime()-current->getStartTime()-length > 0); //the job allocation can not be reduced till zero

        // /* we find the upper bucket to check if it can be joint to the slot of time that its is not used
        // or we must to create a new free bucket and add it to the freeBuckets
        // */
        // ResourceBucket* freeUpperBucket = this->findUpperBound(this->buckets[current->getCpu()],current);


        // if(current->getEndTime()+1 == freeUpperBucket->getStartTime())
        // the freed slot of time can be joined to the next free bucket
        // freeUpperBucket->setStartTime(freeUpperBucket->getStartTime()-length);
        // else
        // {
        // the slot can not be joined ot the next free bucket .. so we must create a new one
        // and add it to the free buckets
        // ResourceBucket* newBucket = new ResourceBucket(current->getEndTime()-length+1,current->getEndTime());
        // newBucket->setUsed(false);
        // newBucket->setId(++this->lastBucketId);
        // newBucket->setCpu(current->getCpu());

        // updating the free buckets structure and the globalview
        // buckets[current->getCpu()]->insert(newBucket);
        // globalView[current->getCpu()]->insert(newBucket);

        // }

        // current->setEndTime(current->getEndTime()-length);

        // }


        return true;

    }
    /*


     */

    /**
     * This function extends the runtime of an allocation of a given with the specified length
     * in this case a precondition is that there are no used buckets after the current bucket  this function must be called
     * @param job The job to whom the job runtime must be extended
     * @param length The lenght of the extension
     * @return True if the extension has not raised any problem
     */

    bool ResourceReservationTable::extendRuntime(Job* job, double length) {
        assert(false);
        //TODO: extendRuntime not being used but will be needed for malleability. Update with new architecure.
        // //we get the allocation and free it from the JobAllocationMapping
        // map<Job*,JobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);
        // ResourceJobAllocation* allocation = (ResourceJobAllocation*) allocIter->second;

        // assert(allocation != 0);


        // for(deque<ResourceBucket*>::iterator it = allocation->allocations.begin();
        // it != allocation->allocations.end();++it)
        // {
        // ResourceBucket* current = *it;


        // /* for sanity*/
        // assert(current != 0);
        // assert(current->getJob()->getJobNumber() == job->getJobNumber());

        // ResourceBucket* freeUpperBucket = this->findUpperBound(this->buckets[current->getCpu()],current);

        // assert(freeUpperBucket->getStartTime()-1 == current->getEndTime() &&
        // (freeUpperBucket->getEndTime() > freeUpperBucket->getStartTime()+length || freeUpperBucket->getEndTime() == -1)
        // );

        // freeUpperBucket->setStartTime(freeUpperBucket->getStartTime()+length);
        // current->setEndTime(current->getEndTime()+length);

        // }

        //now its time to update the allocation
        // allocation->setEndTime(allocation->getEndTime()+length);

        return true;
    }

    /**
     * this function deallocates a given job, that has been allocated to the reservation table. The allocation can be for a job that is running or for a job that is still queued in the queue waitting for its startime.
     * @param job The job to deallocate
     * @return True if the job has succesfully deallocated
     */

    bool ResourceReservationTable::deallocateJob(Job* job) {

//        log->debug("RRT DEALLOC JOB " + to_string(job->getJobNumber()), 4);
//        cout << "DEALLOC START JOB " << job->getJobNumber() << endl;
//        for (int n=0; n<4; n++){
//            printBucketSetOrdered(buckets[n], n, 0);
//        }

        //log->debug("[ResourceReservationTable] deallocating job " + to_string(job->getJobNumber()),6);
        map<Job*, ResourceJobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);

        //This used to be an assert, but sometimes the job is trying to be deallocated before being allocated with the new iterative scheduling.
        if (allocIter == this->JobAllocationsMapping.end())
            return false;
        ResourceJobAllocation* allocation = (ResourceJobAllocation*) allocIter->second;
        this->JobAllocationsMapping.erase(allocIter);

        assert(allocation != 0);


        /*
     the steps to do are the following:
     1- Free the buckets used by this job in each cpu in the allocations buckets
     2- Free the same buckets from the global view
     3- Add the buckets to the free area and update the global view
         */

        deque<ResourceBucket*> allocs = allocation->getAllocations();
        for (deque< ResourceBucket * >::iterator it = allocs.begin();
                it != allocs.end(); ++it) {
            ResourceBucket* currentAlloc = *it; //Current does not correspond to the bucket in the RTand it is only a hint for where to start looking to deallocate.

            int bId = currentAlloc->getID();
            BucketSetOrdered::iterator itlow = findLowerBound(buckets[bId], currentAlloc);
            BucketSetOrdered::iterator itup = findUpperBound(buckets[bId], currentAlloc);

            ResourceBucket* deallocBucket = *itlow;
            assert(deallocBucket->deallocateJob(job));

            //Job may be present in several buckets if the initial was broken down.
            //deallocate forward until the job is not present in the bucket.
            BucketSetOrdered::iterator itnext = itlow;
            itnext++;
            while (itnext != buckets[bId]->end()) {
                if (not (*itnext)->deallocateJob(job))
                    break;
                itnext++;
            }

            //Once the job's been deallocated from every bucket, the previous and the next (counting from currentAlloc)
            //are susceptible to be joined in case the only difference between them was just the deallocated job.

            if (itlow != buckets[bId]->rend().base())
                itlow--;
            //assert(itlow != buckets[bId]->rend().base()); //shouldn't happen since no job starts at time 0 and there's a bucket which does.
            ResourceBucket* prevBucket = *itlow;
            ResourceBucket* nextBucket = *itup; //next bucket after currentAlloc finishes.
            ResourceBucket* nextPrevBucket = *(--itup); //Previous bucket to nextBucket. Could be dealloc but the same applies.

            bool joinPrev = (*prevBucket == *deallocBucket && prevBucket != deallocBucket);
            bool joinEnd = (*nextPrevBucket == *nextBucket);

            if (joinPrev) {
                assert(prevBucket != deallocBucket);
                prevBucket->setEndTime(deallocBucket->getEndTime());
            }
            if (joinEnd) {
                assert(nextBucket != deallocBucket); //It doesn't make sense to have this assert this this can happen.
                if (nextPrevBucket == deallocBucket and joinPrev) { //we're actually joining prev and next buckets.
                    prevBucket->setEndTime(nextBucket->getEndTime());
                } else {
                    nextPrevBucket->setEndTime(nextBucket->getEndTime());
                }
                buckets[bId]->erase(nextBucket);
            }

            if (joinPrev) buckets[bId]->erase(deallocBucket);

        }
        //Free the allocation , if other classes are using it they should copy in its own space
        delete allocation;

//        cout << "DEALLOC END JOB " << job->getJobNumber() << endl;
//        for (int n=0; n<4; n++){
//            printBucketSetOrdered(buckets[n], n, 0);
//        }
        
        
        return true;
    }

    void ResourceReservationTable::dumpAllBucketSetsAndCheck(BucketSetOrdered* cpuAllocations, BucketSetOrdered* cpuGlobalView, BucketSetOrdered* cpuBuckets) {
        if (log->getLevel() >= 6) {
            log->debug("dumpAllBucketSetsAndCheck\n-------------------\n");
            log->debug("dumpBucketSet(allocations):", 6);
            dumpBucketSet(cpuAllocations);
            log->debug("dumpBucketSet(globalView):", 6);
            dumpBucketSet(cpuGlobalView);
            log->debug("dumpBucketSet(buckets):", 6);
            dumpBucketSet(cpuBuckets);
            checkBucketSet(cpuGlobalView);
        }
    }

    /**
     * If a job has been killed this function may be invoqued. Actually it is not used but defined.
     * @param job The job that has been killed
     * @return True if no problems hava rised
     */
    bool ResourceReservationTable::killJob(Job* job) {
        /* maybe with deallocate is enough?*/
        return true; //DUMMY RETURN
    }

    
    /**
     * This function returns a BucketSetOrdered that contains the first freeBucket
     * available at startTime whose length >= minLength for each Cpu.
     *
     * @param startTime The start time from when the job has to start to explore
     * @param minLength The minimum length required for the selected buckets
     * @return The set of buckets that matches the criteria
     */
    ResourceJobAllocation* ResourceReservationTable::findPossibleAllocation(Job* job, double time, double length, std::vector<uint32_t> pnodes) {
        ResourceJobAllocation *allocation = new ResourceJobAllocation();
        allocation->setStartTime(time);
        allocation->setEndTime(time + length);
        std::deque<ResourceBucket*> possibleBuckets;

        log->debug("Finding Possible Allocation for job " + to_string(job->getJobNumber()) + " which is preceeded by [" + vtos(job->getPreceedingJobs()) + "].", 2);

        //First fill the job requirements.
        ResourceRequirements rr = {
            .cpus = (uint32_t)job->getNumberProcessors(),
            .cpusPerNode = (job->getCPUsPerNode() == -1) ? 0 : (uint32_t)job->getCPUsPerNode(),
            .mem = (job->getRequestedMemory() == -1) ? 0 : (uint64_t)job->getRequestedMemory() * (uint32_t)job->getNumberProcessors(),
            .memPerNode = (job->getMemPerNode() == -1) ? 0 : (uint32_t)job->getMemPerNode(),
            .disk = (job->getDiskUsed() == -1) ? 0 : (uint32_t)job->getDiskUsed(),
            .diskPerNode = (job->getDiskPerNode() == -1) ? 0 : (uint32_t)job->getDiskPerNode(),
            //TODO: Not used for the moment.
            .diskBW = 0.0,
            .memBW = 0.0,
            .fabBW = 0.0
        };

        //Job requirements will be compared against freeResources after the allocation is built.
        ResourceRequirements freeResources = {
            .cpus = 0, .cpusPerNode = 0, .mem = 0, .memPerNode = 0,
            .disk = 0, .diskPerNode = 0, .diskBW = 0.0, .memBW = 0.0, .fabBW = 0.0
        };

        ResourceRequirements lack = {0};

        assert(rr.diskBW == 0 and rr.memBW == 0 and rr.fabBW == 0);
        double nextStartTime = -1;

        std::vector<uint32_t> mybuckets;
        
        /*
         * If pnodes < number of requested processors, ie, a fan-out scenario, just put the jobs anywhere.
         */

        if (pnodes.size() != 0 && pnodes.size() >= (uint32_t)job->getRequestedProcessors()) {
            mybuckets = pnodes;

        } else {
            for (uint32_t i = 0; i < buckets.size(); i++) {
                ResourceBucket* current = *(buckets[i]->begin());
                // Here is the problem, some buckets, I reckon have a broken ID due to bad splitting...
                mybuckets.push_back(current->getID());
            }
        }
#ifdef BUCKETDEBUG
        for (uint32_t i = 0; i < buckets.size(); i++) {
            BucketSetOrdered* current_bs = buckets[i];
            BucketSetOrdered::iterator bsit;
            for (bsit = current_bs->begin(); bsit != current_bs->end(); bsit++) {
                ResourceBucket* currentb = (*bsit);
                log->debug("Is this a broken ID? " + to_string(currentb->getID()) + " ST: " + to_string(currentb->getStartTime()) + " ND: " + to_string(currentb->getEndTime()), 4);
            }
        }
#endif

        log->debug("Job: " + to_string(job->getJobNumber()) + "  MYBUCKETS is of size " + to_string(mybuckets.size()), 4);
        log->debug("Job: " + to_string(job->getJobNumber()) + " time=" + to_string(time) + " length=" + to_string(length), 4);

        //        for (uint32_t i = 0; i < buckets.size(); i++) {
        
        
        /*
         * This loop is initially confusing, but it works.
         * You start by locating the first bucket on node 'i' which has a start time >= the the desired time and suitable length.
         * Then you check it's not at the very start or end of the timeline, as that would require splitting etc.
         * Then you do some business logic to set the "next start time", ie the next possible time the node is free for job of this length. This updates as we iterate through the nodes ...
         * ... NextStartTime is either the time of the next release (when a bucket is free) or nextstarttime from another node.
         * Then you check to see if the current bucket has available resource. This is because you might have shared nodes, and need to consider if the current one has enough free CPU/Memory etc.
         * If OVERALL, if considering all nodes at this time, there were not enough resources available, set the flag to say so and provide "NextStartTime", ie when to explore the next time this function is run.
         * */
        
        for (uint32_t i = 0; i < mybuckets.size(); i++) {
            ResourceBucket* current = findFirstBucket(time, length, (int) mybuckets[i]);
            assert(current->getStartTime() != -1 and current->getEndTime() != -1);
            log->debug("Passed Assert for Job " + to_string(job->getJobNumber()) + " looking at node " + to_string(mybuckets[i]), 4);
            double nextRelease = current->getTimeNextRelease();
            if (nextStartTime > -1 && nextRelease > -1)
                nextStartTime = (nextStartTime < nextRelease) ? nextStartTime : nextRelease;
            else if (nextRelease > -1)
                nextStartTime = nextRelease;
            
            ArchDataModel::Node* current_node = current->getNode();
            bool rfnlazy = simulator->getSimInfo()->getarchConfiguration()->getSystemOrchestrator()->getReserveFullNode();
            
            
            
            if ((rr.cpusPerNode > current->getFreeCPUs()) or
                    (rr.memPerNode > current->getFreeMemory()) or
                    (rr.diskPerNode > current->getFreeDisk())) {
                log->debug("[Job " + to_string(job->getJobNumber()) + "] Dropping Node " + to_string(current->getID()) + " as candidate.", 4);
                delete current;
                continue;
            }

            if (current->getFreeCPUs() != current_node->getNumberCPUs() && rfnlazy==true){
                log->debug("Dropping node" + to_string(current->getID()) + " for job " + to_string(job->getJobNumber()) +" as it's being used by someone else.", 4);
                delete current;
                continue;
            }
//            if ((rr.cpusPerNode > current->getFreeCPUs()) or
//                    (rr.memPerNode > current->getFreeMemory()) or
//                    (rr.diskPerNode > current->getFreeDisk())) {
////                cout << "[Job " << job->getJobNumber() << "] Dropping Node " << current->getID() << " as candidate." << endl;
//                delete current;
//                continue;
//            }

            //Check how many CPUs I can allocate with the memory I have
            //how many CPUs can I allocate if I'm limited by free memory?
            double memPerCPU = job->getRequestedMemory();
            uint64_t maxReservableByMem = (rr.mem == 0) ? rr.cpus : current->getFreeMemory() / memPerCPU;
            uint64_t reservableCPUs = (maxReservableByMem > current->getFreeCPUs()) ? current->getFreeCPUs() : maxReservableByMem;
            freeResources.cpus += reservableCPUs;
            //how much memory can I allocate if I'm limited by free cpus?
            uint64_t reservableMemory = reservableCPUs * memPerCPU;
            freeResources.mem += reservableMemory;
            freeResources.memBW += current->getFreeMemoryBW();
            freeResources.disk += current->getFreeDisk();
            freeResources.diskBW += current->getFreeDiskBW();
            freeResources.fabBW += current->getFreeFabricBW();
            possibleBuckets.push_back(current);
//            log->debug("[Job " + to_string(job->getJobNumber()) + "] Possible Node " + to_string(current->getID()) + " as candidate.", 1);
//            cout << "[Job " << job->getJobNumber() << "] Possible Node " << current->getID() << " as candidate." << endl;

            if (current->getFreeDiskBW() != 0.0 or current->getFreeMemoryBW() != 0.0 or current->getFreeFabricBW() != 0.0) {
                std::cout << "[" << job->getJobNumber() << "] diskBW: " << current->getFreeDiskBW() << ", memBW: " << current->getFreeMemoryBW() << ", fabBW: " << current->getFreeFabricBW() << "." << std::endl;
                assert(false);
            }
        }

        if (freeResources.diskBW != 0.0 or freeResources.memBW != 0.0 or freeResources.fabBW != 0.0) {
            std::cout << "[" << job->getJobNumber() << "] diskBW: " << freeResources.diskBW << ", memBW: " << freeResources.memBW << ", fabBW: " << freeResources.fabBW << "." << std::endl;
            assert(false);
        }


        allocation->setNextStartTimeToExplore(nextStartTime);
        //Check if the requirements were met.
        if (freeResources < rr) {
            allocation->setAllocations(possibleBuckets);
            allocation->setAllocationProblem(true);
            allocation->setAllocationReason(NOT_ENOUGH_RESOURCES);
            lack.cpus = rr.cpus - freeResources.cpus;
            allocation->setAllocationLack(lack);
            log->debug("Requirements could not be met for job " + to_string(job->getJobNumber()), 2);
        } else {
            log->debug("Requirements were met for job " + to_string(job->getJobNumber()), 2);
            allocation->setAllocations(possibleBuckets);
        }
        log->debug("Finished allocate", 2);
        return allocation;

    }

    /**
     * This function  returns a pointer to the first available bucket whom length is bigger or equal
     * than the provided and that starts after the date time in the specified processor.
     *
     * @param time The start time from when the job has to start to explore
     * @param minLength The minimum length required for the selected buckets
     * @param processor The processors where the bucket has to be found
     * @return The set of buckets that matches the criteria
     */
    ResourceBucket* ResourceReservationTable::findFirstBucket(double time, double runtime, int bId) {

        /*
         * Starting the search based on the o time, coz may be the more suitable bucket starts at the past
         * for example if we are in the time 3 we may find the bucket 150 -1 that is the first bucket that matches the lower_bound propierty, but we could used the bucket 0..120
         */


        //Create a test bucket only to find the relevant one.
        ResourceBucket* testBucket = new ResourceBucket(time, -1);

        assert(time >= 0);
        BucketSetOrdered::iterator itfreeb = this->findLowerBound(buckets[bId], testBucket);
        ResourceBucket* prevBucket = *itfreeb;
        //Now check if the previous bucket is still in the interval.
        //else should not happen since first bucket's startTime is the starting simulation's time.


        delete testBucket;
        //initialize result with the resources available in the first bucket.
        ResourceBucket *result = new ResourceBucket(*prevBucket);
        result->setStartTime(time);
        result->setEndTime(time + runtime);

        double accuTime = (*itfreeb)->getEndTime() - time;
        itfreeb++;

        while (accuTime < runtime and itfreeb != buckets[bId]->end()) {
            ResourceBucket *freeBucket = (*itfreeb);
            //TODO: Change the behaviour of /= and use a separate function for this. It'd be less confusing.
            *result /= *freeBucket; // '/=' operator returns the common minimum of the free resources.

            //If by *itfreeb any job has been released and it has more resources,
            //mark it as next possible bucket to start looking if with current starting point it is not enough
            if (*freeBucket > *result)
                result->setTimeNextRelease(freeBucket->getStartTime());

            //we move accumulated time forward.
            accuTime += freeBucket->getEndTime() - freeBucket->getStartTime();
            itfreeb++;
        }

        if (result->getTimeNextRelease() == -1 and itfreeb != buckets[bId]->end()) //if no release of resourecs was found, use start of next bucket to move on
            result->setTimeNextRelease((*itfreeb)->getStartTime());

        //No need to check. If we hit the last bucket, then accuTime will be always greater.
        //assert(accuTime >= runtime);

        result->setID(bId);
        return result;
    }

    /**
     * Returns the number of processors that are currently used by running jobs.
     * @return The number of used processors
     */
    uint32_t ResourceReservationTable::getNumberCPUsUsed() {
        uint32_t used = 0;

        for (map<Job*, ResourceJobAllocation*>::iterator allocIter = this->JobAllocationsMapping.begin();
                allocIter != this->JobAllocationsMapping.end();
                ++allocIter
                ) {
            Job* allocatedJob = allocIter->first;

            if (allocatedJob->getJobSimStatus() == RUNNING) {
                used += allocatedJob->getNumberProcessors();
            }
        }

        return used;

    }

    /**
     * Deletes all the buckets used by a given allocation, it is an auxiliar function.
     * @param allocationForCheckingReq  The allocation to free.
     */
    void ResourceReservationTable::freeAllocation(ResourceJobAllocation* allocationForCheckingReq) {
        /* we must free all the used memory that won't be used later */
        deque< ResourceBucket * >::iterator iter = allocationForCheckingReq->getAllocations().begin();
        for (; iter != allocationForCheckingReq->getAllocations().end(); ++iter) {
            ResourceBucket* bucket = *iter;
            delete bucket;

        }

        delete allocationForCheckingReq;
    }

    /**
     * Returns the reference to the buckets
     * @return A reference to the buckets
     */
    vector<BucketSetOrdered*>* ResourceReservationTable::getBuckets() {
        return &this->buckets;
    }

    /**
     * This function returns a BucketSetOrdered that contains the first freeBucket
     * available at startTime whose length >= minLength for each Cpu - extra resource requirements
     *
     * @param startTime The start time from when the job has to start to explore
     * @param minLength The minimum length required for the selected buckets
     * @return The set of buckets that matches the criteria
     */
    ResourceJobAllocation* ResourceReservationTable::findPossibleExtraAllocation(Job* job, double time, double length) {
        ResourceJobAllocation *allocation = new ResourceJobAllocation();
        allocation->setStartTime(time);
        allocation->setEndTime(time + length);
        std::deque<ResourceBucket*> possibleBuckets;

        if (job->getNumberProcessors() == job->getMaxProcessors()) {
            allocation->setAllocationProblem(true);
            return allocation;
        }

        //First fill the job requirements.
        ResourceRequirements rr = {
            .cpus = 1,
            .cpusPerNode = 0, //job->getCPUsPerNode(),
            .mem = 0, //(job->getRequestedMemory() == -1)? 0: job->getRequestedMemory(),
            .memPerNode = 0, //job->getMemPerNode(),
            .disk = 0, //(job->getDiskUsed() == -1)? 0: job->getDiskUsed(),
            .diskPerNode = 0, //job->getDiskPerNode(),
            //TODO: Not used for the moment.
            .diskBW = 0.0,
            .memBW = 0.0,
            .fabBW = 0.0
        };

        //Job requirements will be compared against freeResources after the allocation is built.
        ResourceRequirements freeResources = {
            .cpus = 0, .cpusPerNode = 0, .mem = 0, .memPerNode = 0,
            .disk = 0, .diskPerNode = 0, .diskBW = 0.0, .memBW = 0.0, .fabBW = 0.0
        };

        assert(rr.diskBW == 0 and rr.memBW == 0 and rr.fabBW == 0);
        double nextStartTime = -1;
        for (uint32_t i = 0; i < buckets.size(); i++) {
            ResourceBucket* current = findFirstBucket(time, length, i);
            assert(current->getStartTime() != -1 and current->getEndTime() != -1);
            double nextRelease = current->getTimeNextRelease();
            if (nextStartTime > -1 && nextRelease > -1)
                nextStartTime = (nextStartTime < nextRelease) ? nextStartTime : nextRelease;
            else if (nextRelease > -1)
                nextStartTime = nextRelease;

            if (rr.cpusPerNode > current->getFreeCPUs()) continue;
            if (rr.memPerNode > current->getFreeMemory()) continue;

            freeResources.cpus += current->getFreeCPUs();
            freeResources.mem += current->getFreeMemory();
            freeResources.memBW += current->getFreeMemoryBW();
            freeResources.disk += current->getFreeDisk();
            freeResources.diskBW += current->getFreeDiskBW();
            freeResources.fabBW += current->getFreeFabricBW();
            possibleBuckets.push_back(current);

            if (current->getFreeDiskBW() != 0.0 or current->getFreeMemoryBW() != 0.0 or current->getFreeFabricBW() != 0.0) {
                std::cout << "[" << job->getJobNumber() << "] diskBW: " << current->getFreeDiskBW() << ", memBW: " << current->getFreeMemoryBW() << ", fabBW: " << current->getFreeFabricBW() << "." << std::endl;
                assert(false);
            }
            //log->debug("findFirstBucketCpus: Adding the CPU "+to_string(current->getCpu())+" to the suitableBuckets list whom start time is "+ftos(current->getStartTime())+" and whom endTime is "+ftos(current->getEndTime())+" and bucket id "+to_string(current->getId()),4);
        }

        if (freeResources.diskBW != 0.0 or freeResources.memBW != 0.0 or freeResources.fabBW != 0.0) {
            std::cout << "[" << job->getJobNumber() << "] diskBW: " << freeResources.diskBW << ", memBW: " << freeResources.memBW << ", fabBW: " << freeResources.fabBW << "." << std::endl;
            assert(false);
        }

        allocation->setNextStartTimeToExplore(nextStartTime);
        //Check if the requirements were met.
        if (freeResources < rr) {
            allocation->setAllocationProblem(true);
            allocation->setAllocationReason(NOT_ENOUGH_RESOURCES);
            log->debug("Requirements could not be met for job " + to_string(job->getJobNumber()), 1);
        } else {
            log->debug("Requirements were met for job " + to_string(job->getJobNumber()), 1);
            allocation->setAllocations(possibleBuckets);
        }
        return allocation;
    }


    // Malleability

    bool ResourceReservationTable::expandJobAllocation(Job* job, JobAllocation* jobAlloc) {
        /*it's time to convert it to analogic reservation*/
        deque<ResourceBucket*> allocations = ((ResourceJobAllocation*) jobAlloc)->getAllocations();
        /* it's time to allocate the job to the reservation table !*/
        deque<ResourceBucket*>::iterator it = allocations.begin();
        assert(allocations.size() > 0);

        /* Find the old allocation */
        map<Job*, ResourceJobAllocation*>::iterator itcheck = this->JobAllocationsMapping.find(job);
        //assert(itcheck != this->JobAllocationsMapping.end()); //check that the job is already allocated.
        ResourceJobAllocation* oldJobAlloc = itcheck->second;
        deque<ResourceBucket*> oldAllocations = oldJobAlloc->getAllocations();
        //cout << "OldAllocation size:" << ((ResourceJobAllocation*)oldJobAlloc)->getAllocations().size() <<endl;
        for (; it != allocations.end(); ++it) {
            ResourceBucket* current = *it;
            //int bId = current->getID();
            uint32_t bId = current->getID();
            //if (bId == 7)
            //				cout << "bId 7 in expansion for job " << job->getJobNumber() << endl;
            //if (job->getJobNumber()==721)
            //	cout << "Allocing for job " << job->getJobNumber() << " BId = " << bId << endl;
            /*
            if (job->getJobNumber() == 721) {
                            cout << "OldPair : ";
                            for (deque <ResourceBucket *>::iterator it = allocations.begin(); it != allocations.end(); ++it) {
                                            ResourceBucket* currentAlloc = *it;
                                            int bId = currentAlloc->getID();
                                            BucketSetOrdered::iterator itlow = findLowerBound(buckets[bId], currentAlloc);
                                            BucketSetOrdered::iterator itup = findUpperBound(buckets[bId], currentAlloc);
                                            ResourceBucket* deallocBucket = *itlow;
                                            uint32_t cpus = deallocBucket->getJobUsedCPUs(job);
                                            cout << ":bId=" << bId << ",cpus=" << cpus;
                            }
                            cout << endl;
            }
             */

            /* Check if bId is already a part of the old allocation */
            deque<ResourceBucket*>::iterator oit = oldAllocations.begin();
            int yes = 0;
            for (; oit != oldAllocations.end(); oit++) {
                ResourceBucket* oitcur = *oit;
                if (oitcur->getID() == bId) {
                    yes = 1;
                    break;
                }

            }

            if (!yes) {
                /* before do any stuff with the buckets we will find the upper and lower buckets */
                BucketSetOrdered::iterator bsit = this->findLowerBound(buckets[bId], current);
                ResourceBucket* prevBucket = *bsit;

                //When allocating a job only two buckets might need to be broken down: the first and the last. ONly if the new falls in the middle of those.
                double prevStart = prevBucket->getStartTime();
                double prevEnd = prevBucket->getEndTime();

                bool breakStart = false;
                //bool breakEnd = false;
                if (prevStart < current->getStartTime()) breakStart = true;
                //if (prevEnd > current->getEndTime() or prevEnd == -1) breakEnd = true;

                ReservationList rl = current->getResourceList();
                if (breakStart) {
                    ResourceBucket *newBucket = new ResourceBucket(*prevBucket);
                    prevBucket->setStartTime(current->getStartTime());
                    //prevBucket->allocateJob(job, rl);

                    newBucket->setEndTime(current->getStartTime() - 1);
                    buckets[bId]->insert(newBucket);
                }

                bool keep_alloc = (prevEnd <= current->getEndTime() and (prevEnd != -1));
                while (keep_alloc) {
                    if ((*bsit)->getEndTime() == -1) break;
                    assert(bsit != buckets[bId]->end());
                    assert((*bsit)->getEndTime() != -1);
                    (*bsit)->allocateJob(job, rl);
                    if ((*bsit)->getEndTime() == current->getEndTime()) break; //We know we reached the last bucket to allocate.
                    bsit++;
                    keep_alloc = ((*bsit)->getEndTime() <= current->getEndTime() and (prevEnd != -1));
                }

                if ((*bsit)->getEndTime() > current->getEndTime() or (*bsit)->getEndTime() == -1) { //need to break the bucket by the end
                    ResourceBucket *newBucket = new ResourceBucket(*(*bsit));
                    newBucket->setEndTime(current->getEndTime());
                    assert(newBucket->getEndTime() != -1);
                    newBucket->allocateJob(job, rl);
                    (*bsit)->setStartTime(current->getEndTime() + 1);

                    buckets[bId]->insert(bsit, newBucket);
                }
            } else {
                BucketSetOrdered::iterator bsit = this->findLowerBound(buckets[bId], current);
                ResourceBucket* prevBucket = *bsit;
                ReservationList rl = current->getResourceList();
                // ReservationList oldrl = (*oit)->getResourceList();

                if (prevBucket->getStartTime() == current->getStartTime()) {
                    prevBucket->allocateJob(job, rl);
                    //allocations.erase(it++);
                    //continue;
                } else {
                    ResourceBucket *newBucket = new ResourceBucket(*prevBucket);
                    newBucket->setStartTime(current->getStartTime());
                    newBucket->setEndTime(prevBucket->getEndTime());
                    prevBucket->setEndTime(current->getStartTime());
                    //newBucket->allocateJob(job, oldrl);
                    newBucket->allocateJob(job, rl);
                    buckets[bId]->insert(newBucket);
                }

                // this should find the newBucket we just inserted or find the prevBucket
                BucketSetOrdered::iterator itnext = this->findLowerBound(buckets[bId], current);
                itnext++;
                while (itnext != buckets[bId]->end()) {
                    if (not (*itnext)->expandJob(job, rl))
                        break;
                    itnext++;
                }

                /*
                BucketSetOrdered::iterator itnext = bsit;
                itnext++;
                while(itnext != buckets[bId]->end()) {
                        if(not (*itnext)->expandJob(job, rl))
                                break;
                        itnext++;
                }
                 */

            }
            /*
            if (job->getJobNumber() == 721) {
            cout << "NewPair : ";
            for (deque <ResourceBucket *>::iterator it = allocations.begin(); it != allocations.end(); ++it) {
                            ResourceBucket* currentAlloc = *it;
                            int bId = currentAlloc->getID();
                            BucketSetOrdered::iterator itlow = findLowerBound(buckets[bId], currentAlloc);
                            BucketSetOrdered::iterator itup = findUpperBound(buckets[bId], currentAlloc);
                            ResourceBucket* deallocBucket = *itlow;
                            uint32_t cpus = deallocBucket->getJobUsedCPUs(job);
                            cout << ":bId=" << bId << ",cpus=" << cpus;
            }
            cout << endl;
            }*/
        }
        //((ResourceJobAllocation*)jobAlloc)->setAllocations(allocations);
        ((ResourceJobAllocation*) oldJobAlloc)->addAllocations(allocations);

        return true;
    }

    bool ResourceReservationTable::shrinkJobAllocation(Job* job, int scpus) {
        //log->debug("[ResourceReservationTable] deallocating job " + to_string(job->getJobNumber()),6);
        map<Job*, ResourceJobAllocation*>::iterator allocIter = this->JobAllocationsMapping.find(job);
        if (allocIter == this->JobAllocationsMapping.end()) {
            cout << "Shrinking error: job " << job->getJobNumber() << " not found in JobAllocationsMappting " << endl;
            return false;
        }
        assert(allocIter != this->JobAllocationsMapping.end());
        ResourceJobAllocation* allocation = allocIter->second;

        assert(allocation != 0);

        /*
     the steps to do are the following:
     1- Free the buckets used by this job in each cpu in the allocations buckets
     2- Free the same buckets from the global view
     3- Add the buckets to the free area and update the global view
         */

        //if (job->getJobNumber() == 721)
        //				cout << "starting lscpus = " << scpus << endl;
        int lscpus = scpus;

        deque<ResourceBucket*> allocs = allocation->getAllocations();

        /*
        if (job->getJobNumber() == 721) {
                        cout << "OldPair : ";
                        for (deque <ResourceBucket *>::iterator it = allocs.begin(); it != allocs.end(); ++it) {
                                        ResourceBucket* currentAlloc = *it;
                                        int bId = currentAlloc->getID();
                                        BucketSetOrdered::iterator itlow = findLowerBound(buckets[bId], currentAlloc);
                                        BucketSetOrdered::iterator itup = findUpperBound(buckets[bId], currentAlloc);
                                        ResourceBucket* deallocBucket = *itlow;
                                        uint32_t cpus = deallocBucket->getJobUsedCPUs(job);
                                        cout << ":bId=" << bId << ",cpus=" << cpus;
                        }
                        cout << endl;
        }*/


        for (deque< ResourceBucket * >::iterator it = allocs.begin(); it != allocs.end();) {

            ResourceBucket* currentAlloc = *it; //Current does not correspond to the bucket in the RTand it is only a hint for where to start looking to deallocate.

            int bId = currentAlloc->getID();
            int remcpus = 0;
            //cout << " Deallocate job " << job->getJobNumber() << " bId = " << bId << endl;
            BucketSetOrdered::iterator itlow = findLowerBound(buckets[bId], currentAlloc);
            BucketSetOrdered::iterator itup = findUpperBound(buckets[bId], currentAlloc);

            ResourceBucket* deallocBucket = *itlow;
            int lscpus_thisiter = lscpus;
            assert(deallocBucket->shrinkJob(job, lscpus, remcpus));
            if (job->getJobNumber() == 721)
                cout << " Deallocate BId " << bId << " success. lscpus = " << lscpus << " remcpus = " << remcpus << endl;
            if (remcpus == 0) {
                //Job may be present in several buckets if the initial was broken down.
                //deallocate forward until the job is not present in the bucket.
                BucketSetOrdered::iterator itnext = itlow;
                itnext++;
                while (itnext != buckets[bId]->end()) {
                    if (not (*itnext)->deallocateJob(job))
                        break;
                    itnext++;
                }

                //Once the job's been deallocated from every bucket, the previous and the next (counting from currentAlloc)
                //are susceptible to be joined in case the only difference between them was just the deallocated job.
                assert(itlow != buckets[bId]->rend().base()); //shouldn't happen since no job starts at time 0 and there's a bucket which does.
                itlow--;
                ResourceBucket* prevBucket = *itlow;
                ResourceBucket* nextBucket = *itup; //next bucket after currentAlloc finishes.
                ResourceBucket* nextPrevBucket = *(--itup); //Previous bucket to nextBucket. Could be dealloc but the same applies.

                bool joinPrev = (*prevBucket == *deallocBucket);
                bool joinEnd = (*nextPrevBucket == *nextBucket);

                if (joinPrev) {
                    assert(prevBucket != deallocBucket);
                    prevBucket->setEndTime(deallocBucket->getEndTime());
                }
                if (joinEnd) {
                    assert(nextBucket != deallocBucket); //It doesn't make sense to have this assert this this can happen.
                    if (nextPrevBucket == deallocBucket and joinPrev) { //we're actually joining prev and next buckets.
                        prevBucket->setEndTime(nextBucket->getEndTime());
                    } else {
                        nextPrevBucket->setEndTime(nextBucket->getEndTime());
                    }
                    buckets[bId]->erase(nextBucket);
                }

                if (joinPrev) buckets[bId]->erase(deallocBucket);

                allocs.erase(it++);

            } else {
                BucketSetOrdered::iterator itnext = itlow;
                itnext++;
                while (itnext != buckets[bId]->end()) {
                    int tmplscpus = lscpus_thisiter;
                    if (not (*itnext)->shrinkJob(job, tmplscpus, remcpus))
                        break;
                    itnext++;
                }
                ++it;
            }

            if (lscpus == 0)
                break;
        }

        if (lscpus != 0) {
            cout << " Shrinking error: job << " << job->getJobNumber() << " lscpus did not go to 0, lscpus = " << lscpus << endl;
            return false;
        }


        allocation->setAllocations(allocs);

        deque<ResourceBucket*>::iterator it = allocs.begin();
        /*
        if (job->getJobNumber() == 721) {
                        cout << "Pair : ";
                        for (deque <ResourceBucket *>::iterator it = allocs.begin(); it != allocs.end(); ++it) {
                                        ResourceBucket* currentAlloc = *it;
                                        int bId = currentAlloc->getID();
                                        BucketSetOrdered::iterator itlow = findLowerBound(buckets[bId], currentAlloc);
                                        BucketSetOrdered::iterator itup = findUpperBound(buckets[bId], currentAlloc);
                                        ResourceBucket* deallocBucket = *itlow;
                                        uint32_t cpus = deallocBucket->getJobUsedCPUs(job);
                                        cout << ":bId=" << bId << ",cpus=" << cpus;
                        }
                        cout << endl;
        }*/



        if (it == allocs.end()) {
            cout << " shrinking error: job " << job->getJobNumber() << " doesn'teem to have any more allocations " << endl;
            return false;
        }

        return true;
    }



}
