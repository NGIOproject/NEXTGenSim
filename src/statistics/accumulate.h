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
#ifndef STATISTICSACCUMULATE_H
#define STATISTICSACCUMULATE_H

#include <statistics/statisticalestimator.h>

#include <vector>

using namespace std;
using std::vector;

namespace Statistics {

    /**
     * @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
     */

    /** 
     * Implements the statistic of Accumulate- given a set of values returns the adding.
     * @see the statisticalestimator class
     */
    class Accumulate : public StatisticalEstimator {
    public:
        Accumulate();
        virtual ~Accumulate();

        void setValues(vector< double >* theValue);
        void setValues(vector< int >* theValue);
        virtual Simulator::Metric* computeValue();
        void setdValues(vector< double >* theValue);
        vector< double >* getdValues() const;
        void setiValues(vector< int >* theValue);
        vector< int >* getiValues() const;




    private:
        vector<double>* dvalues; /**<  The reference to the vector that hols all the values to whom the stimator will be computed  */
        vector<int>* ivalues;

    };

}

#endif
