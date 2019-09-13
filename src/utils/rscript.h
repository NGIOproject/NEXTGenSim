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
#ifndef UTILSRSCRIPT_H
#define UTILSRSCRIPT_H

#include <utils/executecommand.h>
#include <utils/log.h>
#include <utils/configurationfile.h>

namespace Utils {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** Function that executes a R script that analyse a given CSV file */
class RScript : public ExecuteCommand{
public:
  RScript(string command,string params,bool outputRequired,Log* log, vector<string>* headerFile,string tempDir);
  RScript();
  ~RScript();
  
  /**
   * This functions is the responsible of carry out the required analysis. Its is virtual since all the subclasses must implement it behaviour
   */
  virtual void analyze() = 0;

  void setScriptType(const r_scripts_type& theValue);
  r_scripts_type getscriptType() const;
	
    
protected:
  vector<string>* headerFile; /**< A set of vector fields indicating the lines that have to be put inside the header R file, this in case required.. usually this vector is used for setup some R variables that are used for the simulator. This mainly depends on the R script used */
  
  r_scripts_type scriptType;
    
};

}

#endif
