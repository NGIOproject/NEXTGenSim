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
#include <statistics/accumulate.h>

#include <algorithm>
#include <numeric>

namespace Statistics {

    /**
     * The default constructor for the class
     */
    Accumulate::Accumulate() {
        this->ivalues = NULL;
        this->dvalues = NULL;
    }

    /**
     * The default destructor for the class
     */
    Accumulate::~Accumulate() {
    }

    /**
     * Function that computes the accumulate of all the values stored in the vector
     * @return Metric with the accumulate
     */
    Simulator::Metric* Accumulate::computeValue() {

        double dvalue = 0.0;
        int ivalue = 0;
        Simulator::Metric* performance = new Simulator::Metric();

        if (this->ivalues != NULL) {

            ivalue = accumulate(this->ivalues->begin(), this->ivalues->end(), 0);
            performance->setNativeType(Simulator::INTEGER);
            performance->setNativeValue(ivalue);

        } else {

            dvalue = accumulate(this->dvalues->begin(), this->dvalues->end(), 0.0);
            performance->setNativeType(Simulator::DOUBLE);
            performance->setNativeValue(dvalue);

        }

        performance->setStatisticUsed(Simulator::ACCUMULATE);
        return performance;

    }

    void Accumulate::setValues(vector< double >* theValue) {
        setdValues(theValue);
    }

    void Accumulate::setValues(vector< int >* theValue) {
        setiValues(theValue);
    }

    /**
     * Returns a reference to the vector that holds all the values to whom the estimator will be computed
     * @return The reference to the vector
     */
    vector< double >* Accumulate::getdValues() const {
        return dvalues;
    }

    /**
     * Sets the reference to the vector that holds all the values to whom the estimator will be computed
     * @param theValue The reference to the vector
     */
    void Accumulate::setdValues(vector< double >* theValue) {
        dvalues = theValue;
    }

    /**
     * Returns a reference to the vector that holds all the values to whom the estimator will be computed
     * @return The reference to the vector
     */
    vector< int >* Accumulate::getiValues() const {
        return ivalues;
    }

    /**
     * Sets the reference to the vector that holds all the values to whom the estimator will be computed
     * @param theValue The reference to the vector
     */
    void Accumulate::setiValues(vector< int >* theValue) {
        ivalues = theValue;
    }

}
