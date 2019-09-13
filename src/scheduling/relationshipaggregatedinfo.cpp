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
#include <scheduling/relationshipaggregatedinfo.h>

namespace Simulator {

RelationshipAggregatedInfo::RelationshipAggregatedInfo()
{
}


RelationshipAggregatedInfo::~RelationshipAggregatedInfo()
{
}

/**
 * Returns the SourceType
 * @return A string containing the SourceType
 */
string RelationshipAggregatedInfo::getSourceName() const
{
	return SourceName;
}

/**
 * Sets the SourceName to the job
 * @param theValue The SourceName
 */
void RelationshipAggregatedInfo::setSourceName ( const string& theValue )
{
	SourceName = theValue;
}


/**
 * Returns the SourceType
 * @return A string containing the SourceType
 */
string RelationshipAggregatedInfo::getSourceType() const
{
	return SourceType;
}

/**
 * Sets the SourceType to the job
 * @param theValue The SourceType
 */
void RelationshipAggregatedInfo::setSourceType ( const string& theValue )
{
	SourceType = theValue;
}

/**
 * Returns the TargetName
 * @return A string containing the TargetName
 */
string RelationshipAggregatedInfo::getTargetName() const
{
	return TargetName;
}

/**
 * Sets the TargetName to the job
 * @param theValue The TargetName
 */
void RelationshipAggregatedInfo::setTargetName ( const string& theValue )
{
	TargetName = theValue;
}

/**
 * Returns the TargetType
 * @return A string containing the SourceType
 */
string RelationshipAggregatedInfo::getTargetType() const
{
	return TargetType;
}

/**
 * Sets the TargetType to the job
 * @param theValue The TargetType
 */
void RelationshipAggregatedInfo::setTargetType ( const string& theValue )
{
	TargetType = theValue;
}

}
