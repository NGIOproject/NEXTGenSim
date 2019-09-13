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
#ifndef STATISTICSSTATISTICALESTIMATOR_H
#define STATISTICSSTATISTICALESTIMATOR_H

#include <scheduling/metric.h>


namespace Statistics {

/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
* This class implements an statistical estimator. Given a set of values it  will compute a given statistic.
*/
class StatisticalEstimator{
public:
  StatisticalEstimator();
  virtual ~StatisticalEstimator();
  void setMetricType(const Simulator::metric_t& theValue);
  Simulator::metric_t getMetricType() const;
  void setStatisticType(const Simulator::statistic_t& theValue);
  Simulator::statistic_t getstatisticType() const;
  /**
   * Function that computes the statistic to the current set values 
   * @return A reference to the metric class that contains the statistic value (the metric object should be destroyed by the object that requires it)
   */
  virtual Simulator::Metric* computeValue() = 0;	

private:
  Simulator::metric_t metricType; /**< the metric type @see the Metric Class for the simulator (the semantic type for the date being computed ) */
  Simulator::statistic_t statisticType; /**< The statistic type that the statististic implements, @see Metric class  */

};

}

#endif
