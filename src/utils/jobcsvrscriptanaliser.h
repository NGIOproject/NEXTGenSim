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
#ifndef UTILSJOBCSVRSCRIPTANALISER_H
#define UTILSJOBCSVRSCRIPTANALISER_H

#include <utils/rscript.h>
#include <utils/architectureconfiguration.h>

namespace Utils {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class JobCSVRScriptAnaliser : public RScript{
public:
  JobCSVRScriptAnaliser(string CSVToAnalyse,string ScriptPath,string OutPutDir,ArchitectureConfiguration* arch, Log* log,string ScriptsDir,string RBinaryPath,string ScriptsBaseDir);
  ~JobCSVRScriptAnaliser();

  virtual void analyze();

private: 
  string CSVToAnalyse; /**< File path of the CSV file to be analysed */
  string ScriptsDir; /**< File path for the scripts dir where to generate the temporary R scripts and where the original auxiliar R scripts are located*/
};

}

#endif
