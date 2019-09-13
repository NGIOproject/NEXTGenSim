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
#include <utils/csvpolicyinfoconverter.h>

namespace Utils {

/**
 * The default constructor for the class
 * @param FilePath The file path where to store the cvs file 
 * @param log The logging engine 
 */
CSVPolicyInfoConverter::CSVPolicyInfoConverter(string FilePath,Log* log) : CSVConverter(FilePath,log)
{

  this->type = CSV_JOBS_INFO_CONVERTER;

  this->CSVHeaderFields = new vector<string>;
  
  this->CSVHeaderFields->push_back("TimeStamp");
  this->CSVHeaderFields->push_back("NumberJobsInTheQueue");
  this->CSVHeaderFields->push_back("NumberCpusUsed");  
  this->CSVHeaderFields->push_back("BackfilledJobs");  
  this->CSVHeaderFields->push_back("PendingWorks");  
  this->CSVHeaderFields->push_back("RunningJobs");
  this->CSVHeaderFields->push_back("Center");
  

  
  this->fieldTypes = new vector<file_field_t>;
  
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(DOUBLE_FIELD);
  this->fieldTypes->push_back(STRING_FIELD);

}

/**
 * The default destructor for the class 
 */
CSVPolicyInfoConverter::~CSVPolicyInfoConverter()
{
}

/**
 * Given the set of sampling for the center create the cvs output file 
 * @param collector The policy entity collector
 */
void CSVPolicyInfoConverter::createPolicyCSVInfoFile(PolicyEntityCollector* collector)
{
  this->open();
  
  int numEntries = collector->getnumberJobsInTheQueue()->size();
  
  for(int i = 0; i < numEntries; i++)
  {
     vector<string>* entry = new vector<string>;
     
     string stamp = ftos(collector->gettimeStamp()->operator[](i));
     string JobsWQ = ftos(collector->getnumberJobsInTheQueue()->operator[](i));
     string CpusUsed = ftos(collector->getNumberCPUsUsed()->operator[](i));
     string bkJobs = ftos(collector->getbackfilledJobs()->operator[](i));
     string lfW = ftos(collector->getleftWork()->operator[](i));
     string rjobs = ftos(collector->getrunningJobs()->operator[](i));
     string center = collector->getcenterName()->operator[](i);
                         
     entry->push_back(stamp);
     entry->push_back(JobsWQ);
     entry->push_back(CpusUsed);
     entry->push_back(bkJobs);
     entry->push_back(lfW);    
     entry->push_back(rjobs);
     entry->push_back(center);
     
     CSVConverter::addEntry(entry);
  }
  
  this->close();
}




}
