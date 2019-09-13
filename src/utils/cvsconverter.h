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
#ifndef UTILSCSVCONVERTER_H
#define UTILSCSVCONVERTER_H

#include <string>
#include <vector>

#include <assert.h>
#include <utils/utilities.h>
#include <utils/configurationfile.h>
#include <time.h>
#include <fstream>
#include <utils/log.h>
#include <scheduling/job.h>

using namespace std;
using std::vector;
using namespace Simulator;

namespace Utils {


/**
* @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Implements a general cvs generator */
class CSVConverter{
public:
  CSVConverter(string FilePath,Log* log);
  virtual ~CSVConverter();
  
  /* sets and gets*/
  void setCSVHeaderFields(vector< string >* theValue);
  vector< string >* getCSVHeaderFields() const;
  void setFieldTypes(vector< file_field_t >* theValue);
  vector< file_field_t >* getfieldTypes() const;	
	 
  /* functions regarding the cvs file management */
  void open();
  virtual void addEntry(Job* job);
  virtual void addEntry(vector<string>* fields);
  void close(); 

  void setType(const csv_converter_t& theValue);
  csv_converter_t getType() const;

  void setFilePath(const string& theValue);
  string getFilePath() const;
	
	

protected:
  string FilePath; /**< The output file for the cvs  */
  vector<string>* CSVHeaderFields;/**< The vector containing the different columns names  for the cvs output file  */
  vector<file_field_t>* fieldTypes;/**<  The vector containing the different columns types for the cvs output file */
 
  std::ofstream* cvsFile;/**< The stream for the output file */
  Log* log;/**< The logging engine */
  bool opened;/**< Indicates if the output file is opened*/
  string separator;/**< The separator between the fields of the cvs */

  csv_converter_t type; /**< Indicates the converter cvs converter type implemented by the current instance  **/
};

}

#endif
