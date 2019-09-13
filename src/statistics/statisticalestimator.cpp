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
#include <statistics/statisticalestimator.h>

namespace Statistics {

/**
 * The default constructor for the class
 */
StatisticalEstimator::StatisticalEstimator()
{
}


/**
 * The default destructor for the class 
 */
StatisticalEstimator::~StatisticalEstimator()
{
}


/**
 * Returns the metric type of the values that are stored in the estimator
 * @return The estatistic type
 */
Simulator::metric_t StatisticalEstimator::getMetricType() const
{
  return metricType;
}


/**
 * Sets the metric type of the values that are stored in the estimator
 * @param theValue The metric type for the current statisticalestimator
 */
void StatisticalEstimator::setMetricType(const Simulator::metric_t& theValue)
{
  metricType = theValue;
}


/**
 * Returns the statisticType type that the current statisticalestimator implements 
 * @return The statisticType implemented 
 */
Simulator::statistic_t StatisticalEstimator::getstatisticType() const
{
  return statisticType;
}


/**
 * Sets the type that the current statisticalestimator implements 
 * @param theValue 
 */
void StatisticalEstimator::setStatisticType(const Simulator::statistic_t& theValue)
{
  statisticType = theValue;
}

}

