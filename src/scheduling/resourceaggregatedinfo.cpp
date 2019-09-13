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
#include <scheduling/resourceaggregatedinfo.h>

namespace Simulator {

ResourceAggregatedInfo::ResourceAggregatedInfo()
{
 this->Attributes = new vector<Metric*>();
}


ResourceAggregatedInfo::~ResourceAggregatedInfo()
{

  //we free the metrics 
  for(vector<Metric*>::iterator it = this->Attributes->begin(); it != Attributes->end();++it)
  {
    Metric* met = *it;
    delete met; 
  }
  delete this->Attributes;
}

/**
 * Returns the Attributes
 * @return A vector reference containing the differents attributes that are configured in the current agregated method
 */
vector< Metric * >* ResourceAggregatedInfo::getAttributes() const
{
	return Attributes;
}

/**
 * Sets the Attributes to the job
 * @param theValue The Attributes
 */
void ResourceAggregatedInfo::setAttributes ( vector< Metric * >* theValue )
{
	Attributes = theValue;
}

/**
 * Returns the ResourceName
 * @return A string containing the ResourceName
 */
string ResourceAggregatedInfo::getResourceName() const
{
	return ResourceName;
}

/**
 * Sets the ResourceName to the job
 * @param theValue The ResourceName
 */
void ResourceAggregatedInfo::setResourceName ( const string& theValue )
{
	ResourceName = theValue;
}


/**
 * Returns the ResourceType
 * @return A string containing the ResourceType
 */
string ResourceAggregatedInfo::getResourceType() const
{
	return ResourceType;
}

/**
 * Sets the ResourceType to the job
 * @param theValue The ResourceType
 */
void ResourceAggregatedInfo::setResourceType ( const string& theValue )
{
	ResourceType = theValue;
}

}
