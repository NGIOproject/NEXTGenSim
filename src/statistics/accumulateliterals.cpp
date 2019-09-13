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
#include <statistics/accumulateliterals.h>
#include <utils/utilities.h>
#include <statistics/accumulate.h>

#include <algorithm>
#include <map>

using std::map;

namespace Statistics {

/**
 * The default constructor for the class. By default currently the accumulate statisticall estimator is implemented (that will be applied to each group)
 */
AccumulateLiterals::AccumulateLiterals()
{
  this->valuesSP = NULL;
  this->accumulateToCompute = ACCUMULATE;
}

/**
 * The default destructor for the class 
 */
AccumulateLiterals::~AccumulateLiterals()
{
}


/**
 * Function that compute the statistic of all the values stored in the hashmap group by the key
 * @return Metric with the accumulate by group 
 */
Simulator::Metric* AccumulateLiterals::computeValue()
{
  map<string,double> literalCount;
  
  for(map<string,vector<double>*>::iterator itv = this->valuesSP->begin();itv != this->valuesSP->end();++itv)
  {
    string st = itv->first;
    vector<double>* vect = itv->second;
    
    double amount = 0;
    
    switch(this->accumulateToCompute)
    {
      case ACCUMULATE: 
      { 
        Accumulate * est = new Accumulate();
        est->setValues(vect); 
        Metric* res = est->computeValue();
        amount = res->getnativeDouble(); 
        delete est;
        break;
      }
      default:
        assert(false);
    }
    
    map<string,double>::iterator inserted = literalCount.find(st);
    
    if(inserted == literalCount.end())
      literalCount.insert(pair<string,double>(st,amount));
    else
      assert(false);
  }

  string ret  = "";
  
  //now we create the string and count them 
  for(map<string,double>::iterator itc = literalCount.begin(); itc != literalCount.end();++itc)
  {
    string literal = itc->first;
    string count = ftos(itc->second);
    ret+=literal+"="+count+";";
  }
  
  Simulator::Metric* performance = new Simulator::Metric();
  performance->setStatisticUsed(Simulator::ACCUMULATE_LITERALS);    
  performance->setNativeString(ret);
  performance->setNativeType(Simulator::STRING);
  
  return performance;
  
}


/**
 * Returns a reference to the hahmap of vector that holds all the values and groups to whom the stimator will be computed 
 * @return The reference to the vector 
 */
map<string,vector<double>*>* AccumulateLiterals::getSPvalues() const
{
  return valuesSP;
}

/**
 * Sets the reference to the hashmap that holds all the values and groups to whom the stimator will be computed  
 * @param theValue The reference to the vector 
 */
void AccumulateLiterals::setSPValues(map<string,vector<double>*>* theValue)
{
  valuesSP = theValue;
}


}
