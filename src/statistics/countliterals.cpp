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
#include <statistics/countliterals.h>

#include <algorithm>
#include <map>

using std::map;

namespace Statistics {

/**
 * The default constructor for the class
 */
CountLiterals::CountLiterals()
{
  this->values = NULL;
}

/**
 * The default destructor for the class 
 */
CountLiterals::~CountLiterals()
{
}

/**
 * Function that compute the number of times that each value is repeated in the set, the metric is returned as string. With the format X=2, Z=34 etc.
 * @return Metric with the count of literals 
 */
 
Simulator::Metric* CountLiterals::computeValue()
{
  map<string,int> literalCount;
  
  for(vector<string>::iterator itv = this->values->begin();itv != this->values->end();++itv)
  {
    string st = *itv;
    
    map<string,int>::iterator inserted = literalCount.find(st);
    
    if(inserted == literalCount.end())
      literalCount.insert(pair<string,int>(st,1));
    else
      literalCount[st] = literalCount[st]+1;
  }

  string ret  = "";
  
  //now we create the string and count them 
  for(map<string,int>::iterator itc = literalCount.begin(); itc != literalCount.end();++itc)
  {
    string literal = itc->first;
    string count = to_string(itc->second);
    ret+=literal+"="+count+";";
  }
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::COUNT_LITERALS);    
  performance->setNativeString(ret);
  performance->setNativeType(Simulator::STRING);
  
  return performance;
  
}



/**
 * Returns a reference to the vector that holds all the values to whom the stimator will be computed 
 * @return The reference to the vector of strings 
 */
vector< string >* CountLiterals::getSvalues() const
{
  return values;
}


/**
 * Sets the reference to the vector that holds all the values to whom the stimator will be computed  
 * @param theValue The reference to the vector of strings 
 */
void CountLiterals::setSValues(vector< string >* theValue)
{
  values = theValue;
}


}
