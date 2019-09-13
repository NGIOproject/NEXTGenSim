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
#include <datamining/discretizer.h>
#include <assert.h>

namespace Datamining {

/**
 * @param Log A reference to the logging engine 
 */
Discretizer::Discretizer(Log* log)
{  
  this->log = log;
}


/**
 * The destructor for the class 
 */
Discretizer::~Discretizer()
{
}

/**
 * Retruns the discretizer type implemented by the current class 
 * @return A discretizer_t indicating the discretizer type implemented 
 */
discretizer_t Discretizer::getdiscretizerType() const
{
  return discretizerType;         
}


/**
 * Sets the discretizer type implementented by the current class 
 * @param theValue The discretizer type
 */
void Discretizer::setDiscretizerType(const discretizer_t& theValue)
{
  discretizerType = theValue;
}

/**
 * Given a discretizer_t type returns an string containing a human readable discretizer type
 * @param discretizer The discretizer type 
 * @return A string containing a human readable discretizerType.
 */
string Discretizer::getDiscretizerName(discretizer_t discretizer)
{
  switch(discretizer)
  {
    case SAME_INTERVAL_DISCRETIZER:
      return "SAME_INTERVAL_DISCRETIZER";
    case SAME_INSTANCES_PER_INTERVAL_DISCRETIZER:
      return "SAME_INSTANCES_PER_INTERVAL_DISCRETIZER";
    case OTHER_DISCRETIZER:
      return "OTHER_DISCRETIZER";
    default:
      assert(false);
  }
}


}
